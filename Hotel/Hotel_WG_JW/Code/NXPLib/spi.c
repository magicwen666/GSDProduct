#include "chip.h"
/*
typedef struct {                                    // (@ 0x40040000) SSP0 Structure
  __IO uint32_t  CR0;                               // (@ 0x40040000) Control register 0. Selects the serial clock
                                                    //   rate, bus type, and data size.
  __IO uint32_t  CR1;                               // (@ 0x40040004) Control register 1. Selects master/slave and
                                                    //   other modes.
  __IO uint32_t  DR;                                // (@ 0x40040008) Data register. Writes fill the transmit FIFO,
                                                    //   and reads empty the receive FIFO.
  __I  uint32_t  SR;                                // (@ 0x4004000C) Status register
  __IO uint32_t  CPSR;                              // (@ 0x40040010) Clock Prescale register
  __IO uint32_t  IMSC;                              // (@ 0x40040014) Interrupt Mask Set and Clear register
  __I  uint32_t  RIS;                               // (@ 0x40040018) Raw Interrupt Status register
  __I  uint32_t  MIS;                               // (@ 0x4004001C) Masked Interrupt Status register
  __O  uint32_t  ICR;                               // (@ 0x40040020) SSPICR Interrupt Clear register
} LPC_SSP0_Type;
*/

#include "spi.h"

void spi0_master_init(void)
{

    // 引脚配置

    // 外设配置
    syscon_ahb_enable(SYSCON_CLK_SSP0);                                 // 打开SPI0外设 
    syscon_spi0_div(1);                                                 // SSP_PCLK时钟分频
    syscon_ppp_reset_dis(SYSCON_PPP_SSP0_RST_N);                        // 禁止SPI0复位   

    LPC_SSP0->CR0 = (0x01 << 8) |                                       // SCR  设置BIT_CLK时钟分频
                    (0x01 << 7) |                                       //  CPHA 时钟输出相位,
                                                                        //  仅SPI模式有效
                    (0x01 << 6) |                                       //  CPOL 时钟输出极性,
                                                                        //  仅SPI模式有效
                    (0x00 << 4) |                                       //  FRF  帧格式 00=SPI,01=SSI,
                                                                        //  10=Microwire,11=保留
                    (0x07 << 0);                                        //  DSS  数据长度,0000-0010=保留,
                                                                        //  0011=4位,0111=8位,1111=16位

    LPC_SSP0->CR1 = (0x00 << 3) |                                       //  SOD  从机输出禁能,1=禁止
                    (0x00 << 2) |                                       //  MS   主从选择,0=主机,1=从机
                    (0x01 << 1) |                                       //  SSE  SSP使能
                    (0x00 << 0);                                        //  LBM  回写模式

    LPC_SSP0->CPSR = 2;                                                 //  SSPCLK分频值
    LPC_SSP0->ICR  = SSP_ICR_RORIC|SSP_ICR_RTIC;                        //  中断清除寄存器

    NVIC_EnableIRQ(SSP0_IRQn);
    NVIC_SetPriority(SSP0_IRQn, 2);                                     //   中断使能并设置优先级
    LPC_SSP0->IMSC |= SSP_IMSC_RXIM;                                    //   中断使能寄存器
} 

uint8_t spi0_send_byte(uint8_t data)
{  
    LPC_SSP0->DR = data;
    while( (LPC_SSP0->SR & (SSP_SR_BSY)) == 0 );                            
    return(LPC_SSP0->DR);
}

void spi0_slave_init(void)
{

}

void spi1_master_init(void)
{

}

void spi1_slave_init(void)
{

}

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


