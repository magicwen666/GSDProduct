#ifndef __SYSCON_H__
#define __SYSCON_H__

#include "chip.h"

#define SYSCON_PPP_SSP0_RST_N   (1<<0)
#define SYSCON_PPP_I2C_RST_N    (1<<1)
#define SYSCON_PPP_SSP1_RST_N   (1<<2)
#define SYSCON_PPP_CAN_RST_N    (1<<3)

#define SYSCON_WDTCLK_IRC          (0x00)
#define SYSCON_WDTCLK_MAIN         (0x01)
#define SYSCON_WDTCLK_WDT          (0x02)

#define SYSCON_PD_WDTOSC    (1<<6)

typedef enum {
    SYSCON_CLK_SYS,         // 0
    SYSCON_CLK_ROM,         // 1
    SYSCON_CLK_RAM,         // 2
    SYSCON_CLK_FLASHREG,    // 3
    SYSCON_CLK_FLASHARRAY,  // 4
    SYSCON_CLK_I2C,         // 5
    SYSCON_CLK_GPIO,        // 6
    SYSCON_CLK_CT16B0,      // 7
    SYSCON_CLK_CT16B1,      // 8
    SYSCON_CLK_CT32B0,      // 9
    SYSCON_CLK_CT32B1,      // 10
    SYSCON_CLK_SSP0,        // 11
    SYSCON_CLK_UART0,       // 12
    SYSCON_CLK_ADC,         // 13
    SYSCON_CLK_NONE14,      // 14
    SYSCON_CLK_WDT,         // 15
    SYSCON_CLK_IOCON,       // 16
    SYSCON_CLK_CAN,         // 17
    SYSCON_CLK_SSP1,        // 18
    SYSCON_CLK_UART1,       // 19
    SYSCON_CLK_UART2,       // 20
}syscon_clk_t;


#define SYSCON_CLKOUT_CS_IRC    (0x00)
#define SYSCON_CLKOUT_CS_SYS    (0x01)
#define SYSCON_CLKOUT_CS_WDT    (0x02)
#define SYSCON_CLKOUT_CS_MAIN   (0x03)

void syscon_ppp_reset_dis(uint32_t ppp);
void syscon_ppp_reset_en(uint32_t ppp);
void syscon_ahb_div(uint32_t div);
void syscon_ahb_enable(syscon_clk_t clk);
void syscon_ahb_disable(syscon_clk_t clk);
void syscon_spi0_div(uint32_t div);
void syscon_uart0_div(uint32_t div);
void syscon_spi1_div(uint32_t div);

void syscon_wdt_osc(void);
void syscon_wdt_clk_update(uint8_t wdt_clk);
void syscon_wdt_div(uint32_t div);
void syscon_clkout_cs(uint8_t clkout_cs);
void syscon_clkout_cs_update(void);
void syscon_clkout_div(uint32_t div);
void syscon_pd_on(uint32_t pd);
void syscon_pd_off(uint32_t pd);
void syscon_restart(void);


#endif

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


