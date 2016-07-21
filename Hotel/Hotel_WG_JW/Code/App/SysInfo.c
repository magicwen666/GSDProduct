#include "SysInfo.h"
#include "W25qFlash.h"
#include "beep.h"
#include "delay.h"
#include <string.h>

static sys_info_t g_sys_info;

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

static void SysInfoDefault() {

    //sys_id_t id = g_sys_info.id;

    memset(&g_sys_info, 0x00, sizeof(sys_info_t));
    g_sys_info.used = SYS_INFO_USED;
    memcpy(g_sys_info.BleMac, "null", 4);
    memcpy(g_sys_info.BleName, "null", 4);
    //SysInfoGetSystemID(id);
    
}

void SysInfoInit(void) {
    
    //W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
    W25qFlashReadData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
    if (SYS_INFO_USED != g_sys_info.used) {
        W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
        SysInfoDefault();
        W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
    }
    LOG("---------System Info---------\r\n");
    LOG("BleMac    : %s\r\n", g_sys_info.BleMac);
    LOG("BleName   : %s\r\n", g_sys_info.BleName);
    LOG("System ID : %02x%02x%02x%02x\r\n", 
        g_sys_info.id.id[0], g_sys_info.id.id[1], g_sys_info.id.id[2], g_sys_info.id.id[3]);
    LOG("RoomNum   : %02x%02x\r\n", g_sys_info.room.floor, g_sys_info.room.num);
    LOG("-----------------------------\r\n");
    
}

void SysInfoBleInfoSet(unsigned char *mac, unsigned char *name) {
    
    if (NULL != mac) {
        memset(g_sys_info.BleMac, 0x00, sizeof(g_sys_info.BleMac));
        memcpy(g_sys_info.BleMac, mac, strlen((char *)mac));
    }

    if (NULL != name) {
        memset(g_sys_info.BleName, 0x00, sizeof(g_sys_info.BleName));
        memcpy(g_sys_info.BleName, name, strlen((char *)name));
    }

    W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
    W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
}

void SysInfoBleInfoGet(unsigned char *mac, unsigned char *name) {

    if (NULL != mac) {
        memcpy(mac, g_sys_info.BleMac, strlen((char *)g_sys_info.BleMac));
    }
    if (NULL != name) {
        memcpy(name, g_sys_info.BleName, strlen((char *)g_sys_info.BleName));
    }
}

unsigned char SysInfoBleIsNull(void *mac) {

    if (0 == memcmp(g_sys_info.BleMac, "null", 4)) {
        return 1;
    }
    if (NULL != mac) {
        strcpy(mac, (char *)g_sys_info.BleMac);
    }
    return 0;
}

void SysInfoRoomNumSet(sys_room_t room) {

    g_sys_info.room = room;
    W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
    W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
}

void SysInfoRoomNumGet(sys_room_t *room) {

    if (NULL == room) {
        return ;
    }
    *room = g_sys_info.room;
}

void SysInfoIDSet(sys_id_t id) {

    g_sys_info.id = id;
    W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
    W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
}

sys_id_t SysInfoIDGet(void) {

    return g_sys_info.id;
}

void SysInfoReset(void) {

    sys_info_t info = g_sys_info;
    
    SysInfoDefault();
    g_sys_info.id = info.id;
    g_sys_info.room = info.room;
    W25qFlashEraseSector(SYS_INFO_SECTOR >> 12);
    W25qFlashWriteData(&g_sys_info, SYS_INFO_SECTOR, SYS_INFO_SIZE);
    BeepLongRing();
    delay_ms(3000);
    NVIC_SystemReset();
}



