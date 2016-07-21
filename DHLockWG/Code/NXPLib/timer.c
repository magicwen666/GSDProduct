#include "chip.h"


// 引用的外部变量
extern uint32_t SystemCoreClock;

// ct16b0 LED1 LED2 LED3
// ct32b0 LED4 LED5 LED6
// ct16b1 RL2_PWM RL3_PWM
// ct32b1 us timer
// systick ms timer
// wdt ort wwdt none
/*
typedef struct
{
  __IO uint32_t IR;                     // Offset: 0x000 Interrupt Register (R/W)
  __IO uint32_t TCR;                    // Offset: 0x004 Timer Control Register (R/W)
  __IO uint32_t TC;                     // Offset: 0x008 Timer Counter Register (R/W)
  __IO uint32_t PR;                     // Offset: 0x00C Prescale Register (R/W)
  __IO uint32_t PC;                     // Offset: 0x010 Prescale Counter Register (R/W)
  __IO uint32_t MCR;                    // Offset: 0x014 Match Control Register (R/W)
  __IO uint32_t MR0;                    // Offset: 0x018 Match Register 0 (R/W)
  __IO uint32_t MR1;                    // Offset: 0x01C Match Register 1 (R/W)
  __IO uint32_t MR2;                    // Offset: 0x020 Match Register 2 (R/W)
  __IO uint32_t MR3;                    // Offset: 0x024 Match Register 3 (R/W)
  __IO uint32_t CCR;                    // Offset: 0x028 Capture Control Register (R/W)
  __I  uint32_t CR0;                    // Offset: 0x02C Capture Register 0 (R/ )
  __I  uint32_t CR1;                    // Offset: 0x030 Capture Register 1 (R/ )
       uint32_t RESERVED1[2];
  __IO uint32_t EMR;                    // Offset: 0x03C External Match Register (R/W)
       uint32_t RESERVED2[12];
  __IO uint32_t CTCR;                   // Offset: 0x070 Count Control Register (R/W)
  __IO uint32_t PWMC;                   // Offset: 0x074 PWM Control Register (R/W)
} LPC_TMR_TypeDef;
*/

/*
void CT16B0_IRQHandler(void)
{
    if ( LPC_CT16B0->IR & IR_MR0_FLAG){
        LPC_CT16B0->IR = IR_MR0_FLAG;			// clear interrupt flag
    }
    return;
}
*/
/*
void CT16B1_IRQHandler(void)
{
    if ( LPC_CT16B0->IR & IR_MR0_FLAG ){
        LPC_CT16B0->IR = IR_MR0_FLAG;			// clear interrupt flag
    }
    return;
}
*/
/*
void CT32B0_IRQHandler(void)
{

}
*/

/*
void CT32B1_IRQHandler(void)
{

}
*/

void ct16b0_enable(void)
{
    LPC_CT16B0->TCR = (1<<0);
}

void ct16b0_disable(void)
{
    LPC_CT16B0->TCR = (0<<0);
}

void ct16b0_reset(void)
{
    LPC_CT16B0->TCR = (1<<1);
}

void ct16b0_timer_init(uint32_t hz)
{
    syscon_ahb_enable(SYSCON_CLK_CT16B0);

    LPC_CT16B0->PR = 99;        // Fpc = PCLK/(PR+1)
    LPC_CT16B0->PC = 0;
    LPC_CT16B0->TC = 0;        // Ftc = Fpc/MR = PCLK/((PR+1)*MR)

    LPC_CT16B0->MR[0] = SystemCoreClock/((LPC_CT16B0->PR+1)*hz);   // T = 1/Ftc = MR*(PR+1)/PCLK
    LPC_CT16B0->MCR = (MCR_MR0I|MCR_MR0R); // 匹配中断和复归

    LPC_CT16B0->CTCR = CTCR_MODE_TIM;  // 定时模式

    ct16b0_enable();
    NVIC_EnableIRQ(CT16B0_IRQn);
    NVIC_SetPriority(CT16B0_IRQn, 0);
}

void ct16b0_pwn_init(void)
{
    syscon_ahb_enable(SYSCON_CLK_CT16B0);

    ct16b0_reset();

    LPC_CT16B0->PR = 0;                         // Fpc = PCLK/(PR+1)

    LPC_CT16B0->PWMC = PWMC_MAT0_EN;
    LPC_CT16B0->MCR = (MCR_MR3R);                  // 设置MR3匹配后复位TC,PWM周期

    LPC_CT16B0->MR[3] = SystemCoreClock/ 38000;            // 控制周期 38k载波
    LPC_CT16B0->MR[0] = LPC_CT16B0->MR[3]*0/100;           //

    ct16b0_enable();
}

void ct16b0_set_match(uint32_t chanel, uint32_t match_val)
{
    switch(chanel){
    case 3:
        LPC_CT16B0->MR[2] = LPC_CT16B0->MR[3]*match_val/100;     // LED1
        break;
    case 2:
        LPC_CT16B0->MR[1] = LPC_CT16B0->MR[3]*match_val/100;     // LED2
        break;
    case 1:
        LPC_CT16B0->MR[0] = LPC_CT16B0->MR[3]*match_val/100;     // LED3
        break;
    default:
        break;
    }
}

void ct16b1_enable(void)
{
    LPC_CT16B1->TCR = (1<<0);
}

void ct16b1_disable(void)
{
    LPC_CT16B1->TCR = (0<<0);
}

void ct16b1_reset(void)
{
    LPC_CT16B1->TCR = (1<<1);
}

void ct16b1_timer_init(uint32_t hz)
{
    syscon_ahb_enable(SYSCON_CLK_CT16B1);

    LPC_CT16B1->PR = 99;        // Fpc = PCLK/(PR+1)
    LPC_CT16B1->PC = 0;
    LPC_CT16B1->TC = 0;        // Ftc = Fpc/MR = PCLK/((PR+1)*MR)

    LPC_CT16B1->MR[0] = SystemCoreClock/((LPC_CT16B1->PR+1)*hz);   // T = 1/Ftc = MR*(PR+1)/PCLK
    LPC_CT16B1->MCR = (MCR_MR0I|MCR_MR0R); // 匹配中断和复归

    LPC_CT16B1->CTCR = CTCR_MODE_TIM;  // 定时模式

    ct16b1_enable();
    NVIC_EnableIRQ(CT16B1_IRQn);
    NVIC_SetPriority(CT16B1_IRQn, 0);
}

void ct16b1_pwm_init(void)
{
    syscon_ahb_enable(SYSCON_CLK_CT16B1);

    ct16b1_reset();
    ct16b1_enable();
}

void ct32b0_enable(void)
{
    LPC_CT32B0->TCR = (1<<0);
}

void ct32b0_disable(void)
{
    LPC_CT32B0->TCR = (0<<0);
}

void ct32b0_reset(void)
{
    LPC_CT32B0->TCR = (1<<1);
}

void ct32b0_timer_init(uint32_t hz)
{
    syscon_ahb_enable(SYSCON_CLK_CT32B0);

    LPC_CT32B0->PR = 99;        // Fpc = PCLK/(PR+1)
    LPC_CT32B0->PC = 0;
    LPC_CT32B0->TC = 0;        // Ftc = Fpc/MR = PCLK/((PR+1)*MR)

    LPC_CT32B0->MR[0] = SystemCoreClock/((LPC_CT32B0->PR+1)*hz);   // T = 1/Ftc = MR*(PR+1)/PCLK
    LPC_CT32B0->MCR = (MCR_MR0I|MCR_MR0R); // 匹配中断和复归

    LPC_CT32B0->CTCR = CTCR_MODE_TIM;  // 定时模式

    ct32b0_enable();
    NVIC_EnableIRQ(CT32B0_IRQn);
}

void ct32b0_pwm_init(void)
{
    syscon_ahb_enable(SYSCON_CLK_CT32B0);               // 打开定时器模块

    ct32b0_reset();                                     // 定时器复位

    LPC_CT32B0->PR      = 1000;                        // 设置分频系数
    LPC_CT32B0->PWMC = PWMC_MAT1_EN|                   // 设置MAT1，2,3,PWM输出
                        PWMC_MAT2_EN|
                        PWMC_MAT3_EN;
    LPC_CT32B0->MCR = MCR_MR0R;                        // 设置MR0匹配后复位TC,PWM周期

    LPC_CT32B0->MR[0]     = SystemCoreClock / 100000;        // 周期控制，100kHZ
    LPC_CT32B0->MR[1]     = LPC_CT32B0->MR[0]*50/100;     // LED4
    LPC_CT32B0->MR[2]     = LPC_CT32B0->MR[0]*50/100;     // LED5
    LPC_CT32B0->MR[3]     = LPC_CT32B0->MR[0]*50/100;     // LED6

    ct32b0_enable();
}

void ct32b0_set_match(uint32_t chanel, uint32_t match_val)
{
    switch(chanel){
    case 0:
        LPC_CT32B0->MR[1]     = LPC_CT32B0->MR[0]*match_val/100;     // LED4
        break;
    case 1:
        LPC_CT32B0->MR[2]    = LPC_CT32B0->MR[0]*match_val/100;     // LED5
        break;
    case 2:
        LPC_CT32B0->MR[3]    = LPC_CT32B0->MR[0]*match_val/100;     // LED6
        break;
    default:
        break;
    }
}

void ct32b1_enable(void)
{
    LPC_CT32B1->TCR = (1<<0);
}

void ct32b1_disable(void)
{
    LPC_CT32B1->TCR = (0<<0);
}

void ct32b1_reset(void)
{
    LPC_CT32B1->TCR = (1<<1);
}

void ct32b1_timer_init(void)
{
    syscon_ahb_enable(SYSCON_CLK_CT32B1);
    ct32b1_reset();
    ct32b1_enable();

    NVIC_EnableIRQ(CT32B1_IRQn);
}

void ct32b1_pwm_init(void)
{
    syscon_ahb_enable(SYSCON_CLK_CT32B1);
    ct32b1_reset();
    ct32b1_enable();
}



/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/



