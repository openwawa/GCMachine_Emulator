#ifndef EMULATOR_H
#define EMULATOR_H

#include <QMainWindow>
#include <QSplitter>
#include "clientpool.h"
#include "gatewaydevice.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Emulator; }
QT_END_NAMESPACE

class Emulator : public QMainWindow
{
    Q_OBJECT

public:
    Emulator(QWidget *parent = nullptr);
    ~Emulator();

private:
    Ui::Emulator *ui;
    clientPool *clientWindow;
    GateWayDevice *gwDevWindow;
    QSplitter *splitter;
};
#endif // EMULATOR_H
