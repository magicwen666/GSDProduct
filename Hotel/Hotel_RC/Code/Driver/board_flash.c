#include "IAP.h"
#include <string.h>

void FlashWriteData(void *buf, unsigned long addr, unsigned char len) {
    
    unsigned char sector;
    unsigned char write[256] = {0x00};
     
    memcpy(write, buf, len);
    sector = addr / 4096;
    __disable_irq();                                                                                                    
    sectorPrepare(sector, sector);                                    
    sectorErase(sector, sector);
    sectorPrepare(sector, sector); 
    ramCopy(addr, (INT32U)write, 256);                             
    __enable_irq();  
    
}

void FlashReadData(unsigned char *buf,unsigned long addr,unsigned int len) {
    
    unsigned char *pos = (unsigned char *)addr;
    
    if (NULL == buf) {
        return ;
    }
    
    while (len--) {
        *buf++ = *pos++;  
    }        
}

void FlashErase(unsigned long addr) {

    unsigned char sector = 0;

    sector = addr / 4096; 
     __disable_irq();                                                                                                    
    sectorPrepare(sector, sector);                                    
    sectorErase(sector, sector);                                                                                                    
    __enable_irq();  
}

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


