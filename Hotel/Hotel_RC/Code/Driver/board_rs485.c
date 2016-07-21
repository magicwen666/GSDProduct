#include "sys.h"
#include "board_time.h"
#include "board_wdt.h"
#include "board_rs485.h"
#include "network.h"
#include <stdio.h>
#include <string.h>


#define RS485_ENTER_TX  (LPC_GPIO1->DATA |= (1 << 10))
#define RS485_ENTER_RX  (LPC_GPIO1->DATA &= ~(1 << 10))

uint8_t _rs485_idle_flag = TRUE;
tick_t _rs485_idle_tick = 0;

void board_rs485_init(void)
{
    // IO功能配置
    syscon_ahb_enable(SYSCON_CLK_IOCON);
    LPC_IOCON->PIO1_10  |= (IOCON_FUNC0|IOCON_MODE_PULLUP);     // 方向选择
    iocon_u0_rxd_loc(U0_RXD_LOC_PIO3_4);        // 引脚重映射
    LPC_IOCON->PIO3_4   = (IOCON_FUNC2|IOCON_MODE_PULLUP);       // RXD
    LPC_IOCON->PIO3_5   = (IOCON_FUNC2|IOCON_MODE_PULLUP);       // TXD

    gpio_set_dir(LPC_GPIO1, 10, GPIO_OUT);
    gpio_set_bitval(LPC_GPIO1, 10, GPIO_L);     // 高发送，低接收
    
    uart0_init(9600);
    RS485_ENTER_RX;
    
}

void board_rs485_write_nbyte(void *data, uint16_t len) {

    unsigned int delay = 800;

    RS485_ENTER_TX;
    unsigned char *p = (unsigned char *)data;
    while (len--) {
        uart0_send_byte(*p++);
    }
    while (delay--);
    RS485_ENTER_RX;

}


void handler_rs485_idle(void)
{
    if(_rs485_idle_flag == FALSE){
        if(_rs485_idle_tick == 0){
            _rs485_idle_flag = TRUE;
        }else{
            _rs485_idle_tick--;
        }
    }
    
}

uint8_t is_board_rs485_idle(void) {
    
	return _rs485_idle_flag;
}

#define RECV_STATE_START    0x00
#define RECV_STATE_END      0x01
static volatile unsigned char recv_sta = RECV_STATE_END;
static unsigned char recv_buf[128] = {0x00};
static volatile unsigned char recv_len = 0;
static volatile unsigned short rs485_recv_flag = 0x00;
static unsigned char rs485_recv_buf[128] = {0x00};

void handler_rs485(void) {

    unsigned char r = 0;

    _rs485_idle_tick = 10; 
    _rs485_idle_flag = FALSE;

    r = LPC_UART0->RBR;
    if ((RECV_STATE_START != recv_sta) && (NET_FRAME_HEAD == r)) {
        recv_sta = RECV_STATE_START;
        return ;
    } else if ((RECV_STATE_START == recv_sta) && (NET_FRAME_TAIL == r)) {
        recv_sta = RECV_STATE_END;
        rs485_recv_flag = recv_len | 0x8000;
        memcpy(rs485_recv_buf, recv_buf, recv_len);
        memset(recv_buf, 0, sizeof(recv_buf));
        recv_len = 0;
        return ;
    }

    if (RECV_STATE_START == recv_sta) {
        recv_buf[recv_len++] = r;
        if (recv_len >= 15) {
            recv_sta = RECV_STATE_END;
            memset(recv_buf, 0, sizeof(recv_buf));
            recv_len = 0;
        }
    }
    
}

uint8_t board_rs485_read(void *data, uint16_t *len) {

    if ((NULL == data) || (NULL == len) ||
        (0x8000 != (rs485_recv_flag & 0x8000))) {
        return FAIL;
    }
    *len = rs485_recv_flag & 0x7FFF;
    rs485_recv_flag = 0x00;
    memcpy(data, rs485_recv_buf, *len);
    memset(rs485_recv_buf, 0x00, sizeof(rs485_recv_buf));

    return SUCC;
}



/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


