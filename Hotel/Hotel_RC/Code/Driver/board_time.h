#ifndef __BOARD_TIME_H__
#define __BOARD_TIME_H__

// 头文件
#include "sys.h"
#include <stdint.h>
// 宏定义

#define MAX_TIME    0xFFFFFFFF
typedef uint32_t tick_t;

// 函数
void handler_systick(void);
void board_time_init(void);
tick_t board_time_get(void);
void board_time_delay1ms(tick_t tim);
void board_time_out_set(tick_t tim);
tick_t board_time_out_wait(void);
tick_t board_time_calc(tick_t tim_pre, tick_t tim_now);

#endif

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


