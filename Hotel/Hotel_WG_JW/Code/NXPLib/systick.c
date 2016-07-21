#include "chip.h"

// 引用的外部变量
extern uint32_t SystemCoreClock;

void systick_init(uint32_t hz)
{
    SysTick_Config(SystemCoreClock/hz);   //  HZ
}

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


