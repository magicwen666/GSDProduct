#ifndef __CONF_KEY_H__
#define __CONF_KEY_H__


#include "sys.h"
#include "board_key.h"
#include "board_led.h"
#include "scr_key.h"

typedef struct {
    key_type_t type;
    key_state_t state;
    okey_t okey;
    led_t leds;
} conf_key_t;

extern conf_key_t conf_key[6];

void ConfKeyInit(void);
void ConfKeyStateSwitch(conf_key_t *conf, key_t keys);


#endif

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


