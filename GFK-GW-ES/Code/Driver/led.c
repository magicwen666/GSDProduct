#include "led.h"
#include "chip.h"

#define LED_OPEN()   gpio_set_bitval(LPC_GPIO2, 9, LED_STA_OPEN)
#define LED_CLOSE()  gpio_set_bitval(LPC_GPIO2, 9, LED_STA_CLOSE)

#define LED_TICK_TIMEOUT    100
static volatile unsigned char ledTick = LED_TICK_TIMEOUT; /* 50ms */
static volatile unsigned char ledBlinkFlag = 0;
static volatile unsigned char ledBlinkSwitch = 0;

void LEDHandler(void) {
#if 0 
    if (0 == ledBlinkSwitch) {
        return ;
    }
    if (0 != (ledTick--)) {
        return ;
    }
    ledTick = LED_TICK_TIMEOUT;
    if (0 == ledBlinkFlag) {
        ledBlinkFlag = 1;
        LEDClose();
        return ;
    } 
    ledBlinkFlag = 0;
    LED_OPEN();
#endif 
}

void LEDInit(void) {

#if 0
    syscon_ahb_enable(SYSCON_CLK_IOCON);
    /*  P2_9 */
    LPC_IOCON->PIO2_9 |= IOCON_FUNC0 | IOCON_MODE_PULLUP;
    
    syscon_ahb_enable(SYSCON_CLK_GPIO);
    gpio_set_dir(LPC_GPIO2, 9, GPIO_OUT);
    
    LEDClose();
#endif
}

void LEDOpen(unsigned char blinkSwitch) {

#if 0
    ledBlinkSwitch = blinkSwitch;
    LED_OPEN();
#endif  
}

void LEDClose(void) {
#if 0
    ledBlinkSwitch = 0;
    LEDClose();
#endif
}


