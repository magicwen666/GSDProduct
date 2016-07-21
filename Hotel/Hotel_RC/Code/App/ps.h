#ifndef __PS_H__
#define __PS_H__

#include "board_time.h"

#define PS_BL_TICK     20000

typedef enum {
    PS_BL_STATE_OFF,
    PS_BL_STATE_ON,
} ps_bl_state_t;


typedef struct {
    ps_bl_state_t bl_sta;
    tick_t delay_tick;
} ps_t;

extern ps_t g_ps;

void PsInit(void);
void PsReset(void);
void PsHandler(void);
void PsOn(void);
void PsOff(void);
void PsSleep(void);
void PsWake(void);

#endif


