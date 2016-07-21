#ifndef __SYS_INFO_H__
#define __SYS_INFO_H__

#include "UpdateInfo.h"


//#define SYS_INFO_SECTOR 1 * 4096
#define SYS_INFO_SIZE   sizeof(sys_info_t)
#define SYS_INFO_USED   0x01

#define SYS_VEN_ID      "GAKATO"           /* 厂商ID */
#define SYS_DEV_MODEL   "GFM-WG-ZWS/8/W"   /* 设备型号 */
#define SYS_DEV_ID      "c1000000"         /* 设备编号 6位 */
#define SYS_DEV_ID_ADDR 0xF000

#pragma pack(1)

typedef enum {
    SYS_EVENT_NONE = 0x00,
    SYS_EVENT_OPEN_LOCK = 0x01,
    SYS_EVENT_DOOR_MAGENT = 0x02,
} sys_event_t;
extern sys_event_t sys_event;

/* 移动侦测 */
typedef struct {
    unsigned char flag;     /* '1':open;'0':close */
    unsigned int time;      /* */
} mc_t;

typedef struct {
    unsigned char used;         /* 0xFF : not used */
    unsigned char wifi[64];     /* 接入网络热点名称和密码, 用,分割 */
    mc_t mc;
    unsigned char id[16];       /* 设备编号8位 */
    unsigned char pwd[16];      /* 无线密码8位 */
    unsigned char bf;           /* 布防 */
    unsigned char reset_flag;   /* 重置标志 */
    unsigned char rf433_data;
} sys_info_t;

#pragma pack()

extern sys_info_t g_sys_info;
extern volatile unsigned char DebugFlag;

void SysInfoDefault(unsigned char reset_flag);
void SysInfoCheckDevID(void);
void SysInfoInit(void);
void SysInfoLock315SnrSave(unsigned int snr);
void SysInfoWifiSave(unsigned char *wifi);
void SysInfoWifiGet(unsigned char *wifi);
void SysInfoWifiClean(void);
unsigned char SysInfoWifiIsNull(unsigned char *wifif);
void SysInfoMcSave(mc_t mc);
void SysInfoMcGet(mc_t *mc);
void SysInfoIDSave(unsigned char *id);
void SysInfoIDGet(unsigned char *id);
void SysInfoPwdSave(unsigned char *pwd);
void SysInfoPwdGet(unsigned int *pwd);
void SysInfoBfSave(unsigned char bf);
void SysInfoBfGet(unsigned char *bf);
void SysInfoReset(unsigned char ResetFlag);
void SystemResetHandler(void);
void SystemReset(void);
unsigned char SysInfoGetResetFlag(void);
unsigned char SysInfoClrResetFlag(void);
unsigned int  SysInfoGetRf433Data(void);
void SysInfoSetRf433Data(unsigned int data);

#endif


