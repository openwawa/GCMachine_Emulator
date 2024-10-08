#ifndef LOGGER_H
#define LOGGER_H
#include <QObject>
#include <QTextEdit>
#include <QTime>
#include "global.h"

class Logger:public QObject{
    Q_OBJECT
public:
    Logger(QTextEdit *logWidget, bool add_timeStamp=true,bool add_logType=true,QObject *parent = nullptr):
    QObject(parent),m_logWidget(logWidget),timeStampFlag(add_timeStamp),logTypeFlag(add_logType){
        // 确保 QTextEdit 是只读的
        m_logWidget->setReadOnly(true);
    }
    void log(const QString &message,LogType type=SYS){
        // 获取当前时间
        QString prefix;

        if(timeStampFlag){
            prefix.append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        }
        if(logTypeFlag){
            switch(type){
                case SYS:
                    prefix.append(" [SYS]->");
                    m_logWidget->setTextColor({175,0,200});     //紫色
                break;
                case DEV:
                    prefix.append(" [DEV]->");
                    m_logWidget->setTextColor({0,155,155});   //青色
                break;
                case ERR:
                    prefix.append(" [ERR]->");
                    m_logWidget->setTextColor({255,0,0}); //红色
                break;
                case WARN:
                    prefix.append(" [WARN]->");
                    m_logWidget->setTextColor({255,155,10}); //黄色
                break;
            }
        }
        m_logWidget->append(prefix);
        // 追加日志消息
        m_logWidget->setTextColor({0,0,0});     //黑色
        m_logWidget->append( message);

        // 滚动到最新的一条消息
        m_logWidget->moveCursor(QTextCursor::End);//将滑动条滚动到最底端;

    }
private:
    QTextEdit *m_logWidget;
    bool timeStampFlag;
    bool logTypeFlag;
};

#endif // LOGGER_H
