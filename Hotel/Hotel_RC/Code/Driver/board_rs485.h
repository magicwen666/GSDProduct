#ifndef __BOARD_RS485_H__
#define __BOARD_RS485_H__


#include "sys.h"

void board_rs485_init(void);
void board_rs485_write_nbyte(void *data, uint16_t len);
uint8_t board_rs485_read(void *data, uint16_t *len);
uint8_t is_board_rs485_idle(void);
void handler_rs485_idle(void);
void handler_rs485(void);


#endif

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


