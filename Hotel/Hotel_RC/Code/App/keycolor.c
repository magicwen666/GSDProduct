#include "keycolor.h"
#include "sys.h"
#include "board_led.h"
#include "scr_key.h"

/*
{60,0,0}, ºì
 {0,60,0}, ÂÌ
 {0,0,60}, À¶
 {30,30,0},»Æ
 {30,0,30},×Ï
 {0,30,30},Ç³À¶
 {20,20,20}»Ò
*/

#if 0

rgb_t fore_color[MAX_COLOR_SIZE] = 
     {{60,0,0},{0,60,0},{0,0,60},{30,30,0},{30,0,30},{0,30,30},{20,20,20}};
rgb_t back_color[4][MAX_COLOR_SIZE] =  {
     {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},{0,0,0},{0,0,0},{0,0,0}},
     {{6,0,0}, {0,6,0}, {0,0,6}, {3,3,0},{3,0,3},{0,3,3},{2,2,2}},
     {{12,0,0},{0,12,0},{0,0,12},{6,6,0},{6,0,6},{0,6,6},{4,4,4}},
     {{18,0,0},{0,18,0},{0,0,18},{9,9,0},{9,0,9},{0,9,9},{6,6,6}}};
#else

rgb_t fore_color[MAX_COLOR_SIZE] = {{0, 0, 100}, {100, 100, 100}, {150, 50, 0}};
rgb_t back_color[MAX_COLOR_SIZE] = {{0, 0, 40}, {40, 40, 40}, {40, 20, 0}};

#endif


void KeyColorInit(unsigned char index) {
    
    unsigned char i = 0;
    rgb_t tmp;
    scr_key_t *p = NULL;

    if (index >= MAX_COLOR_SIZE) {
        return ;
    }
    
    for (i=0; i < MAX_SCR_KEY_SIZE; i++) {
        p  = &g_scr_key[i];
        tmp = fore_color[index];
        board_led_forelight_set(p->leds, tmp);
        tmp = back_color[index];
        board_led_backlight_set(p->leds, tmp);
    }
}


/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


