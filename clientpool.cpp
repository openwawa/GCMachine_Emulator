#include "clientpool.h"
#include "ui_clientpool.h"
#include <string>
#include <iostream>
clientPool::clientPool(ConfigInfo &config,QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::clientPool),configInfo(config)
{
    ui->setupUi(this);
    logger=new Logger(ui->textEdit_log,true,true,this);
    logger->log("初始化界面");
    init_configInfo();
    init_devList();
    ui->comboBox_comm_model->setCurrentIndex(configInfo.get_configData(C_COMM_MODEL).toInt());      //读取配置文件，设置通信模式

    connect(ui->pushButton_saveConfig,&QPushButton::clicked,this,&clientPool::save_configInfo);
}

clientPool::~clientPool()
{
    delete ui;
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
    logger->log("成功读取配置文件并初始化配置信息");
    return 0;

}

int clientPool::save_configInfo()
{
    configInfo.set_configData(SERVER_IP,ui->lineEdit_serverIP->text());
    configInfo.set_configData(SERVER_PORT,ui->lineEdit_serverPort->text());
    configInfo.set_configData(DEVNUM,ui->lineEdit_devNum->text());
    configInfo.set_configData(START_DEVID,ui->lineEdit_startDevId->text());
    configInfo.set_configData(C_COMM_MODEL,QString::number(ui->comboBox_comm_model->currentIndex()));
    configInfo.set_configData(MQTT_BRUD_RATE,ui->lineEdit_BaudRate->text());
    configInfo.set_configData(LORA_ADDR,ui->lineEdit_Addr->text());
    configInfo.set_configData(LORA_CHANNEL,ui->lineEdit_channel->text());

    configInfo.write_clientConfigFile();
    logger->log("成功保存配置信息"+configInfo.get_configFileName());
    return 0;
}

int clientPool::init_devList()
{
    // 从配置中获取设备数
        int numDevices = configInfo.get_configData(DEVNUM).toInt();

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
