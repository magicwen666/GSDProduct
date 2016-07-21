#ifndef __UART_H__
#define __UART_H__

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

// IER
#define UART_IER_RBRIE      (1<<0)
#define UART_IER_THREIE     (1<<1)
#define UART_IER_RXLIE      (1<<2)
#define UART_IER_ABEOIE     (1<<8)
#define UART_IER_ABT0IE     (1<<9)

// IIR
#define UART_IIR_RLS        (0x03<<1)   // 接收线状态
#define UART_IIR_RDA        (0x02<<1)   // 接收数据可用
#define UART_IIR_CTI        (0x06<<1)   // 字符超时指示
#define UART_IIR_THRE       (0x01<<1)   // THRE中断
#define UART_IIR_MODEM      (0x00<<4)   // Modem中断

#define UART_IIR_ABEO       (1<<8)      // 自动波特率结束中断
#define UART_IIR_ABTO       (1<<9)      // 自动波特率超时中断

// LCR
#define UART_LCR_WLS_5      (0x00<<0)
#define UART_LCR_WLS_6      (0x01<<0)
#define UART_LCR_WLS_7      (0x02<<0)
#define UART_LCR_WLS_8      (0x03<<0)

#define UART_LCR_SBS_1      (0x00<<2)
#define UART_LCR_SBS_2      (0x01<<2)

#define UART_LCR_PE_OFF     (0x00<<3)
#define UART_LCR_PE_ON      (0x01<<3)

#define UART_LCR_PS_ODD     (0x00<<4)
#define UART_LCR_PS_EVN     (0x01<<4)
#define UART_LCR_PS_1       (0x02<<4)
#define UART_LCR_PS_0       (0x03<<4)

#define UART_LCR_BC_OFF     (0x00<<6)
#define UART_LCR_BC_ON      (0x01<<6)

#define UART_LCR_DLAB_OFF   (0x00<<7)
#define UART_LCR_DLAB_ON    (0x01<<7)

// LSR
#define UART_LSR_RDR        (1<<0)
#define UART_LSR_OE         (1<<1)
#define UART_LSR_PE         (1<<2)
#define UART_LSR_FE         (1<<3)
#define UART_LSR_BI         (1<<4)
#define UART_LSR_THRE       (1<<5)
#define UART_LSR_TEMT       (1<<6)
#define UART_LSR_RXFE       (1<<7)

void uart0_init(unsigned long baud);
void uart0_send_byte(uint8_t data);
uint8_t uart0_recv_byte(void);
void uart1_init(unsigned long baud);
void uart1_send_byte(uint8_t data);
uint8_t uart1_recv_byte(void);
void uart2_init(unsigned long baud);
void uart2_send_byte(uint8_t data);
void uart2_send(void *data, uint16_t len);

#endif

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


