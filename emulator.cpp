#include "emulator.h"
#include "ui_emulator.h"

Emulator::Emulator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Emulator)
{
    ui->setupUi(this);
    //水平布局
    splitter = new QSplitter(Qt::Horizontal, this);

    // 创建并添加 ClientPool 窗口
    clientWindow = new clientPool(splitter);
    splitter->addWidget(clientWindow);

    // 创建并添加 GateWayDevice 窗口
    gwDevWindow = new GateWayDevice(splitter);
    splitter->addWidget(gwDevWindow);

    // 设置主窗口的中心部件
    setCentralWidget(splitter);
    //clientWindow=new ClientPool(this);
    //gwDevWindow=new GateWayDevice(this);

}

Emulator::~Emulator()
{
    delete ui;
}

