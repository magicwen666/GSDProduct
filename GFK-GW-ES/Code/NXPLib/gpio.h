#ifndef __GPIO_H__
#define __GPIO_H__

#include "chip.h"

#define GPIO_IN     (0x00)
#define GPIO_OUT    (0x01)

#define GPIO_H      (0x01)
#define GPIO_L      (0x00)

#define GPIO_INTMODE_RISE_EDGE  0x01    // 上升沿
#define GPIO_INTMODE_DROP_EDGE  0x02    // 下降沿
#define GPIO_INTMODE_DB_EDGE    0x03    // 双边沿
#define GPIO_INTMODE_HIG_LEVEL  0x04    // 高电平
#define GPIO_INTMODE_LOW_LEVEL  0x05    // 低电平

#define IS_EDGE         0x00
#define IS_LEVEL        0x01
#define IBE_NONE        0x00
#define IBE_DB_EDGE     0x01
#define IEV_DROP_LEVEL  0x00
#define IEV_RISE_HIG    0x01

void gpio_set_dir(LPC_GPIO0_Type *LPC_GPIO, uint32_t bit, uint32_t dir);
void gpio_set_bitval(LPC_GPIO0_Type *LPC_GPIO, uint32_t bit, uint32_t val);
uint32_t gpio_get_bitval(LPC_GPIO0_Type *LPC_GPIO, uint32_t bit);
void gpio_set_interrupt(LPC_GPIO0_Type *LPC_GPIO, uint32_t bit, uint32_t val);
void gpio_enable_interrupt(LPC_GPIO0_Type *LPC_GPIO, uint32_t bit);
void gpio_clear_interrupt(LPC_GPIO0_Type *LPC_GPIO, uint32_t bit);
uint8_t gpio_get_intstatus(LPC_GPIO0_Type *LPC_GPIO, uint32_t bit);

#endif

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


