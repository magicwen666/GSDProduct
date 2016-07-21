#include "SysInfo.h"
#include "W25qFlash.h"
#include "led.h"
#include "KeyMgr.h"
#include "rf433.h"
#include "delay.h"
#include "wt588d.h"
#include "Rf433Mgr.h"
#include "shell.h"
#include "network.h"
#include "key.h"
#include <string.h>

static SysInfo_t g_sys_info;
SysEvent_t g_sys_event = SYS_EVENT_NULL;

static void SysInfoGetRf433Data(Rf433_t rf433List[], unsigned char used) {

    unsigned char i = 0;
    
    if (SYS_INFO_USED != used) {
        for (i = 0; i < RF433_DEV_NUM; i++) {
            rf433List[i].type = (Rf433Type_t)(i + 2);
            rf433List[i].code = 0;
            rf433List[i].flag = RF433_FLAG_CLOSE;
            rf433List[i].linkage = RF433_TYPE_NONE;
        }
        return ;
    }
    memcpy(rf433List, g_sys_info.rf433List, sizeof(Rf433_t) * RF433_DEV_NUM);

}

static unsigned char SysInfoIsValidID(unsigned char *id) {
    
    unsigned char i = 0;

    if ('c' != id[0]) {
        return 1;
    }
    
    for (i = 1; i < 8; i++) {
        if ((id[i] < '0') || (id[i] > '9')) {
            return 1;
        }
    }

    return 0;
}

/* return 1:change 0 : nochange */
static unsigned char SysInfoGetSystemID(unsigned char *id) {

    unsigned char *idAddr = (unsigned char *)SYS_DEV_ID_ADDR;
    unsigned char i = 0;
    unsigned char FlashId[16] = {0x00};
    unsigned char *pos = FlashId;

    if (0 == SysInfoIsValidID(id)) {
        return 0;
    }
    for (i = 0; i < 4; i++) {
        sprintf((char *)pos, "%02x", *idAddr++);
        pos += 2;
    }
    
    if (0 == SysInfoIsValidID(FlashId)) {
        memcpy(id, FlashId, 8);
        return 1;
    }

    memcpy(id, SYS_DEV_ID, 8);

    return 1;
}

void SysInfoDefault(unsigned char resetFlag) {

    Rf433_t rf433List[RF433_DEV_NUM];
    unsigned char id[16] = {0x00};

    memcpy(id, g_sys_info.id, 8);
    SysInfoGetRf433Data(rf433List, g_sys_info.used);
    memset(&g_sys_info, 0x00, SYS_INFO_SIZE);
    g_sys_info.used = SYS_INFO_USED;
    SysInfoGetSystemID(id);
    memcpy(g_sys_info.id, id, 8);
    memcpy(g_sys_info.wifi, "null", 4);
    g_sys_info.resetFlag = resetFlag;
    memcpy(g_sys_info.rf433List, rf433List, sizeof(Rf433_t) * RF433_DEV_NUM);
    
}

static void SysInfoRf433Print(void) {

    unsigned char i = 0;

    for (i = 0; i < RF433_DEV_NUM; i++) {
        switch (g_sys_info.rf433List[i].type) {
            case RF433_TYPE_DOOR : {
                SYSLOG("门磁 : %08x ", g_sys_info.rf433List[i].code);
            } break;
            case RF433_TYPE_PIR : {
                SYSLOG("红外 : %08x ", g_sys_info.rf433List[i].code);
            } break;
            case RF433_TYPE_SMOKE : {
                SYSLOG("烟雾 : %08x ", g_sys_info.rf433List[i].code);
            } break;
            case RF433_TYPE_REMOTE_CONTROL : {
                SYSLOG("遥控 : %08x ", g_sys_info.rf433List[i].code);
            } break;
            default : {
                continue;
            }
        }
        SYSLOG("%s\r\n", ('0' == g_sys_info.rf433List[i].flag) ? "<关闭>" : "<打开>");
    }

}

void SysInfoInit(void) {

    //W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
    W25qFlashReadData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
    if (SYS_INFO_USED != g_sys_info.used) {
        W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
        SysInfoDefault(0);
        W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
    } else {
        if (1 == SysInfoGetSystemID(g_sys_info.id)) {
            W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
            W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
        }
    }
    SYSLOG("---------System Info---------\r\n");
    SYSLOG("WifiInfo: %s \r\n", g_sys_info.wifi);
    SYSLOG("System ID : %s\r\n", g_sys_info.id);
    SYSLOG("Reset Flag : %d\r\n", g_sys_info.resetFlag);
    SysInfoRf433Print();
    SYSLOG("-----------------------------\r\n");
    
}

SysInfo_t SysInfoGet(void) {
    
    return g_sys_info;
}

void SysInfoRf433CodeSave(Rf433Type_t type, int RfCode) {

    unsigned char i = 0;
    
    for (i = 0; i < RF433_DEV_NUM; i++) {
        if (type == g_sys_info.rf433List[i].type) {
            g_sys_info.rf433List[i].code = RfCode;
            W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
            W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
            return ;
        }
    }
    
}

Rf433Type_t SysInfoRf433TypeMatch(int code, Rf433Flag_t *flag, Rf433Type_t *linkage) {

    unsigned char i = 0;

    for (i = 0; i < RF433_DEV_NUM; i++) {
        if (code == g_sys_info.rf433List[i].code) {
            if (NULL != flag) {
                *flag = g_sys_info.rf433List[i].flag;
            }
            if (NULL != linkage) {
                *linkage = g_sys_info.rf433List[i].linkage;
            }
            return g_sys_info.rf433List[i].type;
        }
        if (RF433_TYPE_REMOTE_CONTROL == g_sys_info.rf433List[i].type) {
            if ((code & 0x00FFFFF0) != (g_sys_info.rf433List[i].code & 0x00FFFFF0)) {
                continue;
            }
            if (NULL != flag) {
                *flag = g_sys_info.rf433List[i].flag;
            }
            return RF433_TYPE_REMOTE_CONTROL;
        }
    }

    return RF433_TYPE_NONE;
}

void SysInfoRf433SetFlag(Rf433Type_t type, Rf433Flag_t flag) {

    unsigned char i = 0;

    for (i = 0; i < RF433_DEV_NUM; i++) {
        if (RF433_TYPE_ALL == type) {
            g_sys_info.rf433List[i].flag = flag;
            continue;
        }
        if (g_sys_info.rf433List[i].type == type) {
            g_sys_info.rf433List[i].flag = flag;
            break;
        }
    }
    
}

int  SysInfoRf433GetCode(Rf433Type_t type) {

    unsigned char i = 0;

    for (i = 0; i < RF433_DEV_NUM; i++) {
        if (g_sys_info.rf433List[i].type != type) {
            continue;
        }
        return g_sys_info.rf433List[i].code;
    }

    return 0;
}


#if 0
void SysInfoEventDeal(void) {

    unsigned char i = 0;

    if (SYS_EVENT_NULL == g_sys_event) {
        return ;
    }
    
    for (i = 0; i < 8; i++) {
        switch (g_sys_event & (1 << i)) {
            case SYS_EVENT_KEY_3_TIMES : {
                g_sys_event &= ~(SYS_EVENT_KEY_3_TIMES);
                NetWorkCloseLink();
                Rf433DevMgrMatchCode();
            } break;
            case SYS_EVENT_KEY_LONG_TIMES : {
                
            } break;
            default : {
            } break;
        }
        /* clear */
        g_sys_event &= ~(1 << i);
    }

}
#endif

void SysInfoWifiSave(unsigned char *wifi) {
    
    if (NULL == wifi) {
        return ;
    }
    
    memset(g_sys_info.wifi, 0x00, sizeof(g_sys_info.wifi));
    memcpy(g_sys_info.wifi, wifi, strlen((const char *)wifi));
    W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
    W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
}

void SysInfoWifiGet(unsigned char *wifi) {

    if (NULL == wifi) {
        return ;
    }
    
    memcpy(wifi, g_sys_info.wifi, strlen((const char *)(g_sys_info.wifi)));
}

void SysInfoWifiClean(void) {

    memset(g_sys_info.wifi, 0x00, sizeof(g_sys_info.wifi));
    memcpy(g_sys_info.wifi, "null", 4);
    W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
    W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
}

unsigned char SysInfoWifiIsNull(unsigned char *wifi) {

    if (0 == memcmp(wifi, "null", 4)) {
        return 1;
    }

    return 0;
}

void SysInfoIDSave(unsigned char *id) {

    if (NULL == id) {
        return ;
    }
    memset(g_sys_info.id, 0x00, sizeof(g_sys_info.id));
    memcpy(g_sys_info.id, id, strlen((const char *)id));
    W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
    W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
    
}

void SysInfoIDGet(unsigned char *id) {

    if (NULL == id) {
        return ;
    }
    memcpy(id, g_sys_info.id, strlen((const char *)g_sys_info.id));
}

unsigned char SysInfoGetResetFlag(void) {

    return g_sys_info.resetFlag;
}

unsigned char SysInfoClrResetFlag(void) {

    g_sys_info.resetFlag = 0;
    W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
    W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
    
    return 0;
}

void SysInfoSetLinkage(Rf433Type_t type, Rf433Type_t linkage) {

    unsigned char i = 0;

    for (i = 0; i < RF433_DEV_NUM; i++) {
        if (type != g_sys_info.rf433List[i].type) {
            continue;
        }
        g_sys_info.rf433List[i].linkage = linkage;
        W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
        W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
        break;
    }
    
}

Rf433Type_t SysInfoGetLinkage(Rf433Type_t type) {
    
    unsigned char i = 0;

    for (i = 0; i < RF433_DEV_NUM; i++) {
        if (type != g_sys_info.rf433List[i].type) {
            continue;
        }
        break;
    }
    return (g_sys_info.rf433List[i].linkage);
}

void SysInfoCheckDevID(void) {

    if (0 != strcmp((const char *)g_sys_info.id, SYS_DEV_ID)) {
        return ;
    }
    WT588DStartPlay(WT588D_CONTEX_NO_ID);
    LOG("Must Config Device ID\r\n");
    while (1) {
        DOG_FEED();
        ShellCmdRecv();
        KeyMgrListen();
    }
}

void SysInfoReset(void) {
    
    WT588DStartPlay(WT588D_CONTEX_RESET_SUCC);
    if (0 == NetWorkSendWarn(NET_WARN_TYPE_WG_RESET, NULL)) {
        SysInfoDefault(0);
    } else {
        SysInfoDefault(1);
    }
    W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
    W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
    delay_ms(2000);
    NVIC_SystemReset();
}





