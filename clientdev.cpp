#include "clientdev.h"

ClientDev::ClientDev(QString name,int index,QString &gatewayID,QObject *parent):
    ComDevice(name,index,0,gatewayID,parent){

    devParam.sleepTime=10;
    devParam.collectTime=10;
    devParam.cuttingTime=8;
    devParam.glueWeight=5;

    isRunning=false;       //默认机器处于关闭状态
    isWorking=false;       //一开始都处于睡眠状态
    isSleeping=false;
    waitStatus=DEV_WAIT_NOTHING;


    sleepTimer=new QTimer(this);
    cutTimer=new QTimer(this);
    collectTimer=new QTimer(this);
    waitScheduleTimer=new QTimer(this);

    connect(sleepTimer, &QTimer::timeout, this, &ClientDev::onSleepTimerTimeout);
    connect(cutTimer, &QTimer::timeout, this, &ClientDev::onCutTimerTimeout);
    connect(collectTimer, &QTimer::timeout, this, &ClientDev::onCollectTimerTimeout);
    connect(waitScheduleTimer, &QTimer::timeout, this, &ClientDev::onWaittingTimerTimeout);
    connect(this, &ClientDev::sig_getNewTask, this, &ClientDev::handleGetNewTask);


}

ClientDev::~ClientDev()
{
    delete sleepTimer;
    delete cutTimer;
    delete collectTimer;
    delete waitScheduleTimer;
    delete socket;
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
        }
        else {
            switch((char)waitStatus){
                case DEV_WAIT_SCHEDULE:
                    prefix.append("等待割胶指令返回");
                    break;
                case DEV_WAIT_CUTTING_START:
                    prefix.append("等待割胶时间到达");
                    break;
                case DEV_WAIT_MASKSURE:
                    prefix.append("等待割胶时间确认返回");
                    break;
                case DEV_WAIT_CUTTING:
                    prefix.append("等待割胶任务");
                    break;
                case DEV_WAIT_NOTHING:
                    prefix.append("处于睡眠状态");
                    break;
            }
        }
    }
    return prefix;
}

void ClientDev::connect_tcpServer(QString serverIP,QString port)
{
    socket = new QTcpSocket();
    connect(socket, &QTcpSocket::connected, this,[=]() {        //发送日志信息
        emit(sig_connect_server_successd(devIndex));
        emit(sig_log(this->devID,"成功连上服务器！"));

    });
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), this, [=](QAbstractSocket::SocketError error) {
        emit(sig_connect_server_failed(devIndex));
        emit(sig_log(this->devID,"连接服务器失败，原因："+socket->errorString()));
    });
    connect(socket, &QTcpSocket::readyRead, this, &ClientDev::handleIncomingMessage);
    socket->connectToHost(serverIP, port.toInt()); // 服务器地址和端口号

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
    waitStatus=DEV_WAIT_NOTHING;
    sleepTimer->stop();
    cutTimer->stop();
    waitScheduleTimer->stop();

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
    if(socket->isOpen()&&socket->isValid()){
        QByteArray dataBuf=package.toUtf8();
        socket->write(dataBuf.data(),dataBuf.length());
        emit(sig_log(devID,"预约割胶时间请求发送成功"));
    }
}

void ClientDev::startCuttingTask()
{
    waitStatus=DEV_WAIT_CUTTING;
    isWorking=true;
    cutTimer->start(devParam.cuttingTime*1000);
    waitScheduleTimer->stop();
    task.reset();       //重置任务
    emit(sig_log(devID,"开始割胶"));
}

void ClientDev::onSleepTimerTimeout()
{
    sendGetScheduledTimeRequest();
    sleepTimer->stop();
    isSleeping=false;
    waitStatus = DEV_WAIT_SCHEDULE;
    emit(sig_log(devID,"睡眠唤醒，设备唤醒并发送获取预约割胶时间请求"));
}

void ClientDev::onWaittingTimerTimeout()
{
    waitStatus = DEV_WAIT_MASKSURE;
    waitScheduleTimer->stop();
    QString msg = "GET_SCHEDULED_TIME_REQUEST";  // 消息体
    QString package = build_LoraMesg(msg, gateWayID, MsgType::GetScheduledTimeRequest);
    if(socket->isOpen()&&socket->isValid()){
        QByteArray dataBuf=package.toUtf8();
        socket->write(dataBuf.data(),dataBuf.length());
        emit(sig_log(devID,"请求确认割胶时间"));
    }
}

void ClientDev::onCollectTimerTimeout()
{
    QString msg = "I get some glue !";  // 消息体
    collectTimer->stop();
    QString package = build_LoraMesg(msg, gateWayID, MsgType::LatexWeightReport);
    if(socket->isOpen()&&socket->isValid()){
        QByteArray dataBuf=package.toUtf8();
        socket->write(dataBuf.data(),dataBuf.length());
        emit(sig_log(devID,"收集像胶任务完成，发送收集信息"));
    }
}

void ClientDev::onCutTimerTimeout()
{
    isWorking = false;
    waitStatus=DEV_WAIT_NOTHING;
    cutTimer->stop();
    collectTimer->start(devParam.collectTime*1000);
    startSleepTimer();  // 完成割胶后重新进入睡眠
    emit(sig_log(devID,"割胶任务完成，收集乳胶并重新进入睡眠状态"));
}

void ClientDev::handleIncomingMessageByTcp()
{

}

void ClientDev::handleIncomingMessageByLora()
{

}

void ClientDev::handleIncomingMessage()
{
    //解析接受的字符串
    //01 41 30 2D 42 30 2D 43 30 2D 30 30 30 41 30 2D 42 30 2D 43 30 2D 30 30 30 30 31 30 31 32 33 34 35 36 37 38 39 30 14 A8 32 30 32 34 2D 31 31 2D 32 30 20 31 30 3A 35 39 3A 30 30
     QByteArray buff;
    while (socket->bytesAvailable() > 0) {
        auto data=socket->readAll();        //这里应该用缓冲区优化，待修改
        buff.append(data);
    }
     qDebug() <<"Received data from socket:" << buff;
    if(isRunning==false||waitStatus==DEV_WAIT_NOTHING)return;         //未开机和休眠情况下，不接受，也不处理数据
    ParsedPacket packet;//每次接受数据都处理
    packet=parsePacket(buff);
    if(packet.nodeId==devID){
        if(packet.messageBody[0]==(char)MsgType::ScheduleCuttingTimeCmd){
            QString msg = QString::fromUtf8(packet.messageBody);
            emit(sig_getNewTask(msg));
        }
    }

}

void ClientDev::handleGetNewTask(QString& mesg)
{
    //这里会处理接收到割胶指令的处理，包括第一次接受以及第二次的确认接受
    mesg.remove(0,1);
    if(task.getStatus()==1){
        CuttingTask task;
        task.reLoad(mesg);
        int s=task.waitTime();
#if(DEBUG_MODE==1)          //调试模式下，默认收到就是确认
    s=0;
#endif
        if(s<=0){
            startCuttingTask();
            return ;
        }
    }
    //若第二次收到的任务时间超出当前时间，那么视为当前得到的任务为新任务
    task.reLoad(mesg);
    int sec=task.waitTime();
    if(sec<=0){  //已预期，直接开始/这里可能对应第二次确认，当同样的任务第二次发送，自然是已经逾期了，所以直接运行割胶动作
       startCuttingTask();
       return ;
    }
#if(DEBUG_MODE==1)
    sec=10;     //用于调试，都等待10S
#endif
    //设置等待定时
    waitStatus=DEV_WAIT_CUTTING_START;
    waitScheduleTimer->stop();
    waitScheduleTimer->start(sec*1000);
}
