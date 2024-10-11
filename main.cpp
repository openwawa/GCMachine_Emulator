#include "emulator.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Emulator w;
    w.show();
    return a.exec();
}
