#include "key.h"
#include "delay.h"
#include "chip.h"
#include "SysTimer.h"
#include "SysInfo.h"

#define KEY_VAL gpio_get_bitval(LPC_GPIO0, 1)

void KeyInit(void) {

    syscon_ahb_enable(SYSCON_CLK_IOCON);
    /* p0_1 4 */
    LPC_IOCON->PIO0_1 |= IOCON_FUNC0;
    syscon_ahb_enable(SYSCON_CLK_GPIO);
    gpio_set_dir(LPC_GPIO0, 1, GPIO_IN);
    
}

unsigned char KeyScan(void) {

    if (0x01 == KEY_VAL) {
        return KEY_NONE;
    }
    if (0 == KEY_VAL) {
        delay_ms(80);
        if (0 == KEY_VAL) {
            return KEY_HAVE;
        }
    }

    return KEY_NONE;
}

static key_sta_t KeySta = KEY_STA_UP;
static volatile unsigned int KeyTime = 0;
#define KEY_LONG_TIME   3000
#define KEY_LONG_LONG_TIME  5000
key_sta_t KeyStaSwitch(unsigned char keys) {
    
    switch (KeySta) {
        case KEY_STA_UP : {
            if (KEY_HAVE == keys) {
                KeySta = KEY_STA_DOWN;
            } else {
                KeySta = KEY_STA_UP;
            }
        } break;
        case KEY_STA_DOWN : {
            if (KEY_HAVE == keys) {
                KeySta = KEY_STA_HOLD;
                KeyTime = SysTimerGet();
            } else {
                KeySta = KEY_STA_UP;
            }
        } break;
        case KEY_STA_HOLD : {
            if (KEY_HAVE == keys) {
                if (SysTimerCalc(KeyTime, SysTimerGet()) > KEY_LONG_TIME) {
                    KeySta = KEY_STA_LHOLD;
                }else {
                    KeySta = KEY_STA_HOLD;
                }
            } else {
                KeySta = KEY_STA_UP;
            }
        } break;
        case KEY_STA_LHOLD : {
            if (KEY_HAVE == keys) {
                if (SysTimerCalc(KeyTime, SysTimerGet()) > KEY_LONG_LONG_TIME) {
                    KeySta = KEY_STA_LLHOLD;
                }  else {
                    KeySta = KEY_STA_LHOLD;
                }
            } else {
                KeySta = KEY_STA_UP;
            }
        } break;
        case KEY_STA_LLHOLD : {
            if (KEY_HAVE == keys) {
                KeySta = KEY_STA_LLHOLD;
            } else {
                KeySta = KEY_STA_UP;
            }
        } break;
        default : {
        } break;
    }

    return KeySta;
}

void KeyListened(void) {

    unsigned char sta = KeyStaSwitch(KeyScan());

    switch (sta) {
        case KEY_STA_DOWN : {
        } break;
        case KEY_STA_LHOLD : {
            SystemReset();
        } break;
    }
    
}


