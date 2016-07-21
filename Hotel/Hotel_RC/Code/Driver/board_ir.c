#include "chip.h"
#include "board_ir.h"
#include "board_time.h"
#include "timer.h"

#define IRPS_RUN_TICK   1000
#define IRPS_SEND_TICK  100

volatile uint8_t _irps_flag = 0;        // 是否开启接近感应
volatile tick_t _irps_run_tick = 0;     // 发送周期
volatile uint8_t _irps_recv_count = 0;  // 接收脉冲数

volatile uint8_t ir_val = 0;
volatile tick_t _ir_tick = 0;

volatile uint32_t ircode = 0;

volatile tick_t htime = 0;
volatile tick_t ltime = 0;
volatile uint8_t ir_cnt = 32;
volatile uint32_t ir_code = 0;


irps_state_t   _irps_state = IRPS_STATE_OFF;    // 红外接近感应状态

typedef enum
{
    IR_STATE_BOOT,
    IR_STATE_CODE
}ir_state_t;
volatile ir_state_t ir_state = IR_STATE_BOOT;

void handler_ir_tick(void)
{
    _ir_tick++;
}

/*
 * func     红外接收解码,GPIO中断处理函数中注册
 * param    none
 * retval   none
 */
void handler_ir(void)
{
    //uint8_t ir_flag = SUCESS;
    if(gpio_get_intstatus(LPC_GPIO0, 2) == FALSE){
        return;
    }

    if(gpio_get_bitval(LPC_GPIO0, 2)){
        ltime = _ir_tick;
    }else{
        htime = _ir_tick;
        switch(ir_state){
        case IR_STATE_BOOT:
            if(ltime>330&&htime>160){
                // 引导码
                ir_state = IR_STATE_CODE;
//            }else if(htime>350&&ltime<100){
//                // 重复码
//                ir_state = IR_STATE_BOOT;
            }else if(ltime > 20 &&ltime < 150){
                if(_irps_recv_count <20){
                    //_irps_recv_count++;
                    if(_irps_recv_count == 0){
                        _irps_recv_count = 2;
                    }
                }
                ir_state = IR_STATE_BOOT;
            }else{
                ir_state = IR_STATE_BOOT;
            }
            ltime = htime;
            break;
        case IR_STATE_CODE:
            if(ltime>9&&ltime<50){
                if(htime>9&&htime<50){
                    ir_code = ir_code>>1;
                    ir_cnt--;
                }else if(htime>50&&htime<70){
                    ir_code = ir_code>>1;
                    ir_code = ir_code|0x80000000;
                    ir_cnt--;
                }else{
                    ir_state = IR_STATE_BOOT;
                    ir_cnt = 32;
                    ir_code = 0;
                }
                if(0 == ir_cnt){
                    // 接收完成
                    ircode = ir_code;
                    ir_state = IR_STATE_BOOT;
                    ir_cnt = 32;
                    ir_code = 0;
                }
            }else{
                ir_state = IR_STATE_BOOT;
                ir_cnt = 32;
                ir_code = 0;
            }

            break;
        default:
            ir_state = IR_STATE_BOOT;
            ir_cnt = 32;
            ir_code = 0;
            break;
        }
    }
    //htime = 0;
    //ltime = 0;
    _ir_tick = 0;

    gpio_clear_interrupt(LPC_GPIO0, 2);
}

/*
 * func     红外接收初始化
 * param    none
 * retval   none
 */
void board_ir_init(void)
{
    syscon_ahb_enable(SYSCON_CLK_IOCON);
    // 10 IR_RCV
    LPC_IOCON->PIO0_2 = (IOCON_FUNC0|IOCON_MODE_PULLUP);

    syscon_ahb_enable(SYSCON_CLK_GPIO);
    gpio_set_dir(LPC_GPIO0, 2, GPIO_IN);

    gpio_set_interrupt(LPC_GPIO0, 2, GPIO_INTMODE_DB_EDGE);
    gpio_enable_interrupt(LPC_GPIO0, 2);

    NVIC_EnableIRQ(GPIO0_IRQn);
}


ir_t board_ir_get(void)
{
    ir_t ir_tmp = ircode;
    ircode = 0;
    return ir_tmp;
}

void board_ir_clean(void)
{
    ircode = 0;
}

/*
 * func     红外接近感应初始化，前提是红外接收已开启
 * param    none
 * retval   none
 */
void board_irps_init(void)
{
    syscon_ahb_enable(SYSCON_CLK_IOCON);
    // 10 IR_SEND
    LPC_IOCON->PIO3_0 = (IOCON_FUNC2|IOCON_MODE_PULLUP);

    ct16b0_pwn_init();
}



void board_irps_enable(void)
{
    //ct16b0_enable();
    ct16b0_set_match(1, 50);
}

void board_irps_disable(void)
{
    //ct16b0_disable();
    //gpio_set_bitval(LPC_GPIO3, 0, GPIO_L);
    ct16b0_set_match(1, 0);
}

void board_irps_start(void)
{
    _irps_flag = 1;
}

void board_irps_stop(void)
{
    _irps_flag = 0;
    _irps_recv_count = 0;
    //ct16b0_disable();
    board_irps_disable();
}

// 25us时器调注册
void handler_irps_send(void)
{
    if(_irps_flag != 1){
        return;
    }
    _irps_run_tick++;
    if(_irps_run_tick == 1){
        board_irps_enable();
    }else if(_irps_run_tick ==IRPS_SEND_TICK){
        board_irps_disable();
    }else if(_irps_run_tick == IRPS_RUN_TICK){
        _irps_run_tick = 0;
    }
}

irps_state_t board_irps_get(void)
{
    if(_irps_recv_count > 1){
        _irps_state = IRPS_STATE_ON;
    }else{
        _irps_state = IRPS_STATE_OFF;
    }
    return _irps_state;
}


void board_irps_clean(void)
{
    _irps_state = IRPS_STATE_OFF;
}


/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


