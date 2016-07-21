#include "sys.h"
#include "board_key.h"
#include "board_wdt.h"

static volatile key_t _board_keys = 0;
static volatile tick_t _key_ticks = 0;

#define KEY_SCAN_TIME   80
void ch2_on(void)
{
    gpio_set_bitval(LPC_GPIO2, 5, GPIO_H);
    gpio_set_bitval(LPC_GPIO2, 7, GPIO_L);
}

void ch1_on(void)
{
    gpio_set_bitval(LPC_GPIO2, 5, GPIO_L);
    gpio_set_bitval(LPC_GPIO2, 7, GPIO_H);
}

key_t ch1_get(void)
{
    key_t tmp = 0;

    if(!gpio_get_bitval(LPC_GPIO2, 4)){tmp |= KEY1;}
    if(!gpio_get_bitval(LPC_GPIO2, 1)){tmp |= KEY2;}
    if(!gpio_get_bitval(LPC_GPIO1, 9)){tmp |= KEY3;}
    return tmp;
}

key_t ch2_get(void)
{
    key_t tmp = 0;

    if(!gpio_get_bitval(LPC_GPIO2, 4)){tmp |= KEY4;}
    if(!gpio_get_bitval(LPC_GPIO2, 1)){tmp |= KEY5;}
    if(!gpio_get_bitval(LPC_GPIO1, 9)){tmp |= KEY6;}
    return tmp;
}

// 毫秒定时器注册
void handler_key_scan(void)
{
    if(_key_ticks > KEY_SCAN_TIME){
        _key_ticks = 0;
    }else{
        _key_ticks++;
    }

    if(0 == _key_ticks){
        _board_keys &= ~(KEY4|KEY5|KEY6);
        _board_keys |= ch2_get();
        ch1_on();
    }else if(KEY_SCAN_TIME/2 == _key_ticks){
        _board_keys &= ~(KEY1|KEY2|KEY3);
        _board_keys |= ch1_get();
        ch2_on();
    }
}

// 毫秒定时器注册
void handler_key_scan2(void)
{

}

void board_key_init(void)
{
    syscon_ahb_enable(SYSCON_CLK_IOCON);
    // 20 LIE_LEFT
    LPC_IOCON->PIO2_5 = (IOCON_FUNC0|IOCON_MODE_INACT|IOCON_OPENDRAIN_EN);
    // 11 LIE_RIGHT
    LPC_IOCON->PIO2_7 = (IOCON_FUNC0|IOCON_MODE_INACT|IOCON_OPENDRAIN_EN);
    // 19 HANG1
    LPC_IOCON->PIO2_4 = (IOCON_FUNC0|IOCON_MODE_INACT);
    // 13 HANG2
    LPC_IOCON->PIO2_1 = (IOCON_FUNC0|IOCON_MODE_INACT);
    // 17 HANG3
    LPC_IOCON->PIO1_9 = (IOCON_FUNC0|IOCON_MODE_INACT);

    syscon_ahb_enable(SYSCON_CLK_GPIO);
    gpio_set_dir(LPC_GPIO2, 5, GPIO_OUT);
    gpio_set_dir(LPC_GPIO2, 7, GPIO_OUT);
    gpio_set_dir(LPC_GPIO2, 4, GPIO_IN);
    gpio_set_dir(LPC_GPIO2, 1, GPIO_IN);
    gpio_set_dir(LPC_GPIO1, 9, GPIO_IN);
}

key_t board_key_scan(void)
{
    return _board_keys;
}

uint8_t board_key_free_wait(key_t keys)
{
    key_t key_tmp;

    while(1){
        board_wdt_feed();   // 喂狗
        key_tmp = board_key_scan();
        if(key_tmp&keys){
            continue;
        }else{
            return TRUE;
        }
    }
}

void okey_clear(okey_t *okey_p)
{
    okey_p->keys = 0;
    okey_p->okey_state = OKEY_STATE_UP;
    okey_p->okey_time = 0;
}

/*
 * func     OKEY按键状态切换
 * param1   okey按键指针
 * param2   原始按键电平
 * retval   none
 */
void okey_state_switch(okey_t *okey_p, key_t keys)
{
    switch(okey_p->okey_state){
    case OKEY_STATE_UP:
        if(okey_p->keys&keys){
            okey_p->okey_state = OKEY_STATE_DOWN;
        }else{
            okey_p->okey_state = OKEY_STATE_UP;
        }
        break;
    case OKEY_STATE_DOWN:
        if(okey_p->keys&keys){
            okey_p->okey_state = OKEY_STATE_HOLD;
            okey_p->okey_time = board_time_get();
        }else{
            okey_p->okey_state = OKEY_STATE_CLICK;
        }
        break;
    case OKEY_STATE_HOLD:
        if(okey_p->keys&keys){
            #if 0
            // 无长按模式，按键时间由用户自行定义
            okey_p->okey_state = OKEY_STATE_HOLD;
            #else
            // 长按模式
            if((board_time_get() - okey_p->okey_time) > 3000){
                okey_p->okey_state = OKEY_STATE_LHOLD;
            }else{
                okey_p->okey_state = OKEY_STATE_HOLD;
            }
            #endif
        }else{
            okey_p->okey_state = OKEY_STATE_CLICK;
        }
        break;
    case OKEY_STATE_LHOLD:
        if(okey_p->keys&keys){
            okey_p->okey_state = OKEY_STATE_LHOLD;
        }else{
            okey_p->okey_state = OKEY_STATE_LCLICK;
        }
        break;
    case OKEY_STATE_CLICK:
        if(okey_p->keys&keys){
            okey_p->okey_state = OKEY_STATE_DOWN;
        }else{
            okey_p->okey_state = OKEY_STATE_UP;
        }
        break;
    case OKEY_STATE_LCLICK:
        if(okey_p->keys&keys){
            okey_p->okey_state = OKEY_STATE_DOWN;
        }else{
            okey_p->okey_state = OKEY_STATE_UP;
        }
        break;
    default:
        break;
    }
}

/*
 * func     OKEY按键长按状态判断
 * param1   okey按键指针
 * param2   长按时间
 * retval   TRUE FALSE
 */
uint8_t is_okey_lhold(okey_t *okey_p, tick_t tim)
{
    if((okey_p->okey_state == OKEY_STATE_HOLD)
        &&(board_time_calc(okey_p->okey_time, board_time_get())> tim)){
        //okey_p->okey_state = OKEY_STATE_LHOLD;
        return TRUE;
    }else if(okey_p->okey_state == OKEY_STATE_LHOLD){
        return TRUE;
    }

    return FALSE;

//    if(okey_p->okey_state == OKEY_STATE_LHOLD){
//        return TRUE;
//    }else{
//        return FALSE;
//    }
}

void okey_state_set(okey_t *okey_p, okey_state_t okey_state)
{
    okey_p->okey_state = okey_state;
}

void okey_time_set(okey_t *okey_p, tick_t tim)
{
    okey_p->okey_time = tim;
}

void okey_time_reset(okey_t *okey_p)
{
    okey_p->okey_time = board_time_get();
    okey_p->okey_state = OKEY_STATE_HOLD;
}

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/



