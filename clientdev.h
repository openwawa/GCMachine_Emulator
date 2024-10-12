#ifndef CLIENTDEV_H
#define CLIENTDEV_H
#include <QObject>
#include "comdevice.h"
#include <QTcpSocket>
#include <QTimer>
#include <QDateTime>
typedef struct{
    int sleepTime;         //睡眠时间
    int collectTime;       //割胶收集时间
    int cuttingTime;       //割胶时间
}DevParam;
class CuttingTask {
    QDateTime startTime;
    bool isValid;
    int status;
public:
    CuttingTask(){
        isValid=false;
        status=0;
    }
    bool init(const QString& startTimeStr){
        startTime = QDateTime::fromString(startTimeStr, "yyyy-MM-dd HH:mm:ss");
        if (!startTime.isValid()) {
            qWarning() << "Invalid start time format. Expected: yyyy-MM-dd HH:mm:ss";
            return false;
        }
        status=1;
        isValid=true;
        return true;
    }
    bool reLoad(const QString& startTimeStr){
        return init(startTimeStr);
    }
    int getStatus(){return status;}
    void reset(){isValid=false;status=0;}
    bool shouldBeStart(QDateTime now)const{
        // 比较当前时间和预定时间
        return now >= startTime;
    }
    int waitTime(void)const{
        if (!isValid) {
               qWarning() << "Cutting task is not valid. Cannot calculate wait time.";
               return -1;  // 返回 -1 表示无效的等待时间
           }
       // 计算当前时间和预定开始时间之间的秒数差
       int secondsToWait = startTime.secsTo(QDateTime::currentDateTime());

       // 如果当前时间已经过了预定时间，则返回 0
       if (secondsToWait > 0) {
           return 0;
       }

       return -secondsToWait;
    }
};
//无等待时间-》等待预约时间表-》等待割胶开始-》等待确认指令返回-》等待割胶任务
enum DevWaitStatus{DEV_WAIT_NOTHING,DEV_WAIT_SCHEDULE,DEV_WAIT_CUTTING_START,DEV_WAIT_MASKSURE,DEV_WAIT_CUTTING};
class ClientDev:public ComDevice
{
     Q_OBJECT
private:
    DevParam devParam;  //设备运行参数

     bool isRunning;                //是否开机运行
     bool isSleeping;               //是否睡眠
     bool isWorking;                //是否进行割胶工作

     DevWaitStatus waitStatus;
     //bool isWaittingSchedule;       //等待工作表
     //bool isWaittingMakeSure;       //等待确认工作指令恢复


     QTcpSocket* socket;

     QTimer* sleepTimer;    // 睡眠定时器
     QTimer* cutTimer;      // 模拟割胶任务定时器
     QTimer *collectTimer;   // 模拟收集割胶任务
     QTimer *waitScheduleTimer;

     CuttingTask task;
public:
     ClientDev(QString name,int index,QString &gatewayID,QObject *parent);
     ~ClientDev();

     void setDevParam(DevParam &param);
     DevParam getDevParam(void){return devParam;}
     bool getIsRunning(void){return isRunning;}
     QString getStatus();       //获取机器当前状态
     void connect_tcpServer(QString serverIP,QString port);
     void disconnect_tcpServer();
     void startRunning();
     void stopRunning();

private:

     void startSleepTimer();
     void sendGetScheduledTimeRequest();
     void sendMakeSureTask();           //发送指令确保任务是最新的
     void startCuttingTask();
     void handleGetScheduleFristTime(QString& mesg); //处理第一次获取指令


private slots:
    void onSleepTimerTimeout();
    void onWaittingTimerTimeout();
    void onCollectTimerTimeout();
    void onCutTimerTimeout();
    void handleIncomingMessageByTcp();
    void handleIncomingMessageByLora();
    void handleGetNewScheduleMsg(QString& mesg);

signals:
    void sig_log(QString &devID,QString mesg);
    void sig_getNewTask(QString& mesg);
    void sig_connect_server_failed(int index);
    void sig_connect_server_successd(int index);
    void sendMessageToGateway(const QString& message);  // 发送消息到网关
};

#endif // CLIENTDEV_H
