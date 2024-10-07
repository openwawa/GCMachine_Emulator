#ifndef CLIENTPOOL_H
#define CLIENTPOOL_H

#include <QDockWidget>
#include "configInfo.h"
#include "logger.h"
namespace Ui {
class clientPool;
}

class clientPool : public QDockWidget
{
    Q_OBJECT

public:
    explicit clientPool(ConfigInfo &config,QWidget *parent = nullptr);
    ~clientPool();
    int init_configInfo(void);
    int save_configInfo(void);
    int init_devList(void);

private:
    Ui::clientPool *ui;
    ConfigInfo &configInfo;
    Logger *logger;

};

#endif // CLIENTPOOL_H
