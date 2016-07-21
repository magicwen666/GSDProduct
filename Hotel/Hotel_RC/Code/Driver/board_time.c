#include "sys.h"
#include "board_time.h"
#include "board_wdt.h"

// 全局变量定义
static volatile tick_t _systick = 0;
static volatile tick_t _delayms_time = 0;
static volatile tick_t _timeout_time = 0;
volatile unsigned int lightsensor_tick;

void handler_delayms(void)
{
    if(_delayms_time != 0){
        _delayms_time--;
    }
}

void handler_timeout(void)
{
    if(_timeout_time != 0){
        _timeout_time--;
    }
}

/* 毫秒定时器注册 */
void handler_systick(void)
{
    // 系统计时
    _systick++;
    // 延时
    handler_delayms();
    // 超时
    handler_timeout();
}

void board_time_init(void)
{
    systick_init(1000); // ms定时

    ct16b1_timer_init(40000);    // us定时
}

tick_t board_time_get(void)
{
    return _systick;
}

void board_time_delay1ms(tick_t tim)
{
    _delayms_time = tim;
    while(_delayms_time != 0){
        board_wdt_feed();   // 喂狗
    }
}


void board_time_out_set(tick_t tim)
{
    _timeout_time = tim;
}

tick_t board_time_out_wait(void)
{
    return _timeout_time;
}

tick_t board_time_calc(tick_t tim_pre, tick_t tim_now)
{
    if(tim_now >= tim_pre){
        return tim_now - tim_pre;
    }else{
        return tim_now + MAX_TIME - tim_pre;
    }
}

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/



