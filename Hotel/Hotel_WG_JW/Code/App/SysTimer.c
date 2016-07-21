#include "SysTimer.h"
#include "chip.h"
#include "beep.h"
#include "BleS02.h"
#include "JwLock.h"

static volatile unsigned int SysTimeTick = 0;

void SysTimerInit(void) {

    /* ms */
    systick_init(1000);
    /* us */
    ct16b1_timer_init(40000);  
}

void SysTick_Handler(void) {

    SysTimeTick++;
    BeepHandler();
    BleS02Handler();
    JwLockHandler();
}

void CT16B1_IRQHandler(void) {

    if (LPC_CT16B1->IR & IR_MR0_FLAG) {
        /* clear interrupt flag */
        LPC_CT16B1->IR = IR_MR0_FLAG;
    }

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



