#ifndef GATEWAYDEVICE_H
#define GATEWAYDEVICE_H

#include <QDockWidget>

namespace Ui {
class GateWayDevice;
}

class GateWayDevice : public QDockWidget
{
    Q_OBJECT

public:
    explicit GateWayDevice(QWidget *parent = nullptr);
    ~GateWayDevice();

private:
    Ui::GateWayDevice *ui;
};

#endif // GATEWAYDEVICE_H
