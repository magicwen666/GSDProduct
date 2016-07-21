#ifndef __SYS_TIMER_H__
#define __SYS_TIMER_H__

void SysTimerInit(void);
unsigned int SysTimerGet(void);
unsigned int SysTimerCalc(unsigned int pre, unsigned int now);
unsigned int SysTimerGetUs(void);

#endif


