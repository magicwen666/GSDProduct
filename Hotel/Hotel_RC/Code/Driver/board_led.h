#ifndef __BOARD_LED_H__
#define __BOARD_LED_H__

// 头文件
#include "sys.h"

// 宏定义
#define LED1 0x01
#define LED2 0x02
#define LED3 0x04
#define LED4 0x08
#define LED5 0x10
#define LED6 0x20
#define LEDALL  0x3F

typedef uint8_t led_t;

typedef struct
{
    uint8_t led_name;
    uint8_t back_color_index;
    uint8_t fore_color_index;
    //color_t back_color;
    //color_t fore_color;
}oled_t;


#define forelight_level0 70
#define forelight_level1 80
#define forelight_level2 90
#define forelight_level3 100

typedef enum
{
    forelight_mode0,    //
    forelight_mode1,    //
    forelight_mode2,    //
    forelight_mode3     //
}forelight_mode_t;

// 函数
void handler_flash_led(void);
void handler_rotate_led(void);
void board_led_init(void);
void board_led_open(led_t leds);
void board_led_close(led_t leds);
void board_led_refresh(void);
void board_led_display_en(void);
void board_led_display_dis(void);
void board_led_flash_start(void);
void board_led_flash_set(led_t leds);
void board_led_flash_stop(void);
void board_led_flash_add(led_t leds);
void board_led_flash_del(led_t leds);
void board_led_rotate_start(void);
void board_led_rotate_stop(void);
void board_led_backlight_set(led_t leds, rgb_t color);
void board_led_forelight_set(led_t leds, rgb_t color);
rgb_t board_led_forelight_get(led_t led);
rgb_t board_led_backlight_get(led_t led);
void board_led_display(led_t leds, rgb_t color);

#endif

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


