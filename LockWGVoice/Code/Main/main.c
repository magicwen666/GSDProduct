#include "chip.h"
#include "lock315.h"
#include "wifi.h"
#include "delay.h"
#include "key.h"
#include "W25qFlash.h"
#include "SysInfo.h"
#include "shell.h"
#include "SysTimer.h"
#include "pir.h"
#include "led.h"
#include "network.h"
#include "DoorMagnet.h"
#include "key.h"
#include "UpdateInfo.h"
#include "wt588d.h"
#include <string.h>

int fputc(int ch, FILE *fp) {

    //if ('\n' == ch) {
        //uart0_send_byte('\r');
    //}
    uart0_send_byte(ch);

    return ch;
}

static void LogInit(void) {
    
    syscon_ahb_enable(SYSCON_CLK_IOCON);
    LPC_IOCON->PIO1_6 |= (IOCON_FUNC1 | IOCON_MODE_PULLUP);
    LPC_IOCON->PIO1_7 |= (IOCON_FUNC1 | IOCON_MODE_PULLUP);
    
    LOGInit();
    SYSLOG("The System Start Up[%s %s]\r\n", __DATE__, __TIME__);

}

static void DriverInit(void) {

    SysTimerInit();
    WT588DInit();
    LEDInit();
    if (1 == W25qFlashInit()) {
        WT588DStartPlay(WT588D_CONTEX_FLASH_INIT_ERR);
        while (1);
    }
    SysInfoInit();
    KeyInit();
    DoorMagnetInit();
    Lock315Init();
    PirInit();
    if (WIFI_RET_SUCCESS != WifiInit()) {
        LOG("WIFI Work Failure\r\n");
        WifiResetModule();
        NVIC_SystemReset();
    }
    DOG_INIT();
    SysInfoCheckDevID(); 
    
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
    LogInit();
    DriverInit();
    UpdateInfoSetVersion("0.0.2", "2016070510");
    NetWorkInit();
    if (1 == SysInfoLockIDIsNull()) {
        WT588DStartPlay(WT588D_CONTEX_LOCK_ASSOC_NONE);
    }
    SYSLOG("System Normal Work\r\n");

    while (1) {
        DOG_FEED();
        Lock315Recv();
        ShellCmdRecv();
        NetWorkRecv();
        NetWorkSendHeart();
        NetWorkCheckHeart();
        KeyListened();
        PirCheck();
        DoorMagnetRecv();
    }
}


void HardFault_Handler(void) {
    
    SYSLOG("HardFault_Handler\r\n");
    delay_ms(1000);
    NVIC_SystemReset();
    while (1);
}






