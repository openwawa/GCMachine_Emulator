#include "gatewaydevice.h"
#include "ui_gatewaydevice.h"

GateWayDevice::GateWayDevice(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::GateWayDevice)
{
    ui->setupUi(this);
}

GateWayDevice::~GateWayDevice()
{
    delete ui;
}
