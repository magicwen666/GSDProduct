#include "SysInfo.h"
#include "W25qFlash.h"
#include "chip.h"
#include "SysTimer.h"
#include "key.h"
#include "shell.h"
#include "DoorMagnet.h"
#include "pir.h"
#include "delay.h"
#include "network.h"
#include "wt588d.h"
#include <stdlib.h>
#include <string.h>

static sys_info_t g_sys_info;
sys_event_t sys_event = SYS_EVENT_NONE;

#define SYS_INFO_DEFAULT_PWD    "12345678"

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
void SysInfoDefault(unsigned char reset_flag) {
    unsigned char id[16] = {0x00};
    memcpy(id, g_sys_info.id, 8);
    memset(&g_sys_info, 0x00, sizeof(sys_info_t));
    g_sys_info.used = SYS_INFO_USED;
    g_sys_info.lock315_snr = 1;
    memcpy(g_sys_info.wifi, "null", 4);
    memcpy(g_sys_info.pwd, SYS_INFO_DEFAULT_PWD, 8);
    g_sys_info.mc.flag = '0';
    SysInfoGetSystemID(id);
    memcpy(g_sys_info.id, id, 8);
    g_sys_info.reset_flag = reset_flag;
    
}

void SysInfoCheckDevID(void) {

    if (0 != strcmp((const char *)g_sys_info.id, SYS_DEV_ID)) {
        return ;
    }
    SYSLOG("Must Config Device ID\r\n");
    while (1) {
        DOG_FEED();
        ShellCmdRecv();
        KeyListened();
    }
}

void SysInfoInit(void) {
    
    UpdateInfo_t info;
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
    
    UpdateInfoGet(&info);
    SYSLOG("---------System Info---------\r\n");
    SYSLOG("LockSnr : %ld\r\n", g_sys_info.lock315_snr);
    SYSLOG("WifiInfo: %s \r\n", g_sys_info.wifi);
    SYSLOG("Mc Info : %s, %ld\r\n", 
        (('1' == g_sys_info.mc.flag) ? "open" : "close"), 
        g_sys_info.mc.time);
    SYSLOG("System ID : %s\r\n", g_sys_info.id);
    SYSLOG("Wireless Pwd : %s\r\n", g_sys_info.pwd);
    SYSLOG("System BF : %d\r\n", g_sys_info.bf);
    SYSLOG("Lock ID : %02x:%02x:%02x\r\n", g_sys_info.lockid.h, 
        g_sys_info.lockid.m, g_sys_info.lockid.l);
    SYSLOG("Ver : %s,%s\r\n", info.VersionName, info.VersionCode);
    SYSLOG("-----------------------------\r\n");
    
}

void SysInfoLock315SnrSave(unsigned int snr) {

    //W25qFlashReadData(&g_sys_info, SYS_INFO_ADDR, SYS_INFO_SIZE);
    g_sys_info.lock315_snr = snr;
    W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
    W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
    
}

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


void SysInfoMcSave(mc_t mc) {

    g_sys_info.mc = mc;
    W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
    W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
    
}

void SysInfoMcGet(mc_t *mc) {

    if (NULL == mc) {
        return ;
    }
    *mc = g_sys_info.mc;
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

void SysInfoPwdSave(unsigned char *pwd) {

    if (NULL == pwd) {
        return ;
    }
    memcpy(g_sys_info.pwd, pwd, strlen((const char *)pwd));
    W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
    W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);

}

void SysInfoPwdGet(unsigned int *pwd) {

    unsigned char i = 0;
    unsigned char *pos = NULL;

    if (NULL == pwd) {
        return ;
    }

    pos = g_sys_info.pwd;
    for (i = 0; i < 4; i++) {
        *pwd <<= 4;
        *pwd |= *pos - 0x30;
        *pwd <<= 4;
        pos++;
        *pwd |= *pos - 0x30;
        pos++;
    }
    
}

void SysInfoBfSave(unsigned char bf) {

    g_sys_info.bf = bf;
    W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
    W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
    
}

void SysInfoBfGet(unsigned char *bf) {

    if (NULL == bf) {
        return ;
    }
    
    *bf = g_sys_info.bf;
}

void SysInfoLockIDSave(lock_id_t id) {

    g_sys_info.lockid = id;
    W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
    W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);

}

unsigned char SysInfoLockIDIsAssoc(lock_id_t id) {
    
    if (0 == memcmp(&id, &g_sys_info.lockid, sizeof(lock_id_t))) {
        return 0;
    }
    return 1;
}

unsigned char SysInfoLockIDIsNull(void) {

    lock_id_t id = {0x00, 0x00, 0x00};

    if (0 == memcmp(&id, &g_sys_info.lockid, sizeof(lock_id_t))) {
        return 1;
    }

    return 0;
}

void SysInfoLockSnrGet(unsigned int *snr) {

    if (NULL == snr) {
        return ;
    }
    *snr = g_sys_info.lock315_snr;
}

void SysInfoLockSnrClean(void) {

    g_sys_info.lock315_snr = 1;
    W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
    W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
}

void SysInfoReset(unsigned char ResetFlag) {
    

    SysInfoDefault(ResetFlag);
    W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
    W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
}

static volatile unsigned int SystemResetTick = 0;
#define SYSTEM_RESET_TIMEOUT    30000
void SystemResetHandler(void) {
    if (0 != SystemResetTick) {
        SystemResetTick--;
    }
}


void SystemReset(void) {
    
    WT588DStartPlay(WT588D_CONTEX_RESET_SUCC);
    if (0 == NetWorkSendWarn(NET_WARN_TYPE_WG_RESET, NULL)) {
        SysInfoReset(0);
    } else {
        SysInfoReset(1);
    }
    delay_ms(1000);
    NVIC_SystemReset();
    
}

unsigned char SysInfoGetResetFlag(void) {

    return g_sys_info.reset_flag;
}

unsigned char SysInfoClrResetFlag(void) {

    g_sys_info.reset_flag = 0;
    W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
    W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
    
    return 0;
}

unsigned int  SysInfoGetRf433Data(void) {

    return g_sys_info.rf433_data;
}

void SysInfoSetRf433Data(unsigned int data) {

    g_sys_info.rf433_data = data;
    W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
    W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
}



