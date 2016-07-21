#include "conf_key.h"
#include "board_key.h"

conf_key_t conf_key[6]; 

void ConfKeyInit(void) {
    
    unsigned char i = 0;

    for (i = 0; i < 6; i++) {
        conf_key[i].type = KEY_TYPE_CONF;
        conf_key[i].state = KEY_STATE_CLOSE;
        conf_key[i].okey.keys = (1<<i);
        conf_key[i].leds = (1<<i);
    }
}

void ConfKeyStateSwitch(conf_key_t *conf, key_t keys) {

    okey_state_switch(&conf->okey, keys);
}

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/

