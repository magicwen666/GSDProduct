#ifndef __BOARD_KEY_H__
#define __BOARD_KEY_H__

// 头文件
#include "chip.h"
#include "board_time.h"

// 宏定义
#define KEY1    0x01
#define KEY2    0x02
#define KEY3    0x04
#define KEY4    0x08
#define KEY5    0x10
#define KEY6    0x20
#define KEYALL  0x3F

typedef uint8_t key_t;  // 原始按键电平状态

typedef enum
{
    //OKEY_STATE_FREE,
	OKEY_STATE_UP,
    OKEY_STATE_DOWN,
	OKEY_STATE_HOLD,
	OKEY_STATE_LHOLD,
	OKEY_STATE_CLICK,
	OKEY_STATE_LCLICK
}okey_state_t;

// 按键"类"
typedef struct
{
    key_t keys;
    okey_state_t okey_state;
    tick_t okey_time;
}okey_t;

// 函数
void board_key_init(void);
key_t board_key_scan(void);
uint8_t board_key_free_wait(key_t keys);
void handler_key_scan(void);
void okey_clear(okey_t *okey_p);
void okey_state_switch(okey_t *okey_p, key_t keys);
uint8_t is_okey_lhold(okey_t *okey_p, tick_t tim);
void okey_state_set(okey_t *okey_p, okey_state_t okey_state);
void okey_time_set(okey_t *okey_p, tick_t tim);
void okey_time_set(okey_t *okey_p, tick_t tim);
void okey_time_reset(okey_t *okey_p);

#endif

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


