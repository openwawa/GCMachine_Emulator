#ifndef CONFIGINFO_H
#define CONFIGINFO_H
#include <QString>
#include <QSettings>
#include "global.h"
class ConfigInfo{

public:
  ConfigInfo(QString name){
      settings=new QSettings(name, QSettings::IniFormat);
  }
  QString get_configFileName(void){return settings->fileName();}
  int write_clientConfigFile(void){
      settings->setValue("serverIP", configData[SERVER_IP]);
      settings->setValue("serverPort", configData[SERVER_PORT]);
      settings->setValue("startDevID", configData[START_DEVID]);
      settings->setValue("devNum", configData[DEVNUM]);
      settings->setValue("c_comm_model", configData[C_COMM_MODEL]);
      settings->setValue("mqtt_brud_rate", configData[MQTT_BRUD_RATE]);
      settings->setValue("lora_addr", configData[LORA_ADDR]);
      settings->setValue("lora_channel", configData[LORA_CHANNEL]);
      settings->setValue("gateway_id", configData[GATEWAY_ID]);
      settings->setValue("c_pub_key", configData[C_PUB_KEY]);
      settings->setValue("c_pri_key", configData[C_PRI_KEY]);

      return 0;
  }
  int write_gateWayConfigFile(void){
      settings->setValue("sIP", configData[S_IP]);
      settings->setValue("sPort", configData[S_PORT]);
      settings->setValue("MQTT_URL", configData[MQTT_URL]);
      settings->setValue("MQTT_userName", configData[MQTT_USER_NAME]);
      settings->setValue("MQTT_passwd", configData[MQTT_PASSWD]);
      settings->setValue("s_comm_model", configData[S_COMM_MODEL]);
      return 0;
  }

  int read_clientConfigFile(void){
      configData<<settings->value("serverIP").toString()
                <<settings->value("serverPort").toString()
                <<settings->value("startDevID").toString()
                <<settings->value("devNum").toString()
                <<settings->value("c_comm_model").toString()
                <<settings->value("mqtt_brud_rate").toString()
                <<settings->value("lora_addr").toString()
                <<settings->value("lora_channel").toString()
                <<settings->value("gateway_id").toString()
                <<settings->value("c_pub_key").toString()
                <<settings->value("c_pri_key").toString();       //这里的顺序要和write一致

      return 0;
  }
  int read_gateWayConfigFile(void){
      configData<<settings->value("sIP").toString()
                <<settings->value("sPort").toString()
                <<settings->value("MQTT_URL").toString()
                <<settings->value("MQTT_userName").toString()
                <<settings->value("MQTT_passwd").toString()
                <<settings->value("s_comm_model").toString()
                ;
      return 0;
  }
  const QString get_configData(CONFIG_TYPE type){
      QString error{"error"};
      if(type>=SERVER_IP&&type<=S_COMM_MODEL)
          return configData.at(type);
      return error;
  }
  int set_configData(CONFIG_TYPE type,QString val){
      if(type>=SERVER_IP&&type<=S_COMM_MODEL){
          configData[type]=val;
          return 0;
      }
      return -1;
  }
   QVector<QString>& get_allConfigData(){return configData;}
  ~ConfigInfo(){
      delete settings;
  }
private:
  QVector<QString> configData;
  QSettings *settings;
};


#endif // CONFIGINFO_H
