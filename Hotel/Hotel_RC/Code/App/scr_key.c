#include "scr_key.h"
#include "sys.h"
#include "board_key.h"
#include "network.h"
#include "board_flash.h"
#include <string.h>

scr_key_t g_scr_key[MAX_SCR_KEY_SIZE];
unsigned int WaitDelayTick;

void WaitDelayHandler(void) {

    if (0 != WaitDelayTick) {
        WaitDelayTick--;
    }
}

void ScrKeyClear(void) {

    unsigned char i = 0;
    scr_key_t *p = NULL;
    
    for (i = 0; i < MAX_SCR_KEY_SIZE; i++) {
        p = &g_scr_key[i];
        p->type = KEY_TYPE_NONE;
        p->state = KEY_STATE_NONE;
        p->light_p = NULL;
        p->keyval = 0x00;
        okey_clear(&p->okey);
        memset(&p->assoc_key, 0x00, sizeof(main_key_t));
    }
    
}

unsigned char ScrKeyInit(sys_info_t info) {

    unsigned char keynum = info.scr_num.wait + info.scr_num.other;
    unsigned char index = 0;
    unsigned char i = 0;

    if ((0 == keynum) || (keynum > 4)) {
        return 1;
    }

    ScrKeyClear();
    switch (keynum) {
        case 1 : {
            g_scr_key[0].okey.keys = KEY1|KEY2|KEY3|KEY4|KEY5|KEY6;
            g_scr_key[0].leds = LED1|LED2|LED3|LED4|LED5|LED6;
        } break;
        case 2 : {
            g_scr_key[0].okey.keys = KEY1|KEY2|KEY3;
            g_scr_key[0].leds = LED1|LED2|LED3;
            g_scr_key[1].okey.keys = KEY4|KEY5|KEY6;
            g_scr_key[1].leds = LED4|LED5|LED6;
        } break;
        case 3 : {
            g_scr_key[0].okey.keys = KEY1|KEY4;
            g_scr_key[0].leds = LED1|LED4;
            g_scr_key[1].okey.keys = KEY2|KEY5;
            g_scr_key[1].leds = LED2|LED5;
            g_scr_key[2].okey.keys = KEY3|KEY6;
            g_scr_key[2].leds = LED3|LED6;
        } break;
        case 4 : {
            g_scr_key[0].okey.keys = KEY1|KEY2|KEY3;
            g_scr_key[0].leds = LED1|LED2|LED3;
            g_scr_key[1].okey.keys = KEY4;
            g_scr_key[1].leds = LED4;
            g_scr_key[2].okey.keys = KEY5;
            g_scr_key[2].leds = LED5;
            g_scr_key[3].okey.keys = KEY6;
            g_scr_key[3].leds = LED6;
        } break;
        case 5 : {
            g_scr_key[0].okey.keys = KEY1|KEY2;
            g_scr_key[0].leds = LED1|LED2;
            g_scr_key[1].okey.keys = KEY3;
            g_scr_key[1].leds = LED3;
            g_scr_key[2].okey.keys = KEY4;
            g_scr_key[2].leds = LED4;
            g_scr_key[3].okey.keys = KEY5;
            g_scr_key[3].leds = LED5;
            g_scr_key[4].okey.keys = KEY6;
            g_scr_key[4].leds = LED6;
        } break;
        case 6 : {
            g_scr_key[0].okey.keys = KEY1;
            g_scr_key[0].leds = LED1;
            g_scr_key[1].okey.keys = KEY2;
            g_scr_key[1].leds = LED2;
            g_scr_key[2].okey.keys = KEY3;
            g_scr_key[2].leds = LED3;
            g_scr_key[3].okey.keys = KEY4;
            g_scr_key[3].leds = LED4;
            g_scr_key[4].okey.keys = KEY5;
            g_scr_key[4].leds = LED5;
            g_scr_key[5].okey.keys = KEY6;
            g_scr_key[5].leds = LED6;
       } break;
       default : {
       } return 1;
    }

    index = 0;
    if (1 == info.scr_num.wait) {
        g_scr_key[index].type = KEY_TYPE_WAIT;
        g_scr_key[index].state = KEY_STATE_CLOSE;
        g_scr_key[index].light_p = board_light_register(LIGHT1);
        g_scr_key[index].keyval = 0x01;
        index++;
    }
    
    for (i = 0; i < info.scr_num.other; i++) {
        g_scr_key[index].type = KEY_TYPE_OTHER;
        g_scr_key[index].state = KEY_STATE_CLOSE;
        g_scr_key[index].light_p = board_light_register(1 << index);
        g_scr_key[index].keyval = (1 << index);
        index++;
    }
    

    return 0;
}

void ScrKeyStateSwitch(scr_key_t *p, key_t keys) {

    okey_state_switch(&p->okey, keys);
}

void ScrKeyStateOpen(scr_key_t *p) {

    if (KEY_STATE_OPEN == p->state) {
        return;
    }

    if (KEY_TYPE_WAIT == p->type) {
        WaitDelayTick = WAIT_DELAY_TICK;
    } else {
        ScrKeyMutexOther(p);
    }
    
    board_light_open(p->light_p);
    board_led_flash_del(p->leds);
    board_led_open(p->leds);
    p->state = KEY_STATE_OPEN;

}

void ScrKeyStateClose(scr_key_t *p) {

    if (KEY_STATE_CLOSE == p->state) {
        return;
    }
    
    board_light_close(p->light_p);
    board_led_flash_del(p->leds);
    board_led_close(p->leds);
    p->state = KEY_STATE_CLOSE;

}

void ScrKeyStateNone(scr_key_t *p) {

    if (KEY_STATE_NONE == p->state) {
        return ;
    }
    board_light_close(p->light_p);
    board_led_flash_add(p->leds);
    p->state = KEY_STATE_NONE;

}

static void AssocRun(scr_key_t *p, net_cmd_t cmd) {

    if (0x00 == p->assoc_key) {
        return ;
    }
    NetworkSend(cmd, p->assoc_key);
}

void ScrKeyMutexOther(scr_key_t *p) {

    unsigned char i = 0;

    if (KEY_TYPE_WAIT == g_scr_key[0].type) {
        i = 1;
    }
    
    for ( ; i < MAX_SCR_KEY_SIZE; i++) {
        if (KEY_TYPE_NONE == g_scr_key[i].type) {
            continue;
        }
        if (p->keyval == g_scr_key[i].keyval) {
            continue;
        }
        if (KEY_STATE_OPEN == g_scr_key[i].state) {
            ScrKeyStateClose(&g_scr_key[i]);
            if (0x00 != g_scr_key[i].assoc_key) {
                NetworkSend(NET_CMD_ASSOC_CLOSE, g_scr_key[i].assoc_key);
            }
        }
    }
    
}

void ScrKeyMutexAll(scr_key_t *p) {

    unsigned char i = 0;

    for (i = 0; i < MAX_SCR_KEY_SIZE; i++) {
        if (KEY_TYPE_NONE == g_scr_key[i].type) {
            break;
        }
        if (p->keyval == g_scr_key[i].keyval) {
            continue;
        }
        ScrKeyStateClose(&g_scr_key[i]);
    }
    
}

unsigned char ScrKeyHaveOpen(void) {

    unsigned char i = 0;

    for (i = 0; i < MAX_SCR_KEY_SIZE; i++) {
        if (KEY_TYPE_NONE == g_scr_key[i].type) {
            break;
        }
        if (KEY_STATE_OPEN == g_scr_key[i].state) {
            return 1;
        }
    }

    return 0;
}

void ScrKeyProcess(scr_key_t *p) {

    if (KEY_TYPE_NONE == p->type) {
        return ;
    }

    if (OKEY_STATE_DOWN == p->okey.okey_state) {
        if (KEY_STATE_CLOSE == p->state) {
            ScrKeyStateOpen(p);
            if (1 == SysInfoIsMaster()) {
                NetworkSend(NET_CMD_LOAD_OPEN, p->keyval);
            }
            AssocRun(p, NET_CMD_ASSOC_OPEN);
        } else {
            ScrKeyStateClose(p);
            if (1 == SysInfoIsMaster()) {
                NetworkSend(NET_CMD_LOAD_CLOSE, p->keyval);
            }
            AssocRun(p, NET_CMD_ASSOC_CLOSE);
        }
    }
    
}

void ScrKeyAssocResum(unsigned char keynum) {

    uint32_t pos = ASSOC_KEY_ADDR+sizeof(main_key_t)*keynum;
    
    FlashReadData(&g_scr_key[keynum].assoc_key, pos, sizeof(main_key_t));

}

void ScrKeyAssocBackup(unsigned char keynum) {

    uint32_t pos = ASSOC_KEY_ADDR+sizeof(main_key_t)*keynum;
    
    FlashWriteData(&g_scr_key[keynum].assoc_key, pos, SYS_INFO_SIZE); 
    
}

void ScrKeyAssocClear(void) {

    unsigned char clear[ASSOC_KEY_SZIE] = {0x00};
    
    FlashWriteData(clear, ASSOC_KEY_ADDR, sizeof(main_key_t));
}

void ScrKeyResum(void) {

    unsigned char i = 0;
    scr_key_t *p = NULL;
    
    for (i = 0; i < MAX_SCR_KEY_SIZE; i++) {
        p = &g_scr_key[i];
        if (KEY_TYPE_NONE == p->type) {
            break;
        }
        ScrKeyAssocResum(i);
    }
    
}



