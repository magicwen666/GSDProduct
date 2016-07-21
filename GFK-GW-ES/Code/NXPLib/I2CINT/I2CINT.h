/****************************************Copyright (c)****************************************************
**                            Guangzhou ZHIYUAN electronics Co.,LTD.
**
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  I2CINT.h
** Latest modified Date:       2009-6-8
** Latest Version:
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chenmingji
** Created date:               2009-6-8
** Version:
** Descriptions:               LPC1100系列芯片硬件I2C软件包。主程序要配置好I2C总线接口(I2C引脚功能和
**                             I2C中断，并已使能I2C主模式)
**--------------------------------------------------------------------------------------------------------
** Modified by:                Zhangrong
** Modified date:              2009-6-8
** Version:
** Descriptions:
**
*********************************************************************************************************/

#ifndef I2CINT_H
#define I2CINT_H

#include "chip.h"

#define ONE_BYTE_SUBA   1
#define TWO_BYTE_SUBA   2
#define X_ADD_8_SUBA    3

//#define INT8U    unsigned char
//#define INT16U   unsigned short
//#define INT32U   unsigned int

#define INT8U    uint8_t
#define INT16U   uint16_t
#define INT32U   uint32_t

#define TRUE     1
#define FALSE    0


extern INT8U IRcvByte(INT8U sla, INT8U *dat);
extern INT8U ISendByte(INT8U sla, INT8U dat);
extern INT8U I2C_ReadNByte (INT8U sla, INT8U suba_type, INT32U suba, INT8U *s, INT32U num);
extern INT8U I2C_WriteNByte(INT8U sla, INT8U suba_type, INT32U suba, INT8U *s, INT32U num);

#endif

/*********************************************************************************************************
** End Of File
*********************************************************************************************************/
