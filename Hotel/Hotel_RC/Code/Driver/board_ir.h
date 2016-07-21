#ifndef __BOARD_IR_H__
#define __BOARD_IR_H__

// 头文件
#include "chip.h"

// 宏定义
// 红外编码值定义
#define IR_ID   0xFE

#define IR_SET  0xE817FE01
#define IR_ON   0xF30CFE01
#define IR_OFF  0xF708FE01
#define IR_A    0xF00FFE01
#define IR_B    0xF50AFE01
#define IR_C    0xE916FE01
#define IR_D    0xF807FE01
#define IR_E    0xF609FE01
#define IR_F    0xF906FE01
#define IR_L1   0xEF10FE01
#define IR_L2   0xFF00FE01
#define IR_L3   0xFC03FE01
#define IR_L4   0xFB04FE01
#define IR_D1U  0xF40BFE01
#define IR_D1D  0xFE01FE01
#define IR_D2U  0xEB14FE01
#define IR_D2D  0xFD02FE01
#define IR_DEL  0xFA05FE01

typedef uint32_t ir_t;

// 红外接近感应
typedef enum
{
    IRPS_STATE_OFF,
    IRPS_STATE_ON
}irps_state_t;

// 函数
void handler_ir_tick(void);
void handler_ir(void);
void board_ir_init(void);
ir_t board_ir_get(void);
void board_ir_clean(void);

void board_irps_init(void);
void board_irps_start(void);
void board_irps_stop(void);
void handler_irps_send(void);
irps_state_t board_irps_get(void);
void board_irps_clean(void);

#endif

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


