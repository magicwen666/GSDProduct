#ifndef __LED_H__
#define __LED_H__

typedef enum {
    LED_COLOR_NONE,
    LED_COLOR_RED,
    LED_COLOR_GREEN,
    LED_COLOR_BLUE,
    LED_COLOR_WATHER,
    LED_COLOR_YELLOW,
    LED_COLOR_PURPLE,
} led_color_t;

typedef enum {
    LED_STA_OPEN,
    LED_STA_CLOSE,
} led_sta_t;

void LEDHandler(void);
void LEDInit(void);

void LEDClose(void);
void LEDOpen(led_color_t color, unsigned char blinkCnt);

#endif


