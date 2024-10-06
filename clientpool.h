#ifndef CLIENTPOOL_H
#define CLIENTPOOL_H

#include <QDockWidget>

namespace Ui {
class clientPool;
}

class clientPool : public QDockWidget
{
    Q_OBJECT

public:
    explicit clientPool(QWidget *parent = nullptr);
    ~clientPool();

private:
    Ui::clientPool *ui;
};

#endif // CLIENTPOOL_H
