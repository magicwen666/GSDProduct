#include "chip.h"


/*
typedef struct
{
    __IO uint32_t MOD;      // < Offset: 0x000 Watchdog mode register (R/W)
    __IO uint32_t TC;       // < Offset: 0x004 Watchdog timer constant register (R/W)
    __O  uint32_t FEED;     // < Offset: 0x008 Watchdog feed sequence register (W)
    __I  uint32_t TV;       // < Offset: 0x00C Watchdog timer value register (R)
    uint32_t RESERVED0;
    __IO uint32_t WARNINT;	// < Offset: 0x014 Watchdog timer warning int. register (R/W)
    __IO uint32_t WINDOW;	// < Offset: 0x018 Watchdog timer window value register (R/W)
} LPC_WDT_TypeDef;
*/



void wdt_feed(void)
{
    __disable_irq();
    LPC_WWDT->FEED = 0xAA;
    LPC_WWDT->FEED = 0x55;
    __enable_irq();
}

void wdt_init(void)
{
    syscon_wdt_osc();   // 配置时钟
    syscon_pd_on(SYSCON_PD_WDTOSC); // 时钟上电
    syscon_wdt_clk_update(SYSCON_WDTCLK_WDT);    // 更新使能时钟源
    syscon_wdt_div(0x01);

    //LPC_SYSCON->SYSAHBCLKCTRL |= (1<<15);
    syscon_ahb_enable(SYSCON_CLK_WDT);


    LPC_WWDT->MOD = WDT_MOD_WDEN|WDT_MOD_WDRESET;
    LPC_WWDT->TC = 0xFFFFFF;
    wdt_feed();
}

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


