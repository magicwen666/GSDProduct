#ifndef __TIMER_H__
#define __TIMER_H__

#include "chip.h"


// IR
#define IR_MR0_FLAG    (1<<0)
#define IR_MR1_FLAG    (1<<1)
#define IR_MR2_FLAG    (1<<2)
#define IR_MR3_FLAG    (1<<3)
#define IR_CR0_FLAG    (1<<4)

// TCR

// MCR
#define MCR_MR0I        (1<<0)  // 中断
#define MCR_MR0R        (1<<1)  // 复位
#define MCR_MR0S        (1<<2)  // 停止
#define MCR_MR1I        (1<<3)  // 中断
#define MCR_MR1R        (1<<4)  // 复位
#define MCR_MR1S        (1<<5)  // 停止
#define MCR_MR2I        (1<<6)  // 中断
#define MCR_MR2R        (1<<7)  // 复位
#define MCR_MR2S        (1<<8)  // 停止
#define MCR_MR3I        (1<<9)  // 中断
#define MCR_MR3R        (1<<10)  // 复位
#define MCR_MR3S        (1<<11)  // 停止

// CCR
#define CCR_CAP0RE      (1<<0)  // 上升沿捕获使能
#define CCR_CAP0FE      (1<<1)  // 下降沿
#define CCR_CAP0I       (1<<2)  // 中断使能

// EMR
#define EMR_EM0         (1<<0)
#define EMR_EM1         (1<<1)
#define EMR_EM2         (1<<2)
#define EMR_EM3         (1<<3)
#define EMR_EMC0_NONE   (0<<4)  // 不操作
#define EMR_EMC0_LOW    (1<<4)  // 输出0
#define EMR_EMC0_HIG    (2<<4)  // 输出1
#define EMR_EMC0_REV    (3<<4)  // 输出翻转
#define EMR_EMC1_NONE   (0<<6)  // 不操作
#define EMR_EMC1_LOW    (1<<6)  // 输出0
#define EMR_EMC1_HIG    (2<<6)  // 输出1
#define EMR_EMC1_REV    (3<<6)  // 输出翻转
#define EMR_EMC2_NONE   (0<<8)  // 不操作
#define EMR_EMC2_LOW    (1<<8)  // 输出0
#define EMR_EMC2_HIG    (2<<8)  // 输出1
#define EMR_EMC2_REV    (3<<8)  // 输出翻转
#define EMR_EMC3_NONE   (0<<10)  // 不操作
#define EMR_EMC3_LOW    (1<<10)  // 输出0
#define EMR_EMC3_HIG    (2<<10)  // 输出1
#define EMR_EMC3_REV    (3<<10)  // 输出翻转

// CTCR
#define CTCR_MODE_TIM     (0<0)   // 定时器模式
#define CTCR_MODE_RC      (1<0)   // 上升沿计数模式
#define CTCR_MODE_FC      (2<0)   // 下降沿计数模式
#define CTCR_MODE_BC      (3<0)   // 双边沿计数模式

// PWMC
#define PWMC_MAT0_EN    (1<<0)  // MAT0 PWM使能
#define PWMC_MAT1_EN    (1<<1)  // MAT1 PWM使能
#define PWMC_MAT2_EN    (1<<2)  // MAT2 PWM使能
#define PWMC_MAT3_EN    (1<<3)  // MAT3 PWM使能

void ct16b0_enable(void);
void ct16b0_disable(void);
void ct16b0_timer_init(uint32_t hz);
void ct16b0_pwn_init(void);
void ct16b0_set_match(uint32_t chanel, uint32_t match_val);
void ct16b1_timer_init(uint32_t hz);
void ct16b1_pwn_init(void);
void ct16b1_set_match(uint32_t chanel, uint32_t match_val);
void ct32b0_pwm_init(void);
void ct32b0_set_match(uint32_t chanel, uint32_t match_val);
void ct32b0_timer_init(uint32_t hz);

#endif

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


