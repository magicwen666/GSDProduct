#include <stdio.h>
#include "timer.h"
#include "board_time.h"
#include "board_rs485.h"
#include "board_light.h"
#include "board_key.h"
#include "board_ir.h"
#include "board_led.h"
#include "board_time.h"
#include "ps.h"
#include "scr_key.h"
#include "door.h"

/*
 * func     UART0中断
 * param    none
 * retval   none
 */
void UART0_IRQHandler (void)
{
    handler_rs485();
}

/*
 * func     CT16B0定时器中断
 * param    none
 * retval   none
 */
void CT16B0_IRQHandler(void)
{

}

/*
 * func     CT16B1定时器中断 25us中断
 * param    none
 * retval   none
 */
void CT16B1_IRQHandler(void)
{
    if ( LPC_CT16B1->IR & IR_MR0_FLAG) {
        // 红外定时
        handler_ir_tick();
        handler_irps_send(); 
        LPC_CT16B1->IR = IR_MR0_FLAG;			// clear interrupt flag
    }
    return;
}

/*
 * func     SysTick 中断
 * param    none
 * retval   none
 */
void SysTick_Handler(void)
{
    // 系统计时
    handler_systick();
    // 按键扫描
    handler_key_scan();
    // 闪屏
    handler_flash_led();
    // 转屏
    handler_rotate_led();
    PsHandler();
    // RS485接收超时
    handler_rs485_idle();
    // 请稍候延时
    WaitDelayHandler();
    DoorHander();
    
}

/*
 * func     GPIO0中断
 * param    none
 * retval   none
 */
void GPIO0_IRQHandler(void)
{
    // 红外中断接收
    handler_ir();
}

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


