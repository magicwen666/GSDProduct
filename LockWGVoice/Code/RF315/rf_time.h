#ifndef __RF_TIME_H__
#define __RF_TIME_H__

#include "chip.h"

void RF_TimerOn(uint16_t u16Time);
void RF_TimerOff(void);
uint16_t RF_GetTimerCntValue(void);
uint8_t RF_GetTimerFlag(void);
void RF_ClrTimerFlag(void);


#endif


