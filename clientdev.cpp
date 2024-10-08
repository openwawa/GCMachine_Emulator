#include "clientdev.h"

ClientDev::ClientDev(QString name,int index,QString &gatewayID,QObject *parent):
    ComDevice(name,index,0,gatewayID,parent){

    devParam.sleepTime=60;
    devParam.collectTime=10;
    devParam.cuttingTime=10;

    isRunning=false;       //默认机器处于关闭状态
    isWorking=false;       //一开始都处于睡眠状态
    isSleeping=false;
    isWaitting=false;


    sleepTimer=new QTimer(this);
    cutTimer=new QTimer(this);

    connect(sleepTimer, &QTimer::timeout, this, &ClientDev::onSleepTimerTimeout);
    connect(cutTimer, &QTimer::timeout, this, &ClientDev::onCutTimerTimeout);

}

ClientDev::~ClientDev()
{
    delete sleepTimer;
    delete cutTimer;
}

void ClientDev::setDevParam(DevParam &param)
{
    devParam=param;
    emit(sig_log(this->devID,"修改设备运行参数成功！"));
}

QString ClientDev::getStatus()
{
    QString prefix;
    prefix.append("devID:"+devID+"设备状态: ");
    if(isRunning==false){
        prefix.append("关机状态");
    }else{
        if (isWorking) {
                prefix.append("正在割胶");
            } else if (isWaitting) {
                prefix.append("等待割胶指令返回");
            } else {
                prefix.append("处于睡眠状态");
            }
    }
    return prefix;
}

void ClientDev::startRunning()
{
    isRunning=true;
    startSleepTimer();
    emit(sig_log(this->devID,"开启运行，并进入睡眠模式"));
}

void ClientDev::stopRunning()
{
    isRunning=false;       //默认机器处于关闭状态
    isWorking=false;       //一开始都处于睡眠状态
    isSleeping=false;
    isWaitting=false;
    sleepTimer->stop();
    cutTimer->stop();

    emit(sig_log(this->devID,"已停止运行"));
}

void ClientDev::startSleepTimer()
{
    isSleeping=true;
    sleepTimer->start(devParam.sleepTime*1000);
}

void ClientDev::sendGetScheduledTimeRequest()
{
    QString msg = "GET_SCHEDULED_TIME_REQUEST";  // 消息体
    QString package = build_LoraMesg(msg, gateWayID, MsgType::GetScheduledTimeRequest);
    emit sendMessageToGateway(package);
}

void ClientDev::onSleepTimerTimeout()
{
    sendGetScheduledTimeRequest();
    sleepTimer->stop();
    isSleeping=false;
    isWaitting = true;
    emit(sig_log(devID,"睡眠唤醒，设备唤醒并发送获取预约割胶时间请求"));
}

void ClientDev::onCutTimerTimeout()
{
    isWorking = false;
    startSleepTimer();  // 完成割胶后重新进入睡眠
    emit(sig_log(devID,"割胶任务完成，重新进入睡眠状态"));
}
