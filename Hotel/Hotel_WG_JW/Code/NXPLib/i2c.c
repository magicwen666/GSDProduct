#include "chip.h"
#include "delay.h"
#include "I2CINT.h"

/*
typedef struct
{
  __IO uint32_t CONSET;                 // Offset: 0x000 I2C Control Set Register (R/W)
  __I  uint32_t STAT;                   // Offset: 0x004 I2C Status Register (R/ )
  __IO uint32_t DAT;                    // Offset: 0x008 I2C Data Register (R/W)
  __IO uint32_t ADR0;                   // Offset: 0x00C I2C Slave Address Register 0 (R/W)
  __IO uint32_t SCLH;                   // Offset: 0x010 SCH Duty Cycle Register High Half Word (R/W)
  __IO uint32_t SCLL;                   // Offset: 0x014 SCL Duty Cycle Register Low Half Word (R/W)
  __O  uint32_t CONCLR;                 // Offset: 0x018 I2C Control Clear Register ( /W)
  __IO uint32_t MMCTRL;                 // Offset: 0x01C Monitor mode control register (R/W)
  __IO uint32_t ADR1;                   // Offset: 0x020 I2C Slave Address Register 1 (R/W)
  __IO uint32_t ADR2;                   // Offset: 0x024 I2C Slave Address Register 2 (R/W)
  __IO uint32_t ADR3;                   // Offset: 0x028 I2C Slave Address Register 3 (R/W)
  __I  uint32_t DATA_BUFFER;            // Offset: 0x02C Data buffer register ( /W) 
  __IO uint32_t MASK0;                  // Offset: 0x030 I2C Slave address mask register 0 (R/W)
  __IO uint32_t MASK1;                  // Offset: 0x034 I2C Slave address mask register 1 (R/W)
  __IO uint32_t MASK2;                  // Offset: 0x038 I2C Slave address mask register 2 (R/W)
  __IO uint32_t MASK3;                  // Offset: 0x03C I2C Slave address mask register 3 (R/W)
} LPC_I2C_TypeDef;
*/

// 引用的外部变量
extern uint32_t SystemCoreClock;

void i2c_init(uint32_t clk)
{
    if (clk > 400000) clk = 400000;

    //LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 16);
    //LPC_IOCON->PIO0_4 |= 0x01;
    //LPC_IOCON->PIO0_5 |= 0x01;

    syscon_ppp_reset_dis(SYSCON_PPP_I2C_RST_N);     /* 禁止复位I2C（必须）          */
    syscon_ahb_enable(SYSCON_CLK_I2C);              /* 打开i2c功能部件时钟          */
    
    LPC_I2C->SCLH = (SystemCoreClock/clk + 1) / 2;  /* 设置I2C时钟为fi2c            */
    LPC_I2C->SCLL = (SystemCoreClock/clk) / 2;
    LPC_I2C->CONCLR = 0x2C;
    
    NVIC_EnableIRQ(I2C_IRQn);                       /* 设置I2C1中断优先级并使能中断 */
    NVIC_SetPriority(I2C_IRQn, 1);
    LPC_I2C->CONSET = 0x40;   
}

uint8_t i2c_send_byte(uint8_t addr, uint8_t data)
{
    return ISendByte(addr, data);
}

uint8_t i2c_recv_byte(uint8_t addr, uint8_t *pdata)
{
    return IRcvByte(addr, pdata);
}

uint8_t i2c_write_nbyte(uint8_t addr, 
                        uint8_t addr_type, 
                        uint32_t sub_addr, 
                        uint8_t *pdata, 
                        uint32_t num)
{
    
    
    if(I2C_WriteNByte(addr, addr_type, sub_addr,  pdata, num) == TRUE){
        delay_ms(3);
        return TRUE;
    }else{
        return FALSE;
    }
}

uint8_t i2c_read_nbyte( uint8_t addr, 
                        uint8_t addr_type, 
                        uint32_t sub_addr, 
                        uint8_t *pdata, 
                        uint32_t num)
{
    if(I2C_ReadNByte(addr, addr_type, sub_addr, pdata, num) == TRUE){
        return TRUE;
    }else{
        return FALSE;
    }
}

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


