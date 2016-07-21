#ifndef __ET6290_H__
#define __ET6290_H__

#include "chip.h"

// 头文件

// 宏定义
#define ET6290A     0x01
#define ET6290B     0x02

#define MODE_OFF	0x00	/* 软shutdown */
#define MODE_ON		0x80	/* normal */
#define MODE1		0x00	/* 模式1 */
#define MODE2		0x20	/* 模式2 */
#define MODE3		0x40	/* 模式3 */
#define MODE4		0x60	/* 模式4 */

// 函数
void et6290_init(void);
void et6290_send_cmd(   const uint8_t chip,
                        const uint8_t conf, 
                        const uint8_t rpwm,
                    	const uint8_t gpwm, 
                    	const uint8_t bpwm);

#endif

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


