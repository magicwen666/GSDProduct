#include "chip.h"

/*
typedef struct {                                    // (@ 0x50000000) GPIO0 Structure
  __IO  uint32_t  RESERVED[4095];
  __IO uint32_t  DATA;                              // (@ 0x50003FFC) Port n data register for pins PIOn_0 to PIOn_11
  __I  uint32_t  RESERVED1[4096];
  __IO uint32_t  DIR;                               // (@ 0x50008000) Data direction register for port n
  __IO uint32_t  IS;                                // (@ 0x50008004) Interrupt sense register for port n
  __IO uint32_t  IBE;                               // (@ 0x50008008) Interrupt both edges register for port n
  __IO uint32_t  IEV;                               // (@ 0x5000800C) Interrupt event register for port n
  __IO uint32_t  IE;                                // (@ 0x50008010) Interrupt mask register for port n
  __I  uint32_t  RIS;                               // (@ 0x50008014) Raw interrupt status register for port n
  __I  uint32_t  MIS;                               // (@ 0x50008018) Masked interrupt status register for port n
  __O  uint32_t  IC;                                // (@ 0x5000801C) Interrupt clear register for port n
} LPC_GPIO0_Type;
*/

void gpio_set_dir(LPC_GPIO0_Type *LPC_GPIO, uint32_t bit, uint32_t dir)
{
  if(dir == GPIO_OUT){
		LPC_GPIO->DIR |= (1<<bit);
  }else{
		LPC_GPIO->DIR &= ~(1<<bit);
  }
}

void gpio_set_bitval(LPC_GPIO0_Type *LPC_GPIO, uint32_t bit, uint32_t val)
{
     //LPC_GPIO->MASKED_ACCESS[1<<bit] = (val<<bit);
    
    if(val){
        LPC_GPIO->DATA |= (1<<bit); 
    }else{
        LPC_GPIO->DATA &= ~(1<<bit); 
    } 
}

uint32_t gpio_get_bitval(LPC_GPIO0_Type *LPC_GPIO, uint32_t bit)
{
    if(LPC_GPIO->DATA&(1<<bit)){
        return 0x01;
    }else{
        return 0x00;
    }
}

void gpio_set_interrupt(LPC_GPIO0_Type *LPC_GPIO, uint32_t bit, uint32_t val)
{
    switch(val){
    case GPIO_INTMODE_RISE_EDGE:
        LPC_GPIO->IS &= ~(1<<bit);      // 边沿触发
        LPC_GPIO->IBE &= ~(1<<bit);     // 无双边沿触发
        LPC_GPIO->IEV |= (1<<bit);      // 上升沿或高电平
        break;
    case GPIO_INTMODE_DROP_EDGE:
        LPC_GPIO->IS &= ~(1<<bit);      // 边沿触发
        LPC_GPIO->IBE &= ~(1<<bit);     // 无双边沿触发
        LPC_GPIO->IEV &= ~(1<<bit);     // 下降沿或低电平
        break;
    case GPIO_INTMODE_DB_EDGE:
        LPC_GPIO->IS &= ~(1<<bit);      // 边沿触发
        LPC_GPIO->IBE |= (1<<bit);      // 双边沿触发
        //LPC_GPIO->IEV &= ~(1<<bit);   // 下降沿或低电平
        break;
    case GPIO_INTMODE_HIG_LEVEL:
        LPC_GPIO->IS |= (1<<bit);       // 电平触发
        LPC_GPIO->IBE &= ~(1<<bit);     // 无双边沿触发
        LPC_GPIO->IEV |= (1<<bit);      // 上升沿或高电平
        break;
    case GPIO_INTMODE_LOW_LEVEL:
        LPC_GPIO->IS |= (1<<bit);       // 电平触发
        LPC_GPIO->IBE &= ~(1<<bit);     // 无双边沿触发
        LPC_GPIO->IEV &= ~(1<<bit);     // 下降沿或低电平
        break;
    default:
        break;
    }
}

void gpio_enable_interrupt(LPC_GPIO0_Type *LPC_GPIO, uint32_t bit)
{
    LPC_GPIO->IE |= (1<<bit);
}

void gpio_clear_interrupt(LPC_GPIO0_Type *LPC_GPIO, uint32_t bit)
{
    LPC_GPIO->IC |= (1<<bit);
}

uint8_t gpio_get_intstatus(LPC_GPIO0_Type *LPC_GPIO, uint32_t bit)
{
    if(LPC_GPIO->MIS&(1<<bit)){
        return TRUE;
    }else{
        return FALSE;
    }
}

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


