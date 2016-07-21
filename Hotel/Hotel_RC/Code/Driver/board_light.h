#ifndef __BOARD_LIGHT_H__
#define __BOARD_LIGHT_H__

// 头文件
#include "sys.h"
#include "board_time.h"
#include <stdint.h>
// 宏定义
#define LIGHT1 0x01
#define LIGHT2 0x02
#define LIGHT3 0x04
#define LIGHT4 0x08
#define LIGHT5 0x10
#define LIGHT6 0x20

#define LIGHT_STATE_OPEN  0x01
#define LIGHT_STATE_CLOSE 0x02

typedef struct
{
    uint8_t light_name;
    uint8_t light_state;
    tick_t delay_us;
}light_t;

// 函数
void handler_light(void);
void board_light_init(void);
light_t * board_light_register(uint8_t light_name);
void board_light_open(light_t *light_p);
void board_light_close(light_t *light_p);
void board_rl_delay_open(light_t *light_p, uint32_t delay_us);
void board_rl_delay_open(light_t *light_p, uint32_t delay_us);

#endif

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/



