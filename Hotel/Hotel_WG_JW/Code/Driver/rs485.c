#include "rs485.h"
#include "chip.h"
#include "delay.h"
#include <string.h>

#define RS485_SEND_MODE()   (LPC_GPIO2->DATA |= (1 << 9))
#define RS485_RECV_MODE()   (LPC_GPIO2->DATA &= ~(1 << 9))
#define UsartInterrupt      UART0_IRQHandler
#define USART_RECV_MODE_IDLE    0
#define USART_RECV_MODE_SHELL   1
#define USART_RECV_MODE_RS485   2
volatile unsigned char UsartRecvMode = USART_RECV_MODE_IDLE;
//#define USART_RECV_MAX_LEN  128
unsigned char UsartBuf[USART_RECV_MAX_LEN] = {0x00};
volatile unsigned short UsartLen = 0;

int fputc(int ch, FILE *fp) {

    RS485_SEND_MODE();
    //if ('\n' == ch) {
        //uart0_send_byte('\r');
    //}

    uart0_send_byte(ch);
    RS485_RECV_MODE();

    return ch;
}

static void UsartIntDealShell(unsigned char r) {

    UsartBuf[UsartLen++] = r;
    if (('\r' == r) || ('\n' == r)) {
        UsartLen |= 0x8000;
        return ;
    }
    
}

static void UsartIntDealRs485(unsigned char r) {
    
    UsartBuf[UsartLen++] = r;
    if (0xFE == r) {
        UsartLen |= 0x8000;
        return ;
    }
    
}

void UsartInterrupt(void) {

    unsigned char r = 0;

    //if (0x01 == (LPC_UART0->IIR & 0x01)) {
        //return ;
    //}
    
    r = LPC_UART0->RBR;
    if (0x8000 == (UsartLen & 0x8000)) {
        return ;
    }
    
    switch (UsartRecvMode) {
        case USART_RECV_MODE_IDLE : {
            if (0xFD == r) {
                UsartRecvMode = USART_RECV_MODE_RS485;
                UsartIntDealRs485(r);
            } else {
                UsartRecvMode = USART_RECV_MODE_SHELL;
                UsartIntDealShell(r);
            }
        } break;
        case USART_RECV_MODE_RS485 : {
            UsartIntDealRs485(r);
        } break;
        case USART_RECV_MODE_SHELL : {
            UsartIntDealShell(r);
        } break;
    }
    
}

void RS485Init(void) {

    syscon_ahb_enable(SYSCON_CLK_IOCON);
    LPC_IOCON->PIO1_6 |= (IOCON_FUNC1 | IOCON_MODE_PULLUP);
    LPC_IOCON->PIO1_7 |= (IOCON_FUNC1 | IOCON_MODE_PULLUP);
    LPC_IOCON->PIO2_9 |= (IOCON_FUNC0 | IOCON_MODE_PULLUP);
    syscon_ahb_enable(SYSCON_CLK_GPIO);
    gpio_set_dir(LPC_GPIO2, 9, GPIO_OUT);
    
    uart0_init(9600);
    RS485_RECV_MODE();
    
}

void RS485Send(void *data, unsigned int len) {

    unsigned char *pos = (unsigned char *)data;
    
    RS485_SEND_MODE();
    while (len--) {
        uart0_send_byte(*pos++);
    }
    delay_ms(10);
    RS485_RECV_MODE();
}

unsigned char RS485Recv(void *data, unsigned int *len) {

    if ((0x8000 != (UsartLen & 0x8000)) || 
        (USART_RECV_MODE_RS485 != UsartRecvMode)) {
        return 1;    
    }

    if (NULL != data) {
        memcpy(data, UsartBuf, UsartLen & 0x7FFF);
    }
    if (NULL != len) {
        *len = UsartLen & 0x7FFF;
    }
    
    UsartLen = 0;
    memset(UsartBuf, 0x00, sizeof(UsartBuf));
    UsartRecvMode = USART_RECV_MODE_IDLE;
    
    return 0;
}


