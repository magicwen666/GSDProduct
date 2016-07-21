#include "rf_time.h"


//CT16B0
void RF_TimerOn(uint16_t u16Time) {

	LPC_SYSCON->SYSAHBCLKCTRL  |= (1 << 7);	
    LPC_CT16B0->IR	            = 1;
	LPC_CT16B0->PR				= 3;				
	LPC_CT16B0->TC				= 0;				
	LPC_CT16B0->MCR				= MCR_MR0I | MCR_MR0R;			
	LPC_CT16B0->MR[0]   		= u16Time;
    LPC_CT16B0->TCR             = 0x01;
    
}

void RF_TimerOff(void) {
    
	LPC_CT16B0->TCR &= (~0x01);	
}

uint16_t RF_GetTimerCntValue(void) {
    
	return  (uint16_t)((LPC_CT16B0->TC) & 0x0000FFFF);
}

uint8_t RF_GetTimerFlag(void) {
    
    return ((LPC_CT16B0->IR) & 0x01);
}

void RF_ClrTimerFlag(void) {
    
	LPC_CT16B0->IR	= 1;
}



