#include "chip.h"
#include "iocon.h"

void iocon_u0_rxd_loc(uint32_t u0_rxd_loc)
{
    LPC_IOCON->RXD_LOC = U0_RXD_LOC_PIO3_4;
}
/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


