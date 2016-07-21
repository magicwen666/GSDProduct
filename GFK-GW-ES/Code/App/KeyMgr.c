#include "KeyMgr.h"
#include "key.h"
#include "chip.h"
#include "delay.h"
//#include "Rf433Mgr.h"
#include "SysInfo.h"
#include "W25qFlash.h"

#define KEY_CNT_TIME_OUT    2000
volatile unsigned int KeyCntTime = 0;
volatile unsigned char KeyCnt = 0;
volatile unsigned int KeyDownTime = 0;
#define KEY_DOWN_TIME_OUT    500
#define KEY_CNT_TIME_OUT    2000

void KeyMgrHandler(void) {

    if (0 != KeyCntTime) {
        KeyCntTime--;
    } else {
        KeyCnt = 0;
    }

    if (0 != KeyDownTime) {
        KeyDownTime--;
    }
}

void KeyMgrListen(void) {

    unsigned char sta = KeyStaSwitch(KeyScan());

    switch (sta) {
        case KEY_STA_DOWN : {
            delay_ms(250);
            if (0 == KEY_VAL) {
                break;
            }
            KeyCntTime = KEY_CNT_TIME_OUT;
            if (SYS_EVENT_RFMATCH_ING == (g_sys_event & SYS_EVENT_RFMATCH_ING)) {
                g_sys_event |= SYS_EVENT_RFMATCH_SWITCH;
                break;
            } 
            if (3 == (++KeyCnt)) {
                KeyCnt = 0;
                Rf433MgrMatchCode();
                return ;
            }
        } break;
        case KEY_STA_LHOLD : {
            if (SYS_EVENT_RFMATCH_ING == (g_sys_event & SYS_EVENT_RFMATCH_ING)) {
                g_sys_event &= ~SYS_EVENT_RFMATCH_ING;
                g_sys_event |= SYS_EVENT_RFMATCH_FINISH;
                KeyDownTime = 5000;
                break;
            } 
            if (0 != KeyDownTime) {
                break;
            }
            /* reset */
            SysInfoReset();
        } break;
    }
    
}



