#ifndef COMMHANDEL_H
#define COMMHANDEL_H
#include <QObject>
#include <QDataStream>
#include <QByteArray>
#include "global.h"
//通信句柄父类
class commHandel : public QObject
{
    Q_OBJECT
public:
    explicit commHandel(QObject *parent = nullptr);

    virtual void sendData(const QByteArray &data) = 0;

    virtual bool connectToHost() = 0;
    virtual void disconnectFromHost() = 0;

    // 虚函数，由子类实现获取连接状态
    virtual bool isConnected() const = 0;

protected:
    QByteArray buffer; // 用于存储接收到的数据
    int parseTpye;

signals:
    void newDataReceived(const QByteArray &data); // 当接收到新数据时发出信号

public slots:
    // 子类可以重载此方法来处理接收到的数据
    virtual void processData(const QByteArray &data) = 0;
};

#endif // COMMHANDLE_H
