#ifndef __WDT_H__
#define __WDt_H__

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

// MOD
#define WDT_MOD_WDEN        (1<<0)
#define WDT_MOD_WDRESET     (1<<1)
#define WDT_MOD_WDTOF       (1<<2)
#define WDT_MOD_INT         (1<<3)
#define WDT_MOD_WDPROTECT   (1<<4)

void wdt_init(void);
void wdt_feed(void);


#endif

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/



