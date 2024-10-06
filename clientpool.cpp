#include "clientpool.h"
#include "ui_clientpool.h"

clientPool::clientPool(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::clientPool)
{
    ui->setupUi(this);
}

clientPool::~clientPool()
{
    delete ui;
}
