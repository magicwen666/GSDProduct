#include "sys.h"
#include "board_led.h"
#include "board_wdt.h"
#include "et6290.h"

#define FLASH_TIME   600    // ÁÁÃðÒ»´ÎÊ±¼ä
#define ROTATE_TIME  400    // »»µÆÒ»´ÎÊ±¼ä
#define DISPLAY_FLAG_EN     1   // Ê¹ÄÜ
#define DISPLAY_FLAG_DIS    0   // ½ûÖ¹

volatile uint8_t _led_flash_flag = FALSE;
volatile uint8_t _led_rotate_flag = FALSE;
volatile led_t _flash_leds = 0;
volatile uint32_t _led_flash_tick = 0;
volatile uint32_t _led_rotate_tick = 0;

volatile rgb_t _led_back_color[6];
volatile rgb_t _led_fore_color[6];
volatile led_t _display_leds = 0;       //  ledÏÔÊ¾×´Ì¬

volatile uint8_t _display_flag = DISPLAY_FLAG_EN;  //ÏÔÊ¾Êä³öÊ¹ÄÜ

// ºÁÃë¶¨Ê±Æ÷×¢²á
void handler_flash_led(void)
{
    if(FALSE == _led_flash_flag){
        return ;
    }

    if(_led_flash_tick > FLASH_TIME){
        _led_flash_tick = 0;
    }else{
        _led_flash_tick++;
    }

    if(_led_flash_tick == 5){
        board_led_open(_flash_leds);
    }else if(_led_flash_tick == (FLASH_TIME/2+5)){
        board_led_close(_flash_leds);
    }
}

// ºÁÃë¶¨Ê±Æ÷×¢²á
void handler_rotate_led(void)
{
    if(FALSE == _led_rotate_flag){
        return ;
    }

    if(_led_rotate_tick > (ROTATE_TIME*6)){
        _led_rotate_tick = 0;
    }else{
        _led_rotate_tick++;
    }

    switch(_led_rotate_tick){
    case (ROTATE_TIME*0 +5):
        board_led_open(LED1);
        board_led_close(LED4);
        break;
    case (ROTATE_TIME*1 +5):
        board_led_open(LED2);
        board_led_close(LED1);
        break;
    case (ROTATE_TIME*2 +5):
        board_led_open(LED3);
        board_led_close(LED2);
        break;
    case (ROTATE_TIME*3 +5):
        board_led_open(LED6);
        board_led_close(LED3);
        break;
    case (ROTATE_TIME*4 +5):
        board_led_open(LED5);
        board_led_close(LED6);
        break;
    case (ROTATE_TIME*5 +5):
        board_led_open(LED4);
        board_led_close(LED5);
        break;
    default:
        break;
    }
}

void board_led_open(led_t leds)
{
    uint8_t i = 0;
    led_t led_tmp;

    _display_leds |= leds;

    for(i=0;i<6;i++){
        board_wdt_feed();   // Î¹¹·
        led_tmp = (1<<i);
        if(leds&led_tmp){
            board_led_display(led_tmp, _led_fore_color[i]);
        }
    }
}

void board_led_close(led_t leds)
{
    uint8_t i = 0;
    led_t led_tmp;

    _display_leds &= ~leds;

    for(i=0;i<6;i++){
        board_wdt_feed();   // Î¹¹·
        led_tmp = (1<<i);
        if(leds&led_tmp){
            board_led_display(led_tmp, _led_back_color[i]);
        }
    }
}

void board_led_refresh(void)
{
    uint8_t i = 0;
    led_t led_tmp;

    for(i=0;i<6;i++){
        board_wdt_feed();   // Î¹¹·
        led_tmp = (1<<i);
        if(_display_leds&led_tmp){
            board_led_display(led_tmp, _led_fore_color[i]);
        }else{
            board_led_display(led_tmp, _led_back_color[i]);
        }
    }
}

void board_led_display_en(void)
{
    // Êä³öÏÔÊ¾Ê¹ÄÜ
    _display_flag = DISPLAY_FLAG_EN;
}

void board_led_display_dis(void)
{
    // Êä³öÏÔÊ¾½ûÖ¹
    _display_flag = DISPLAY_FLAG_DIS;
}

void board_led_flash_start(void)
{
    _led_flash_flag = TRUE;
    _flash_leds = LEDALL;
    _led_flash_tick = 0;
    board_led_close(LEDALL);
}

void board_led_flash_set(led_t leds)
{
    _flash_leds = leds;
}

void board_led_flash_stop(void)
{
    _led_flash_flag = FALSE;
    _flash_leds = 0;
    _led_flash_tick = 0;
    board_led_close(LEDALL);
}

void board_led_flash_add(led_t leds)
{
    _flash_leds |= leds;
}

void board_led_flash_del(led_t leds)
{
    _flash_leds &= ~leds;
}

void board_led_rotate_start(void)
{
    _led_rotate_flag = TRUE;
    _led_rotate_tick = 0;

    board_led_close(LEDALL);
}

void board_led_rotate_stop(void)
{
    _led_rotate_flag = FALSE;
    _led_rotate_tick = 0;

    board_led_close(LEDALL);
}

void board_led_backlight_set(led_t leds, rgb_t color)
{
    uint8_t i = 0;
    led_t led_tmp;
    for(i=0;i<6;i++){
        board_wdt_feed();   // Î¹¹·
        led_tmp = (1<<i);
        if(leds&led_tmp){
            _led_back_color[i] = color;
        }
    }
}

rgb_t board_led_backlight_get(led_t led)
{
    uint8_t i = 0;
    led_t led_tmp;
    rgb_t color_tmp;

    for(i=0;i<6;i++){
        board_wdt_feed();   // Î¹¹·
        led_tmp = (1<<i);
        if(led&led_tmp){
            color_tmp = _led_back_color[i];
        }
    }
    return color_tmp;
}

void board_led_forelight_set(led_t leds, rgb_t color)
{
    uint8_t i = 0;
    led_t led_tmp;
    for(i=0;i<6;i++){
        board_wdt_feed();   // Î¹¹·
        led_tmp = (1<<i);
        if(leds&led_tmp){
            _led_fore_color[i] = color;
        }
    }
}

rgb_t board_led_forelight_get(led_t led)
{
    uint8_t i = 0;
    led_t led_tmp;
    rgb_t color_tmp;
    for(i=0;i<6;i++){
        board_wdt_feed();   // Î¹¹·
        led_tmp = (1<<i);
        if(led&led_tmp){
            color_tmp = _led_fore_color[i];
        }
    }
    return color_tmp;
}

void board_led_display(led_t leds, rgb_t color)
{
    if(_display_flag == DISPLAY_FLAG_DIS){
        return; // ½ûÖ¹Êä³ö
    }

    if(leds&LED1){
        et6290_send_cmd(ET6290A, MODE_ON|MODE3, color.r_val, color.g_val, color.b_val);
    }
    if(leds&LED2){
        et6290_send_cmd(ET6290A, MODE_ON|MODE2, color.r_val, color.g_val, color.b_val);
    }
    if(leds&LED3){
        et6290_send_cmd(ET6290A, MODE_ON|MODE1, color.r_val, color.g_val, color.b_val);
    }
    if(leds&LED4){
        et6290_send_cmd(ET6290B, MODE_ON|MODE1, color.r_val, color.g_val, color.b_val);
    }
    if(leds&LED5){
        et6290_send_cmd(ET6290B, MODE_ON|MODE2, color.r_val, color.g_val, color.b_val);
    }
    if(leds&LED6){
        et6290_send_cmd(ET6290B, MODE_ON|MODE3, color.r_val, color.g_val, color.b_val);
    }
}

void board_led_init(void)
{
    et6290_init();
    board_led_display_en(); // Ê¹ÄÜÊä³ö
}

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/



