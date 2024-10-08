#ifndef CLIENTDEV_H
#define CLIENTDEV_H
#include <QObject>
#include "comdevice.h"
#include <QTcpSocket>
#include <QTimer>

typedef struct{
    int sleepTime;         //睡眠时间
    int collectTime;       //割胶收集时间
    int cuttingTime;       //割胶时间
}DevParam;

class ClientDev:public ComDevice
{
     Q_OBJECT
private:
    DevParam devParam;  //设备运行参数

     bool isRunning;        //是否开机运行
     bool isSleeping;       //是否睡眠
     bool isWaitting;       //等待指令
     bool isWorking;        //是否进行割胶工作

     QTcpSocket* socket;

     QTimer* sleepTimer;    // 睡眠定时器
     QTimer* cutTimer;      // 模拟割胶任务定时器
public:
     ClientDev(QString name,int index,QString &gatewayID,QObject *parent);
     ~ClientDev();

     void setDevParam(DevParam &param);
     DevParam getDevParam(void){return devParam;}
     bool getIsRunning(void){return isRunning;}
     QString getStatus();       //获取机器当前状态

     void startRunning();
     void stopRunning();

private:
     void startSleepTimer();
     void sendGetScheduledTimeRequest();
     void startCuttingTask();
     void handleIncomingMessage(const QString& message);

private slots:
    void onSleepTimerTimeout();
    void onCutTimerTimeout();

signals:
    void sig_log(QString &devID,QString mesg);
    void sendMessageToGateway(const QString& message);  // 发送消息到网关
};

#endif // CLIENTDEV_H
