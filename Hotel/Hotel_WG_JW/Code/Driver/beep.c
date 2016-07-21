#include "beep.h"
#include "chip.h"
#include "delay.h"

#define BEEP_OPEN()     (LPC_GPIO2->DATA |= (1<<3))
#define BEEP_CLOSE()    (LPC_GPIO2->DATA &= ~(1<<3))
static volatile unsigned int BeepTick = 0;
#define BEEP_FLAG_OPEN  0
#define BEEP_FALG_CLOSE 1
static volatile unsigned char BeepFlag = BEEP_FALG_CLOSE;

void BeepHandler(void) {

    if (0 == BeepTick) {
        if (BEEP_FLAG_OPEN == BeepFlag) {
            BeepFlag = BEEP_FALG_CLOSE;
            BEEP_CLOSE();
        }
        return ;
    }
    BeepTick--;

}


void BeepInit(void) {

    syscon_ahb_enable(SYSCON_CLK_IOCON);
    /* p2_3 */
    LPC_IOCON->PIO2_3 |= IOCON_FUNC0;
    syscon_ahb_enable(SYSCON_CLK_GPIO);
    gpio_set_dir(LPC_GPIO2, 3, GPIO_OUT);
    
    BeepThreeRing();

}

void BeepLongRing(void) {

    BeepFlag = BEEP_FLAG_OPEN;
    BeepTick = BEEP_LONG_TICK;
    BEEP_OPEN();
}

void BeepThreeRing(void) {

    unsigned char cnt = 3;

    while (cnt--) {
        DOG_FEED();
        BEEP_OPEN();
        delay_ms(500);
        BEEP_CLOSE();
        delay_ms(300);
    }

}



