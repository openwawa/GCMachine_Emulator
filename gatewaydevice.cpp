#include "gatewaydevice.h"
#include "ui_gatewaydevice.h"

GateWayDevice::GateWayDevice(ConfigInfo &config,QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::GateWayDevice),configInfo(config)
{
    ui->setupUi(this);
    logger=new Logger(ui->textEdit_log,true,true,this);
    logger->log("初始化界面");
    init_configInfo();
    ui->comboBox_model->setCurrentIndex(configInfo.get_configData(S_COMM_MODEL).toInt());      //读取配置文件，设置通信模式

    connect(ui->pushButton_save,&QPushButton::clicked,this,&GateWayDevice::save_configInfo);
}

GateWayDevice::~GateWayDevice()
{
    delete ui;
}

int GateWayDevice::init_configInfo()
{
    configInfo.read_gateWayConfigFile();  //获取配置文件中保存的信息
    //将读取的配置信息初始化界面中的配置

    ui->lineEdit_s_ip->setText(configInfo.get_configData(S_IP));
    ui->lineEdit_s_port->setText(configInfo.get_configData(S_PORT));
    ui->comboBox_model->setCurrentIndex(configInfo.get_configData(S_COMM_MODEL).toInt());
    ui->lineEdit_MQTT_url->setText(configInfo.get_configData(MQTT_URL));
    ui->lineEdit_username->setText(configInfo.get_configData(MQTT_USER_NAME));
    ui->lineEdit_passwd->setText(configInfo.get_configData(MQTT_PASSWD));
    logger->log("成功读取配置文件并初始化配置信息");
    return 0;
}

int GateWayDevice::save_configInfo()
{
    configInfo.set_configData(S_IP,ui->lineEdit_s_ip->text());
    configInfo.set_configData(S_PORT,ui->lineEdit_s_port->text());
    configInfo.set_configData(MQTT_URL,ui->lineEdit_MQTT_url->text());
    configInfo.set_configData(MQTT_USER_NAME,ui->lineEdit_username->text());
    configInfo.set_configData(S_COMM_MODEL,QString::number(ui->comboBox_model->currentIndex()));
    configInfo.set_configData(MQTT_PASSWD,ui->lineEdit_passwd->text());

    configInfo.write_gateWayConfigFile();
    logger->log("成功保存配置信息"+configInfo.get_configFileName());
    return 0;
}
