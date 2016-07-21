#include "chip.h"

/*
typedef struct
{
  union {
  __I  uint32_t  RBR;           // Offset: 0x000 Receiver Buffer  Register (R/ )
  __O  uint32_t  THR;           // Offset: 0x000 Transmit Holding Register ( /W)
  __IO uint32_t  DLL;           // Offset: 0x000 Divisor Latch LSB (R/W)
  };
  union {
  __IO uint32_t  DLM;           // Offset: 0x004 Divisor Latch MSB (R/W)
  __IO uint32_t  IER;           // Offset: 0x000 Interrupt Enable Register (R/W)
  };
  union {
  __I  uint32_t  IIR;           // Offset: 0x008 Interrupt ID Register (R/ )
  __O  uint32_t  FCR;           // Offset: 0x008 FIFO Control Register ( /W)
  };
  __IO uint32_t  LCR;           // Offset: 0x00C Line Control Register (R/W)
  __IO uint32_t  MCR;           // Offset: 0x010 Modem control Register (R/W)
  __I  uint32_t  LSR;           // Offset: 0x014 Line Status Register (R/ )
  __I  uint32_t  MSR;           // Offset: 0x018 Modem status Register (R/ )
  __IO uint32_t  SCR;           // Offset: 0x01C Scratch Pad Register (R/W)
  __IO uint32_t  ACR;           // Offset: 0x020 Auto-baud Control Register (R/W)
       uint32_t  RESERVED0;
  __IO uint32_t  FDR;           // Offset: 0x028 Fractional Divider Register (R/W)
       uint32_t  RESERVED1;
  __IO uint32_t  TER;           // Offset: 0x030 Transmit Enable Register (R/W)
       uint32_t  RESERVED2[6];
  __IO uint32_t  RS485CTRL;     // Offset: 0x04C RS-485/EIA-485 Control Register (R/W)
  __IO uint32_t  ADRMATCH;      // Offset: 0x050 RS-485/EIA-485 address match Register (R/W)
  __IO uint32_t  RS485DLY;      // Offset: 0x054 RS-485/EIA-485 direction control delay Register (R/W)
  __I  uint32_t  FIFOLVL;       // Offset: 0x058 FIFO Level Register (R)
} LPC_UART_TypeDef;
*/

void uart0_init(unsigned long baud)
{
    uint16_t usFdiv;

    syscon_ahb_enable(SYSCON_CLK_UART0);     // uart外设时钟(开启后可以配置外设)
    syscon_uart0_div(1);   

    LPC_UART0->FDR = (1<<0)|(2<<4); // FDR = DIVVAL/MULVAL = 1/2
    LPC_UART0->LCR  = 0x83;  // 解锁波特率设置
    usFdiv = (SystemCoreClock/LPC_SYSCON->UART0CLKDIV)/(16*baud*(1+0.5));  // 设置波特率
    LPC_UART0->DLM  = usFdiv / 256;
    LPC_UART0->DLL  = usFdiv % 256; 
    LPC_UART0->LCR  = 0x03;  // 锁定波特率设置   
    
    LPC_UART0->FCR  = 0x87;  // 清空FIFO,   8字节缓冲

    //LPC_UART0->RS485CTRL = 0x30;         // 配置485
    //LPC_UART->RS485CTRL = 0x10;         // 配置485

    NVIC_EnableIRQ(UART0_IRQn);
    NVIC_SetPriority(UART0_IRQn, 1);

    LPC_UART0->IER = UART_IER_RBRIE;  // 使能接收中断
}

void uart0_send_byte(uint8_t data)
{
    LPC_UART0->THR = data;                   // 写入数据
    while ((LPC_UART0->LSR & 0x40) == 0);    // 等待数据发送完毕
}

uint8_t uart0_recv_byte(void)
{
    return LPC_UART0->RBR;
}

void uart1_init(unsigned long baud) {

    uint16_t usFdiv;

    syscon_ahb_enable(SYSCON_CLK_UART1);
    LPC_SYSCON->UART1CLKDIV = 1;   

    LPC_UART1->FDR = (1<<0)|(2<<4);
    LPC_UART1->LCR  = 0x83;  
    usFdiv = (SystemCoreClock/LPC_SYSCON->UART1CLKDIV)/(16*baud*(1+0.5)); 
    LPC_UART1->DLM  = usFdiv / 256;
    LPC_UART1->DLL  = usFdiv % 256; 
    LPC_UART1->LCR  = 0x03; 
    LPC_UART1->FCR  = 0x87;

    NVIC_EnableIRQ(UART1_IRQn);
    NVIC_SetPriority(UART1_IRQn, 2);
    LPC_UART1->IER = UART_IER_RBRIE;
    
}

void uart1_send_byte(uint8_t data) {

    LPC_UART1->THR = data; 
    while ((LPC_UART1->LSR & 0x40) == 0); 
}

uint8_t uart1_recv_byte(void) {
    return LPC_UART1->RBR;
}

void uart2_init(unsigned long baud)
{
    uint16_t usFdiv;

    syscon_ahb_enable(SYSCON_CLK_UART2);     // uart外设时钟(开启后可以配置外设)

    LPC_IOCON->PIO0_3 = (IOCON_FUNC3|IOCON_MODE_PULLUP);    
    LPC_IOCON->PIO1_8 = (IOCON_FUNC3|IOCON_MODE_PULLUP); 

    LPC_SYSCON->UART2CLKDIV  = 4;
    LPC_UART2->FDR = (5<<0)|(8<<4); // FDR = DIVVAL/MULVAL
    LPC_UART2->LCR  = 0x83;  // 解锁波特率设置
    usFdiv = (SystemCoreClock/LPC_SYSCON->UART2CLKDIV)/(16*baud*(1+5.0/8));  // 设置波特率
    LPC_UART2->DLM  = usFdiv / 256;
    LPC_UART2->DLL  = usFdiv % 256;
    LPC_UART2->LCR  = 0x03; 
    LPC_UART2->FCR  = 0x87;  // 清空FIFO,   8字节缓冲

    NVIC_EnableIRQ(UART2_IRQn);
    NVIC_SetPriority(UART2_IRQn, 1);
    LPC_UART2->IER = UART_IER_RBRIE;  // 使能接收中断
    
}

void uart2_send_byte(uint8_t data)
{
    LPC_UART2->THR = data;                   // 写入数据
    while ((LPC_UART2->LSR & 0x40) == 0);    // 等待数据发送完毕
}

void uart2_send(void *data, uint16_t len) 
{
    unsigned char *p = (unsigned char *)data;
    if ((NULL == p) || (0 == len)) {
        return ;
    }
    while (len--) {
        LPC_UART2->THR = *p++;
        while ((LPC_UART2->LSR & 0x40) == 0); 
    }
}

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


