#include "SysTimer.h"
#include "chip.h"
#include "pir.h"
#include "network.h"
#include "lock315.h"
#include "SysInfo.h"
#include "DoorMagnet.h"
#include "led.h"

static volatile unsigned int SysTimeTick = 0;
static volatile unsigned int SysTimeUs = 0;

void SysTimerInit(void) {

    /* ms */
    systick_init(1000);
    /* us */
    ct16b1_timer_init(40000);  
}

void SysTick_Handler(void) {

    SysTimeTick++;
    DoorMagnetMsHandler();
    PirHandler();
    NetWorkHandler();
    LockHandler();
    SystemResetHandler();
    LEDHandler();
}

void CT16B1_IRQHandler(void) {

    if (LPC_CT16B1->IR & IR_MR0_FLAG) {
        SysTimeUs++;
        DoorMagnetHandler();
        /* clear interrupt flag */
        LPC_CT16B1->IR = IR_MR0_FLAG;
    }

}

unsigned int SysTimerGetUs(void) {

    return SysTimeUs;
}

unsigned int SysTimerGet(void) {

    return SysTimeTick;
}

unsigned int SysTimerCalc(unsigned int pre, unsigned int now) {

    if (now >= pre) {
        return now - pre;
    }
    
    return now + 0xFFFFFFFF - pre;
}



