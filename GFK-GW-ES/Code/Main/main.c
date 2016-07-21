#include "chip.h"
#include "wifi.h"
#include "delay.h"
#include "key.h"
#include "KeyMgr.h"
#include "W25qFlash.h"
#include "shell.h"
#include "SysTimer.h"
#include "led.h"
#include "rf433.h"
#include "SysInfo.h"
#include "wt588d.h"
#include "network.h"
#include "Rf433Mgr.h"
#include <string.h>

int fputc(int ch, FILE *fp) {


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
    W25qFlashInit();
    SysInfoInit();
    WT588DInit();
    KeyInit();
    RF433Init();
    SysInfoCheckDevID();
    if (WIFI_RET_SUCCESS != WifiInit()) {
        LOG("WIFI Work Failure\r\n");
        WifiResetModule();
        NVIC_SystemReset();
    }
    DOG_INIT();
}

static void VectorRemap(void) {
    unsigned char i = 0;
    unsigned int *pUser = (unsigned int *)APP_CODE_START_ADDR;
    unsigned int *pRam = (unsigned int *)0x10000000;
    for (i = 0; i < 48; i++) {
        *pRam++ = *pUser++;
    }
    LPC_SYSCON->SYSMEMREMAP = 0x01;
    SystemCoreClockUpdate();
}
int main(void) {
    
    VectorRemap();
    LogInit();
    DriverInit();
    UpdateInfoSetVersion("0.0.2", "2016070510");
    NetWorkInit();
    SYSLOG("System Normal Work\r\n");
 
    while (1) {
        DOG_FEED();
        KeyMgrListen();
        ShellCmdRecv();
        NetWorkRecv();
        NetWorkSendHeart();
        NetWorkCheckHeart();
        RF433MgrTask(); 
    }
}


void HardFault_Handler(void) {
    
    SYSLOG("HardFault_Handler\r\n");
    delay_ms(1000);
    NVIC_SystemReset();
    while (1);
}






