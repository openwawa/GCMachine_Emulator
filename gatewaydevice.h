#ifndef GATEWAYDEVICE_H
#define GATEWAYDEVICE_H

#include <QDockWidget>
#include "configInfo.h"
#include "logger.h"

namespace Ui {
class GateWayDevice;
}

class GateWayDevice : public QDockWidget
{
    Q_OBJECT

public:
    explicit GateWayDevice(ConfigInfo &config,QWidget *parent = nullptr);
    ~GateWayDevice();
    int init_configInfo(void);
    int save_configInfo(void);

private:
    Ui::GateWayDevice *ui;
    ConfigInfo &configInfo;
    Logger *logger;
};

#endif // GATEWAYDEVICE_H
