#include "chip.h"
#include "delay.h"
#include "key.h"
#include "shell.h"
#include "SysTimer.h"
#include "beep.h"
#include "BleS02.h"
#include "W25qFlash.h"
#include "SysInfo.h"
#include "JwLock.h"
#include "rs485.h"
#include "frame.h"
#include "UpdateInfo.h"
#include "KeyMgr.h"

static void DriverInit(void) {
    
    SysTimerInit();
    KeyMgrInit();
    BeepInit();
    if (1 == W25qFlashInit()) {
        LOG("Flash Init Failure\r\n");
        while (1);
    }
    BleS02Init();
    DOG_INIT();
    
}

static void VectorRemap(void) {
   
    unsigned char i = 0;
    unsigned int *pUser = (unsigned int *)APP_CODE_START_ADDR;
    unsigned int *pRam = (unsigned int *)0x10000000;
    
    for (i = 0; i < 48; i++) {
        *pRam++ = *pUser++;
    }
    /*  Interrupt vectors are re-mapped to Static RAM. */
    LPC_SYSCON->SYSMEMREMAP = 0x01;
    SystemCoreClockUpdate();
}

int main(void) {
    
    VectorRemap();
#ifdef DEBUG
    LOGInit();
#else
    RS485Init();
#endif
    LOG("the system start up[%s %s]\r\n", __DATE__, __TIME__);
    DriverInit();
    SysInfoInit();
    JwLockInit();
    
    LOG("System Normal Work\r\n");
    while (1) {
        DOG_FEED();
        ShellCmdRecv();
        FrameRecv();
        JwLockRecv();
        KeyMgrListened();
    }
    
}






