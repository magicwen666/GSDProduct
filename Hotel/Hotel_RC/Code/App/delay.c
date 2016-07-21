#include "delay.h"

void delay_us(unsigned int us) {
#if 0 
    unsigned int tmp = 0;
    
    SysTick->LOAD = (SystemCoreClock / (1000 * 1000) - 1) * us;
    SysTick->VAL  = 0x00;
    SysTick->CTRL = 0x01; 
    
    do {
        tmp = SysTick->CTRL;
    } while ((tmp & 0x01) && (!(tmp & (1<<16))));
    SysTick->CTRL = 0x00;
    SysTick->VAL  = 0x00;
#else
    while (us--);
#endif

}

void delay_ms(unsigned int ms) {

#if 0  
    unsigned int tmp = 0;
    
    SysTick->LOAD = (SystemCoreClock / 1000 - 1) * ms;
    SysTick->VAL  = 0x00;
    SysTick->CTRL = 0x01; 
    
    do {
        tmp = SysTick->CTRL;
    } while ((tmp & 0x01) && (!(tmp & (1<<16))));
    SysTick->CTRL = 0x00;
    SysTick->VAL  = 0x00;
 #endif   
    unsigned int cnt = 5000;

    while (ms--) {
        cnt = 5000;
        while (cnt--);
    }
}


