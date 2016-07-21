#ifndef __SYS_INFO_H__
#define __SYS_INFO_H__

#include "UpdateInfo.h"
#include "Rf433Mgr.h"


#define SYS_INFO_SIZE   sizeof(SysInfo_t)
#define SYS_INFO_USED   0x01

#define SYS_VEN_ID      "GAKATO"           /* 厂商ID */
#define SYS_DEV_MODEL   "GFK-ES-AF/W/R"    /* 设备型号 */
#define SYS_DEV_ID      "c2000000"         /* 设备编号 8位 */
#define SYS_DEV_ID_ADDR 0xF000

typedef enum {
    SYS_EVENT_NULL = 0x00,
    SYS_EVENT_KEY_3_TIMES = 0x01,
    SYS_EVENT_KEY_LONG_TIMES = 0x02,
    SYS_EVENT_RFMATCH_ING = 0x04,
    SYS_EVENT_RFMATCH_SWITCH = 0x08,
    SYS_EVENT_RFMATCH_FINISH = 0x10,
} SysEvent_t;
extern SysEvent_t g_sys_event;

#pragma pack(1)

typedef struct {
    unsigned char used;         /* 0xFF : not used */
    unsigned char id[16];       /* device id 8bit */
    unsigned char wifi[64];     /* wifi info, name and pwd use , split */
    unsigned char resetFlag;
    Rf433_t       rf433List[RF433_DEV_NUM];    
} SysInfo_t;

#pragma pack()

void SysInfoDefault(unsigned char resetFlag);
void SysInfoInit(void);
SysInfo_t SysInfoGet(void);
void SysInfoRf433CodeSave(Rf433Type_t type, int RfCode);
Rf433Type_t SysInfoRf433TypeMatch(int code, Rf433Flag_t *flag, Rf433Type_t *linkage);
void SysInfoRf433SetFlag(Rf433Type_t type, Rf433Flag_t flag);
int  SysInfoRf433GetCode(Rf433Type_t type);
void SysInfoEventDeal(void);
void SysInfoWifiSave(unsigned char *wifi);
void SysInfoWifiGet(unsigned char *wifi);
void SysInfoWifiClean(void);
unsigned char SysInfoWifiIsNull(unsigned char *wifif);
void SysInfoIDSave(unsigned char *id);
void SysInfoIDGet(unsigned char *id);
unsigned char SysInfoGetResetFlag(void);
unsigned char SysInfoClrResetFlag(void);
void SysInfoSetLinkage(Rf433Type_t type, Rf433Type_t linkage);
Rf433Type_t SysInfoGetLinkage(Rf433Type_t type);
void SysInfoCheckDevID(void);
void SysInfoReset(void);

#endif



