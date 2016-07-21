#include "UpdateInfo.h"
#include "W25qFlash.h"
//#include "iap.h"
#include <stdlib.h>
#include <string.h>

void (*AppCodeRun)(void) = (void (*)(void))(APP_CODE_START_ADDR + 1);

#if 0
void UpdateInfoInit(void) {

    UpdateInfo_t info;

    if (W25qFlashInit()) {
        while (1);
    }
    //W25qFlashEraseSector(UPDATE_INFO_SECTOR >> 12);
    W25qFlashReadData(&info, UPDATE_INFO_SECTOR, sizeof(UpdateInfo_t));
    LOG("Update Flag : %s\r\n", (1 == info.NeedFlag) ? ("yes") : ("no"));
    LOG("Version Name: %s\r\n", info.VersionName);
    LOG("Version Code: %s\r\n", info.VersionCode);
    LOG("Packet Size : %d\r\n", info.size);
    LOG("MD5(32bit)  : %s\r\n", info.md5);
    if (0xFF == info.NeedFlag) {
        W25qFlashEraseSector(UPDATE_INFO_SECTOR >> 12);
        memset(&info, 0x00, sizeof(UpdateInfo_t));
        info.NeedFlag = UPDATE_FLAG_NO;
        strcpy(info.VersionName, "0.0.1");
        strcpy(info.VersionCode, "2016060301");
        W25qFlashWriteData(&info, UPDATE_INFO_SECTOR, sizeof(UpdateInfo_t));
    }
    
}
#endif

char UpdateInfoIsUpdate(void) {

    UpdateInfo_t info;
    
    W25qFlashReadData(&info, UPDATE_INFO_SECTOR, sizeof(UpdateInfo_t));

    return info.NeedFlag;
}

void UpdateInfoSetVersion(const char *VersionName, const char *VersionCode) {

    UpdateInfo_t info;

    if ((NULL == VersionName) || (NULL == VersionCode)) {
        return ;
    }

    W25qFlashReadData(&info, UPDATE_INFO_SECTOR, sizeof(UpdateInfo_t));
    memset(info.VersionName, 0x00, sizeof(info.VersionName));
    memset(info.VersionCode, 0x00, sizeof(info.VersionCode));
    memcpy(info.VersionName, VersionName, strlen(VersionName));
    memcpy(info.VersionCode, VersionCode, strlen(VersionCode));
    UpdateInfoSet(&info);

}

void UpdateInfoSet(UpdateInfo_t *info) {

    if (NULL == info) {
        return ;
    }
    
    W25qFlashEraseSector(UPDATE_INFO_SECTOR >> 12);
    W25qFlashWriteData(info, UPDATE_INFO_SECTOR, sizeof(UpdateInfo_t));
}

void UpdateInfoGet(UpdateInfo_t *info) {

    if (NULL == info) {
        return ;
    }

    W25qFlashReadData(info, UPDATE_INFO_SECTOR, sizeof(UpdateInfo_t));

}

void UpdateInfoAppCodeErase(void) {
    
    int i = 0;
    unsigned int sector = 0;

    for (i = 0; i < APP_CODE_SECTOR_SIZE; i++) {
        sector = APP_CODE_SECTOR + i * W25Q_FLASH_SECTOR_SIZE;
        W25qFlashEraseSector(sector >> 12);
    }
    
    
}

void UpdateInfoAppCodeSave(void * buf,unsigned long addr,unsigned int len) {
    
    W25qFlashWriteData(buf, addr, len);

}

#if 0
void UpdateInfoAppCodeUpdate(void) {

    UpdateInfo_t info;
    int size = 0;
    INT8U startSec = 0;
    INT8U endSec = 0;
    char readBuf[512] = {0x00};
    unsigned long readAddr = APP_CODE_SECTOR;
    INT32U writeAddr = APP_CODE_START_ADDR;
    
    W25qFlashReadData(&info, UPDATE_INFO_SECTOR, sizeof(UpdateInfo_t));
    if (UPDATE_FLAG_NO == info.NeedFlag) {
        (*AppCodeRun)();
        return ;    /* forever not run */
    }
    if (0 == info.size) {
        LOG("App Code Error!\r\n");
        while (1);
    }
    startSec = APP_CODE_START_ADDR / 4096;
    endSec = startSec + info.size / 4096;
    IAP_Entry = (void(*)())IAP_ENTER_ADR;  
    __disable_irq();
    sectorPrepare(startSec, endSec);
    sectorErase(startSec, endSec);
    __enable_irq();
    do {
        size = UPDATE_PACK_SIZE;
        if (info.size <= UPDATE_PACK_SIZE) {
            size = info.size;
        }
        W25qFlashReadData(readBuf, readAddr, size);
        __disable_irq();
        sectorPrepare(startSec, endSec);
        ramCopy(writeAddr, (INT32U)readBuf, UPDATE_PACK_SIZE);
        __enable_irq();
        memset(readBuf, 0x00, sizeof(readBuf));
        readAddr += UPDATE_PACK_SIZE;
        writeAddr += UPDATE_PACK_SIZE;
        info.size -= size;
    } while (0 != (info.size));
    
    free(readBuf);
    LOG("App Code Update Finish\r\n");
    (*AppCodeRun)();
    
}
#endif


