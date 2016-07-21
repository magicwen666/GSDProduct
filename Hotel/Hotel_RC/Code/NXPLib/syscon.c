#include "chip.h"


void syscon_ppp_reset_dis(uint32_t ppp)
{
	LPC_SYSCON->PRESETCTRL  |= ppp;
}

void syscon_ppp_reset_en(uint32_t ppp)
{
	LPC_SYSCON->PRESETCTRL  &= ~ppp;
}

void syscon_ahb_div(uint32_t div)
{
	LPC_SYSCON->SYSAHBCLKDIV  = div;
}

void syscon_ahb_enable(syscon_clk_t clk)
{
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << clk);
}
/*
void syscon_ahb_disable(syscon_clk_t clk)
{
	LPC_SYSCON->SYSAHBCLKCTRL &= ~(1 << clk);
}
*/

void syscon_spi0_div(uint32_t div)
{
	LPC_SYSCON->SSP0CLKDIV  = div;
}

void syscon_uart0_div(uint32_t div)
{
	LPC_SYSCON->UART0CLKDIV  = div;
}

void syscon_spi1_div(uint32_t div)
{
	LPC_SYSCON->SSP1CLKDIV  = div;
}

void syscon_wdt_osc(void)
{
   // wdt_osc_clk = Fclkana / (2 ¡Á (1 + DIVSEL))
   #define WDT_OSC_DIVSEL   1
   #define WDT_OSC12        12  // 4.0Mhz
   LPC_SYSCON->WDTOSCCTRL = (WDT_OSC_DIVSEL<<0)|(WDT_OSC12<<5); // wdt_osc_clk = 1.0Mhz
}

void syscon_wdt_clk_update(uint8_t wdt_clk)
{

    LPC_SYSCON->WDTCLKSEL = wdt_clk;
    LPC_SYSCON->WDTCLKUEN = 0x01;
    LPC_SYSCON->WDTCLKUEN = 0x00;
    LPC_SYSCON->WDTCLKUEN = 0x01;
}

void syscon_wdt_div(uint32_t div)
{
	LPC_SYSCON->WDTCLKDIV = div;
}

void syscon_clkout_cs(uint8_t clkout_cs)
{
    LPC_SYSCON->CLKOUTCLKSEL = clkout_cs;
}

void syscon_clkout_cs_update(void)
{
    LPC_SYSCON->CLKOUTUEN = 0x01;
    LPC_SYSCON->CLKOUTUEN = 0x00;
    LPC_SYSCON->CLKOUTUEN = 0x01;
}

void syscon_clkout_div(uint32_t div)
{
	LPC_SYSCON->CLKOUTCLKDIV = div;
}

void syscon_pd_on(uint32_t pd)
{
    LPC_SYSCON->PDRUNCFG &= ~pd;
}

void syscon_pd_off(uint32_t pd)
{
    LPC_SYSCON->PDRUNCFG |= pd;
}

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


