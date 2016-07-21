#ifndef __CHIP_H__
#define __CHIP_H__

//#define CHIP_LPC11XX
#define CHIP_LPC112X

#ifdef CHIP_LPC11XX
#include "LPC11xx.h"
#include "system_LPC11xx.h"
#include "core_cm0.h"
#endif

#ifdef CHIP_LPC112X
#include "LPC112x.h"
#include "system_LPC112x.h"
#include "core_cm0.h"
#endif

#include <stdint.h>
#include <stdio.h>
#include "gpio.h"
#include "i2c.h"
#include "iocon.h"
#include "syscon.h"
#include "systick.h"
#include "timer.h"
#include "wdt.h"
#include "uart.h"
//#include "adc.h"

#define ERR     0
#define SUCC    1
#define FAIL    2

#define LOGInit()    uart0_init(115200)
#define SYSLOG(...)  printf(__VA_ARGS__)

//#define DEBUG
#ifdef  DEBUG
#define LOG(...)     printf(__VA_ARGS__)
#else
#define LOG(...)
#endif

#define DOG_INIT()  wdt_init()
#define DOG_FEED()  wdt_feed()

#endif

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/



