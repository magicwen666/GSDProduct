#include "chip.h"

/*
typedef struct
{
  __IO uint32_t CR;                     // Offset: 0x000       A/D Control Register (R/W)
  __IO uint32_t GDR;                    // Offset: 0x004       A/D Global Data Register (R/W)
       uint32_t RESERVED0;
  __IO uint32_t INTEN;                  // Offset: 0x00C       A/D Interrupt Enable Register (R/W)
  __IO uint32_t DR[8];                  // Offset: 0x010-0x02C A/D Channel 0..7 Data Register (R/W)
  __I  uint32_t STAT;                   // Offset: 0x030       A/D Status Register (R/ )
} LPC_ADC_TypeDef;
*/

void adc_start(void)
{

}

void adc_init(void)
{
    syscon_ahb_enable(SYSCON_CLK_IOCON);

    syscon_ahb_enable(SYSCON_CLK_ADC);
    
}

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


