#include "led.h"
#include "chip.h"

#define LED_R(v)     gpio_set_bitval(LPC_GPIO1, 4, v)
#define LED_G(v)     gpio_set_bitval(LPC_GPIO1, 5, v)
#define LED_B(v)     gpio_set_bitval(LPC_GPIO3, 3, v)

static volatile led_color_t ledColor = LED_COLOR_NONE;
#define LED_TICK_TIMEOUT    100
static volatile unsigned char ledTick = LED_TICK_TIMEOUT; /* 50ms */
static volatile unsigned char ledBlinkCnt = 0;
static volatile unsigned char ledBlinkFlag = 0;


void LEDHandler(void) {
    
    if (0 == ledBlinkCnt) {
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
    ledBlinkCnt--;
    switch (ledColor) {
        case LED_COLOR_RED : {
            LED_R(LED_STA_OPEN);
        } break;
        case LED_COLOR_GREEN : {
            LED_G(LED_STA_OPEN);
        } break;
        case LED_COLOR_BLUE : {
            LED_B(LED_STA_OPEN);
        } break;
        case LED_COLOR_WATHER : {
            LED_G(LED_STA_OPEN);
            LED_B(LED_STA_OPEN);
        } break;
        case LED_COLOR_YELLOW : {
            LED_R(LED_STA_OPEN);
            LED_G(LED_STA_OPEN);
        } break;
        case LED_COLOR_PURPLE : {
            LED_R(LED_STA_OPEN);
            LED_B(LED_STA_OPEN);
        } break;
        default : {
        } break;
    }
    
}

void LEDInit(void) {

    syscon_ahb_enable(SYSCON_CLK_IOCON);
    /* LED_R P1_4 */
    LPC_IOCON->PIO1_4 |= IOCON_FUNC0 | IOCON_MODE_PULLUP;
    /* LED_G P1_5 */
    LPC_IOCON->PIO1_5 |= IOCON_FUNC0 | IOCON_MODE_PULLUP;
    /* LED_B P3_3 */
    LPC_IOCON->PIO3_3 |= IOCON_FUNC0 | IOCON_MODE_PULLUP;
    
    syscon_ahb_enable(SYSCON_CLK_GPIO);
    gpio_set_dir(LPC_GPIO1, 4, GPIO_OUT);
    gpio_set_dir(LPC_GPIO1, 5, GPIO_OUT);
    gpio_set_dir(LPC_GPIO3, 3, GPIO_OUT);
    
    LEDOpen(LED_COLOR_RED, 3);
}

void LEDClose(void) {

    LED_R(LED_STA_CLOSE);
    LED_G(LED_STA_CLOSE);
    LED_B(LED_STA_CLOSE);
}

void LEDOpen(led_color_t color, unsigned char blinkCnt) {

    if (0 != ledBlinkCnt) {
        return ;
    }
    ledColor = color;
    ledBlinkCnt = blinkCnt;
    LEDClose();
    switch (color) {
        case LED_COLOR_RED : {
            LED_R(LED_STA_OPEN);
        } break;
        case LED_COLOR_GREEN : {
            LED_G(LED_STA_OPEN);
        } break;
        case LED_COLOR_BLUE : {
            LED_B(LED_STA_OPEN);
        } break;
        case LED_COLOR_WATHER : {
            LED_G(LED_STA_OPEN);
            LED_B(LED_STA_OPEN);
        } break;
        case LED_COLOR_YELLOW : {
            LED_R(LED_STA_OPEN);
            LED_G(LED_STA_OPEN);
        } break;
        case LED_COLOR_PURPLE : {
            LED_R(LED_STA_OPEN);
            LED_B(LED_STA_OPEN);
        } break;
        default : {
        } break;
    }
    
}


