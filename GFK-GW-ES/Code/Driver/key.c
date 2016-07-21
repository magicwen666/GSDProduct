#include "key.h"
#include "delay.h"
#include "chip.h"
#include "SysTimer.h"


#define KEY_LONG_TIME   3000
#define KEY_LONG_LONG_TIME  5000


void KeyHandler(void) {

    
    
}

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
static unsigned int KeyTime = 0;

key_sta_t KeyStaSwitch(unsigned char keys) {

    if (KEY_NONE == keys) {
        KeySta = KEY_STA_UP;
        return KeySta;
    }

    switch (KeySta) {
        case KEY_STA_UP : {
            KeySta = KEY_STA_DOWN;
        } break;
        case KEY_STA_DOWN : {
            KeySta = KEY_STA_HOLD;
            KeyTime = SysTimerGet();
        } break;
        case KEY_STA_HOLD : {
            if (SysTimerCalc(KeyTime, SysTimerGet()) > KEY_LONG_TIME) {
                KeySta = KEY_STA_LHOLD;
            } else {
                KeySta = KEY_STA_HOLD;
            }
        } break;
        case KEY_STA_LHOLD : {
            KeySta = KEY_STA_L2LLSPACE;
        } break;
        case KEY_STA_L2LLSPACE : {
            if (SysTimerCalc(KeyTime, SysTimerGet()) > KEY_LONG_LONG_TIME) {
                KeySta = KEY_STA_LLHOLD;
            }  else {
                KeySta = KEY_STA_L2LLSPACE;
            }
        } break;
        case KEY_STA_LLHOLD : {
            KeySta = KEY_STA_LLHOLD;
        } break;
        default : {
        } break;
    }

    
    return KeySta;

}



