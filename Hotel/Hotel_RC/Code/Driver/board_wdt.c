#include "chip.h"
#include "wdt.h"

// 看门狗初始化
void board_wdt_init(void)
{
    wdt_init();
}

// 喂狗
void board_wdt_feed(void)
{
    wdt_feed();
}
/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


