#ifndef GLOBAL_H
#define GLOBAL_H
#include <cstdint>  // 用于 std::uint8_t
#define MAX_DEV_NUM 100
#define DEBUG_MODE 0
enum LogType{SYS,DEV,ERR,WARN};
enum DevStatus{ENABLED,DISABLED};
enum CONFIG_TYPE{SERVER_IP,SERVER_PORT,START_DEVID,DEVNUM,C_COMM_MODEL,MQTT_BRUD_RATE,LORA_ADDR,LORA_CHANNEL,GATEWAY_ID,C_PUB_KEY,C_PRI_KEY,
                 S_IP,S_PORT,MQTT_URL,MQTT_USER_NAME,MQTT_PASSWD,S_COMM_MODEL};
enum class MsgType : std::uint8_t {
    Heartbeat = 0xA5,                // 心跳包
    JoinNetworkRequest = 0xAA,       // 入网请求
    GetScheduledTimeRequest = 0xAB,  // 获取预约时间请求
    CutDurationReport = 0xAC,        // 割胶时长上报
    AlarmReport = 0xAD,              // 警报上报
    LocationReport = 0xAE,           // 经纬度信息上报
    LatexWeightReport = 0xA9,        // 乳胶重量上报
    ScheduleCuttingTimeCmd = 0xA8,   // 预约割胶时间指令下发
    LatexWeightReportAck = 0xA7      // 乳胶重量上报回复
};

enum BUFFER_TYPE{UA_TYPE,WA_TA_TYPE,SIGN_TYPE,VERIFY_RESULT,ENC_MSG};





#endif // GLOBAL_H
