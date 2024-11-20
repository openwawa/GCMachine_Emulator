#ifndef COMDEVICE_H
#define COMDEVICE_H

#include <QObject>
#include <QDataStream>
#include "global.h"
typedef struct { //产品公私钥对
    unsigned char pubkey[11];
    unsigned char prikey[11];
}KeyPair;
struct ParsedPacket
{
    quint8 messageId;
    QByteArray gatewayId;
    QByteArray nodeId;
    QByteArray nodeProductKey;
    quint8 messageBodyLength;
    QByteArray messageBody;
};

class ComDevice: public QObject
{
    Q_OBJECT
protected:
    QString devID;
    QString gateWayID;     // 网关 ID
    KeyPair keyPair;
    int devIndex;
    int devType;   //0->client,1->gateway
private:

    unsigned char msgID;
public:
    ComDevice(QString name,int index,int type,QString &gwID,QObject *parent = nullptr):
        QObject(parent),devID(name),gateWayID(gwID),devType(type),devIndex(index){
        msgID=0;
    }
    QString build_LoraMesg(QString& msg,QString &gateWayID,MsgType type){
        QString package;
        //依次为消息 ID、网关 ID、节点 ID、消息体长度和消息体
        if(devType){//下行通讯协议的组成比上行协议多了一个消息 ID 字段,一个字节
            package.append(msgID);
            msgID=msgID>=255?0:msgID+1; //更新消息ID值
        }
        package.append(gateWayID);
        package.append(devID);
        package.append(QByteArray(reinterpret_cast<const char*>(keyPair.pubkey), 11));
        // 消息体长度 (1 bytes)
        unsigned  char dataLength=0;
        if(msg.size()>254){     //规定消息体小于255字节,若大于则截断
             dataLength=255;
        }else{
            dataLength= msg.size()+1;   //还有一个字节的消息类型
        }
        package.append((unsigned char)type);        //消息体包含一个字节的消息类型和消息内容
        package.append(msg);
        return package;
    }
    ParsedPacket parsePacket(const QByteArray &packet)
    {
        ParsedPacket parsedPacket;
        QDataStream stream(packet);

        if(packet.size()<1+12+14+11+1){
            return parsedPacket;
        }
        //
        // 读取消息 ID (1 字节)
        stream >> parsedPacket.messageId;

        // 读取网关 ID (12 字节)
        parsedPacket.gatewayId = packet.mid(stream.device()->pos(), 12);
        stream.skipRawData(12);

        // 读取节点 ID (14 字节)
        parsedPacket.nodeId = packet.mid(stream.device()->pos(), 14);
        stream.skipRawData(14);

        // 读取节点产品密钥 (11 字节)
        parsedPacket.nodeProductKey = packet.mid(stream.device()->pos(), 11);
        stream.skipRawData(11);

        // 读取消息体长度 (1 字节)
        stream >> parsedPacket.messageBodyLength;

        // 读取消息体 (长度由 messageBodyLength 指定)
        parsedPacket.messageBody = packet.mid(stream.device()->pos(), parsedPacket.messageBodyLength);
        stream.skipRawData(parsedPacket.messageBodyLength);

        return parsedPacket;
    }

};

#endif // COMDEVICE_H
