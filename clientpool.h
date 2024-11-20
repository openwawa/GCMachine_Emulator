#ifndef CLIENTPOOL_H
#define CLIENTPOOL_H

#include <QDockWidget>
#include <QVector>
#include "configInfo.h"
#include "logger.h"
#include "clientdev.h"

namespace Ui {
class clientPool;
}

class clientPool : public QDockWidget
{
    Q_OBJECT
signals:
    void sig_log(QString& msg,LogType type);
public:
    explicit clientPool(ConfigInfo &config,QWidget *parent = nullptr);
    ~clientPool();

private:
    Ui::clientPool *ui;
    ConfigInfo &configInfo;
    Logger *logger;
    QVector<ClientDev*> devPool;        //设备池

    QString gateWayID;
    bool isRunning;
    int currentDevIndex;    //当前设备列表选择的标号
private:
    int init_configInfo(void);
    int save_configInfo(void);
    int init_devList(void);
    int init_devPool();
    int init_tcpConnect(void);
    ClientDev* getDev(int index);
public:
    void start_simul(void);
    void stop_simul(void);

    void startOrStop_simul(void);
private slots:
   void handle_got_log(QString devID,QString msg);
   void handle_start_dev();
   void handle_devListIndex_changed(void);
   void handle_clickStartBtn(void);
   void handle_clickSetParam(void);
   void handle_clickGetDevStatus(void);

   void handle_connect_server_success(int index);
   void handle_connect_server_failed(int index);

};

#endif // CLIENTPOOL_H
