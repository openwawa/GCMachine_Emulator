#include "clientpool.h"
#include "ui_clientpool.h"
#include <string>
#include <iostream>
clientPool::clientPool(ConfigInfo &config,QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::clientPool),configInfo(config)
{
    ui->setupUi(this);
    logger=new Logger(ui->textEdit_log,true,true,parent);
    logger->log("初始化界面");
    currentDevIndex=-1;     //当前选中设备标签初始化为-1
    isRunning=false;
    init_configInfo();
    init_devList();

    connect(ui->pushButton_saveConfig,&QPushButton::clicked,this,&clientPool::save_configInfo);
    connect(this,&clientPool::sig_log,logger,&Logger::log);
    connect(ui->pushButton_start,&QPushButton::clicked,this,&clientPool::handle_clickStartBtn);
    connect(ui->listWidget_dev,&QListWidget::itemClicked,this,&clientPool::handle_devListIndex_changed);
    connect(ui->pushButton_devStart,&QPushButton::clicked,this,&clientPool::handle_start_dev);  //在界面点击启动某个设备
    connect(ui->pushButton_setParam,&QPushButton::clicked,this,&clientPool::handle_clickSetParam);
    connect(ui->pushButton_status,&QPushButton::clicked,this,&clientPool::handle_clickGetDevStatus);

}

clientPool::~clientPool()
{
    delete ui;
    delete logger;
}

int clientPool::init_configInfo()
{
    configInfo.read_clientConfigFile();  //获取配置文件中保存的信息
    //将读取的配置信息初始化界面中的配置

    ui->lineEdit_serverIP->setText(configInfo.get_configData(SERVER_IP));
    ui->lineEdit_serverPort->setText(configInfo.get_configData(SERVER_PORT));
    ui->lineEdit_startDevId->setText(configInfo.get_configData(START_DEVID));
    ui->lineEdit_devNum->setText(configInfo.get_configData(DEVNUM));
    ui->comboBox_comm_model->setCurrentIndex(configInfo.get_configData(C_COMM_MODEL).toInt());
    ui->lineEdit_BaudRate->setText(configInfo.get_configData(MQTT_BRUD_RATE));
    ui->lineEdit_Addr->setText(configInfo.get_configData(LORA_ADDR));
    ui->lineEdit_channel->setText(configInfo.get_configData(LORA_CHANNEL));
    ui->lineEdit_pubkey->setText(configInfo.get_configData(C_PUB_KEY));
    ui->lineEdit_prikey->setText(configInfo.get_configData(C_PRI_KEY));
    ui->lineEdit_gateway_id->setText(configInfo.get_configData(GATEWAY_ID));

    logger->log("成功读取配置文件并初始化配置信息");
    return 0;

}

int clientPool::save_configInfo()
{
    if(isRunning){
        logger->log("仿真系统正在运行，不可配置仿真参数，请停止仿真！",WARN);
        return -1;
    }
    configInfo.set_configData(SERVER_IP,ui->lineEdit_serverIP->text());
    configInfo.set_configData(SERVER_PORT,ui->lineEdit_serverPort->text());
    configInfo.set_configData(DEVNUM,ui->lineEdit_devNum->text());
    configInfo.set_configData(START_DEVID,ui->lineEdit_startDevId->text());
    configInfo.set_configData(C_COMM_MODEL,QString::number(ui->comboBox_comm_model->currentIndex()));
    configInfo.set_configData(MQTT_BRUD_RATE,ui->lineEdit_BaudRate->text());
    configInfo.set_configData(LORA_ADDR,ui->lineEdit_Addr->text());
    configInfo.set_configData(LORA_CHANNEL,ui->lineEdit_channel->text());
    configInfo.set_configData(C_PUB_KEY,ui->lineEdit_pubkey->text());
    configInfo.set_configData(C_PRI_KEY,ui->lineEdit_prikey->text());
    configInfo.set_configData(GATEWAY_ID,ui->lineEdit_gateway_id->text());

    configInfo.write_clientConfigFile();
    logger->log("成功保存配置信息在："+configInfo.get_configFileName());
    return 0;
}

int clientPool::init_devList()
{
    // 从配置中获取设备数
        int numDevices = configInfo.get_configData(DEVNUM).toInt();
        if(numDevices>MAX_DEV_NUM)numDevices=MAX_DEV_NUM;  //限制最大仿真数量
        // 从配置中获取设备名前缀
        QString prefix = configInfo.get_configData(START_DEVID);

        // 清空现有的列表
        ui->listWidget_dev->clear();

        // 生成设备名称并添加到列表中
        for (int i = 0; i < numDevices; ++i) {
            // 生成设备名称
            QString deviceName = QString("%1-%2")
                        .arg(prefix)
                        .arg(i+1, 5, 10, QLatin1Char('0'));  // 确保设备编码是五位数，不足补零
            new QListWidgetItem(deviceName, ui->listWidget_dev);
        }

        return 0;
}

int clientPool::init_devPool()
{

    int devNum = configInfo.get_configData(DEVNUM).toInt();
    if(devNum>MAX_DEV_NUM)devNum=MAX_DEV_NUM;  //限制最大仿真数量
    QString prefix = configInfo.get_configData(START_DEVID);
    for(int i=0;i<devNum;i++){
        QString deviceName = QString("%1-%2")
                    .arg(prefix)
                    .arg(i+1, 5, 10, QLatin1Char('0'));  // 确保设备编码是五位数，不足补零
        devPool.append(new ClientDev(deviceName,i,gateWayID,this));
        connect(devPool[i],&ClientDev::sig_log,this,&clientPool::handle_got_log);       //打印日志对接
        connect(devPool[i],&ClientDev::sig_connect_server_failed,this,&clientPool::handle_connect_server_failed);
        connect(devPool[i],&ClientDev::sig_connect_server_successd,this,&clientPool::handle_connect_server_success);
    }

    return 0;
}

int clientPool::init_tcpConnect()
{
    QString ip=configInfo.get_configData(SERVER_IP);
    QString port=configInfo.get_configData(SERVER_PORT);
    for(auto dev:devPool){
        dev->connect_tcpServer(ip,port);
    }
}

ClientDev *clientPool::getDev(int index)
{
    if(!isRunning){
        logger->log("仿真未启动,请点击开启仿真，初始化仿真系统！",WARN);
        return nullptr;
    }
    if(index>=devPool.size()){
        logger->log("系统错误，选择的设备小标大于等于设备池数量！！",ERR);
        return nullptr;
    }
    else if(index<0){
        logger->log("未在列表选中设备，请在设备列表点击要操控的设备！",WARN);
        return nullptr;
    }
    return devPool[index];
}

void clientPool::start_simul()
{
    logger->log("正在开启仿真.....");

    ui->pushButton_start->setText("停止仿真");
    logger->log("正在初始化仿真设备池.....");
    init_devPool();
    logger->log("仿真设备初始化成功");
    logger->log("正在初始化设备列表....");
    init_devList();
    logger->log("设备列表初始化成功");
    if(configInfo.get_configData(C_COMM_MODEL).toUInt()==0){
        logger->log("设备正在连接TCP服务器.....");
        init_tcpConnect();
    }
    else{//LORA 模式
        isRunning=true;
        logger->log("客户端仿真器初始化完成！正在运行.....");
    }
}

void clientPool::stop_simul()
{
    isRunning=false;
    for(auto dev:devPool) delete dev;
    devPool.clear();
    ui->pushButton_start->setText("开启仿真");
    logger->log("仿真停止");

}

void clientPool::handle_got_log(QString devID, QString msg)
{
    logger->log("devID:"+devID+": "+msg,DEV);
}

void clientPool::handle_start_dev()
{
    int devIndex=currentDevIndex;
    ClientDev *dev=nullptr;
    if((dev=getDev(devIndex))==nullptr)return;

    if(dev->getIsRunning()){    //正在运行
        ui->pushButton_devStart->setText("启动运行");
        dev->stopRunning();     //停止运行
    }
    else{
        ui->pushButton_devStart->setText("停止运行");
        dev->startRunning();     //开启运行
    }
}

void clientPool::handle_devListIndex_changed()
{
    if(!isRunning) return ;
    currentDevIndex=ui->listWidget_dev->currentRow();
    //切换界面中的设备参数及其相关状态
    ClientDev *dev=getDev(currentDevIndex);
    if(dev->getIsRunning()){    //正在运行
        ui->pushButton_devStart->setText("停止运行");
    }else{
        ui->pushButton_devStart->setText("启动运行");
    }
    DevParam para=dev->getDevParam();
    ui->lineEdit_sleepTime->setText(QString::number(para.sleepTime));
    ui->lineEdit_collectTime->setText(QString::number(para.collectTime));
    ui->lineEdit_waitTime->setText(QString::number(para.cuttingTime));
    ui->lineEdit_glueWeight->setText(QString::number(para.glueWeight));
}

void clientPool::handle_clickStartBtn()
{
    if(isRunning){
        stop_simul();
    }
    else{
        start_simul();
    }
}

void clientPool::handle_clickSetParam()
{
    ClientDev *dev=getDev(currentDevIndex);
    if(dev==nullptr){
        return;
    }
    DevParam para;
    bool ret=false;
    para.sleepTime=ui->lineEdit_sleepTime->text().toUInt(&ret);
    if(!ret){
        logger->log("睡眠时间的输入为正整数，单位为秒！",ERR);
        return;
    }
    para.collectTime=ui->lineEdit_collectTime->text().toUInt(&ret);
    if(!ret){
        logger->log("收集时间的输入为正整数，单位为秒！",ERR);
        return;
    }
    para.cuttingTime=ui->lineEdit_waitTime->text().toUInt(&ret);
    if(!ret){
        logger->log("割胶时间的输入为正整数，单位为秒！",ERR);
        return;
    }
    para.glueWeight=ui->lineEdit_glueWeight->text().toUInt(&ret);
    if(!ret){
        logger->log("乳胶重量的输入为正整数，单位为kg！",ERR);
        return;
    }
    dev->setDevParam(para);
}

void clientPool::handle_clickGetDevStatus()
{

    ClientDev *dev=getDev(currentDevIndex);
    if(dev==nullptr){
        return;
    }
    logger->log(dev->getStatus());
}

void clientPool::handle_connect_server_success(int index)
{
    static int connectedNum=0;
    connectedNum++;
    if(connectedNum==devPool.size()){
        connectedNum=0;
        logger->log("所有设备套接字连接tcp服务端成功！");
        isRunning=true;
    }
}

void clientPool::handle_connect_server_failed(int index)
{
    static int num;
    num++;  //这里需要修改，似乎会多次触发
    if(num==devPool.size())
    {
        num=0;
        //连接不上服务器，终止仿真
        stop_simul();
    }
}
