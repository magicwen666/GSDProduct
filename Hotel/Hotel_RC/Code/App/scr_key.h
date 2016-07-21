#ifndef __SCR_KEY_H__
#define __SCR_KEY_H__

// 头文件
#include "sys.h"
#include "board_key.h"
#include "board_led.h"
#include "board_light.h"

typedef enum {
    KEY_TYPE_NONE,
    KEY_TYPE_WAIT,
    KEY_TYPE_OTHER,
    KEY_TYPE_CONF,
} key_type_t;

typedef enum {
    KEY_STATE_NONE,
    KEY_STATE_CLOSE,
    KEY_STATE_OPEN,
} key_state_t;

#pragma anon_unions

typedef struct {
    key_type_t type;
    key_state_t state;
    light_t *light_p;
    okey_t okey;
    led_t leds;
    unsigned char keyval;
    main_key_t assoc_key;  // 主关联键
} scr_key_t;

extern scr_key_t g_scr_key[MAX_SCR_KEY_SIZE];
extern unsigned int WaitDelayTick;

void WaitDelayHandler(void);
void ScrKeyClear(void);
unsigned char ScrKeyInit(sys_info_t info);
void ScrKeyStateSwitch(scr_key_t *p, key_t keys);
void ScrKeyStateOpen(scr_key_t *p);
void ScrKeyStateClose(scr_key_t *p);
void ScrKeyStateNone(scr_key_t *p);
void ScrKeyMutexOther(scr_key_t *p);
void ScrKeyMutexAll(scr_key_t *p);
unsigned char ScrKeyHaveOpen(void);
void ScrKeyProcess(scr_key_t *p);
void ScrKeyAssocResum(unsigned char keynum);
void ScrKeyAssocBackup(unsigned char keynum);
void ScrKeyAssocClear(void);
void ScrKeyResum(void);


#endif

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/



