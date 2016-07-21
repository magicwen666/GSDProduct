/*******************************************************************
Copyright (C):    GAKATO. Co., Ltd.
File name    :    W25qFlash.c
DESCRIPTION  :    
AUTHOR       :    HZW
Version      :    1.0
Date         :    2015/9/22
Others       :
History      :
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
1) Date: 2015/9/22        Author: HZW
content:

*******************************************************************/

/** include files **/
#include "W25qFlash.h"
/** local definitions **/

/** default settings **/

/** external functions **/

/** external data **/

/** internal functions **/

/** private data **/

/** public data **/

/** private functions **/

/************************************************************
Function    : W25qFlashRWBytes
Description : Read and Write a bytes to flash
Input       : 
Output      : 
Return      : 
Others      : 
*************************************************************/
static unsigned char W25qFlashRWBytes(unsigned char w){

    unsigned char r = 0;
    unsigned char i;

    for (i = 0; i < 8; i++) {
        W25QFLASH_CLK(0);
        W25QFLASH_DI((w & 0x80) ? 1 : 0);
        w <<= 1;
        W25QFLASH_CLK(1);
        r <<= 1;
        r |= W25QFLASH_DO;
    }
    
    return r;
}


/************************************************************
Function    : W25qFlashWaitIdle
Description : wait for flash idle
Input       : 
Output      : 
Return      : 
Others      : 
*************************************************************/
static RetVal W25qFlashWaitIdle(void){

    volatile unsigned int delay = 65535;
    while ((--delay) && W25qFlashIsBusy());
    return (RetVal)((0 == delay) ? 1 : 0);
}

/************************************************************
Function    : W25qFlashWriteControl
Description : enable or disable to write
Input       : 
Output      : 
Return      : 
Others      : 
*************************************************************/
static void W25qFlashWriteControl(unsigned char en_or_dis){

    W25QFLASH_CS(0);
    W25qFlashRWBytes(en_or_dis);
    W25QFLASH_CS(1);  
}

/************************************************************
Function    : W25qFlashWriteAPage
Description : write data to a flash page
Input       : 
Output      : 
Return      : 
Others      : 
*************************************************************/
RetVal W25qFlashWriteAPage(void * buf,unsigned long addr,unsigned int len){

    unsigned char * pos = (unsigned char *)buf;

    if (COMMON_RETURN_TIMEOUT == W25qFlashWaitIdle()) {
        return COMMON_RETURN_TIMEOUT;
    }
    W25qFlashWriteControl(W25Q_FLASH_WriteEnable); 

    W25QFLASH_CS(0);
    W25qFlashRWBytes(W25Q_FLASH_PageProgram);
    W25qFlashRWBytes((unsigned char)(addr >> 16));
    W25qFlashRWBytes((unsigned char)(addr >> 8));
    W25qFlashRWBytes((unsigned char)(addr));
    
    while (len--) {
        W25qFlashRWBytes(*pos);
        pos++;
    }
    W25QFLASH_CS(1); 

    if (COMMON_RETURN_TIMEOUT == W25qFlashWaitIdle()) {
        return COMMON_RETURN_TIMEOUT;
    }

    return COMMON_RETURN_SUCCESS;
}


/** public functions **/

/************************************************************
Function    : W25qFlashInit
Description : flash interface init
Input       : 
Output      : 
Return      : 
Others      : 
*************************************************************/
unsigned char W25qFlashInit(void) {

    unsigned int id = 0x00;
    syscon_ahb_enable(SYSCON_CLK_IOCON);
    /* cs p2_2 */
    LPC_IOCON->PIO2_2  |= (IOCON_FUNC0 | IOCON_MODE_PULLUP);
    /* clk p1_0 */
    LPC_IOCON->R_PIO1_0|= (IOCON_FUNC1 | IOCON_MODE_PULLUP);
    /* miso p0_8 */
    LPC_IOCON->PIO0_8  |= (IOCON_FUNC0 | IOCON_MODE_PULLUP);
    /* mosi  p0_9 */
    LPC_IOCON->PIO0_9  |= (IOCON_FUNC0 | IOCON_MODE_PULLUP);
    
    syscon_ahb_enable(SYSCON_CLK_GPIO);
    gpio_set_dir(LPC_GPIO2, 2, GPIO_OUT);
    gpio_set_dir(LPC_GPIO1, 0, GPIO_OUT);
    gpio_set_dir(LPC_GPIO0, 8, GPIO_IN);
    gpio_set_dir(LPC_GPIO0, 9, GPIO_OUT);
    
    W25QFLASH_CS(0);

    id = W25qFlashReadID();
    if ((0x0000 == id) || (0xFFFF == id)) {
        return 1;
    }
    
    return 0;
}

/************************************************************
Function    : W25qFlashInit
Description : flash sleep or wake up control
Input       : 
Output      : 
Return      : 
Others      : 
*************************************************************/
void W25qFlashPowerControl(unsigned char on_or_off){

    W25QFLASH_CS(0);
    W25qFlashRWBytes(on_or_off);
    W25QFLASH_CS(1);
    
    return;
}

/************************************************************
Function    : W25qFlashReadID
Description : Read flash manufact device ID
Input       : 
Output      : 
Return      : 
Others      : 
*************************************************************/
unsigned short W25qFlashReadID(void){

    unsigned short ManufactureID = 0;
    
    W25QFLASH_CS(0);
    W25qFlashRWBytes(W25Q_FLASH_ManufactDeviceID);
    W25qFlashRWBytes(0x00);
    W25qFlashRWBytes(0x00);
    W25qFlashRWBytes(0x00);
    ManufactureID |= W25qFlashRWBytes(0x00) << 8;
    ManufactureID |= W25qFlashRWBytes(0x00);
    W25QFLASH_CS(1);
    
    return ManufactureID;
}

/************************************************************
Function    : W25qFlashIsBusy
Description : Is flash busy?
Input       : 
Output      : 
Return      : 
Others      : 
*************************************************************/
unsigned char W25qFlashIsBusy(void){

    unsigned char busy;

    W25QFLASH_CS(0);
    W25qFlashRWBytes(W25Q_FLASH_ReadStatusReg);
    busy = W25qFlashRWBytes(0xFF);
    W25QFLASH_CS(1);
    
    return busy & 0x01;
}

/************************************************************
Function    : W25qFlashEraseSector
Description : Erase a sector
Input       : sector
Output      : 
Return      : 
Others      : 
*************************************************************/
RetVal W25qFlashEraseSector(unsigned int sector){

    unsigned long addr = sector;
    addr <<= 12;

    if (COMMON_RETURN_TIMEOUT == W25qFlashWaitIdle()) {
        return COMMON_RETURN_TIMEOUT;
    }

    W25qFlashWriteControl(W25Q_FLASH_WriteEnable); 
    W25QFLASH_CS(0);
    W25qFlashRWBytes(W25Q_FLASH_SectorErase);
    W25qFlashRWBytes((unsigned char)(addr >> 16));
    W25qFlashRWBytes((unsigned char)(addr >> 8));
    W25qFlashRWBytes((unsigned char)(addr));
    W25QFLASH_CS(1);

    if (COMMON_RETURN_TIMEOUT == W25qFlashWaitIdle()) {
        return COMMON_RETURN_TIMEOUT;
    }

    return COMMON_RETURN_SUCCESS;
}

/************************************************************
Function    : W25qFlashEraseChip
Description : Erase a sector
Input       : sector
Output      : 
Return      : 
Others      : 
*************************************************************/
RetVal W25qFlashEraseChip(void){

    if (COMMON_RETURN_TIMEOUT == W25qFlashWaitIdle()) {
        return COMMON_RETURN_TIMEOUT;
    }
    W25qFlashWriteControl(W25Q_FLASH_WriteEnable); 

    W25QFLASH_CS(0);
    W25qFlashRWBytes(W25Q_FLASH_ChipErase);
    W25QFLASH_CS(1);
    
    if (COMMON_RETURN_TIMEOUT == W25qFlashWaitIdle()) {
        return COMMON_RETURN_TIMEOUT;
    }
    
    return COMMON_RETURN_SUCCESS;
}

/************************************************************
Function    : W25qFlashReadData
Description : Read data from flash
Input       : 
Output      : 
Return      : 
Others      : 
*************************************************************/
RetVal W25qFlashReadData(void * buf,unsigned long addr,unsigned int len){

    unsigned char * pos = (unsigned char *)buf;
    if (COMMON_RETURN_TIMEOUT == W25qFlashWaitIdle()) {
        return COMMON_RETURN_TIMEOUT;
    }
    W25QFLASH_CS(0);
    W25qFlashRWBytes(W25Q_FLASH_ReadData);
    W25qFlashRWBytes((unsigned char)(addr >> 16));
    W25qFlashRWBytes((unsigned char)(addr >> 8));
    W25qFlashRWBytes((unsigned char)(addr));
    while (len--) {
        *pos = W25qFlashRWBytes(0xFF);
        pos++;
    }
    W25QFLASH_CS(1);

    if (COMMON_RETURN_TIMEOUT == W25qFlashWaitIdle()) {
        return COMMON_RETURN_TIMEOUT;
    }
    return COMMON_RETURN_SUCCESS;
}

/************************************************************
Function    : W25qFlashWriteData
Description : write data to a flash
Input       : 
Output      : 
Return      : 
Others      : 
*************************************************************/
RetVal W25qFlashWriteData(void * buf,unsigned long addr,unsigned int len){

    unsigned char * pos = (unsigned char *)buf;
    unsigned int writelen = 0;
    RetVal ret;

    while (1) {
        if ((addr/256) != ((addr + len)/256)) {
            /* not in the same page */
            writelen = 256 - (addr%256);
        }else{
            writelen = len;
        }
        ret = W25qFlashWriteAPage(pos,addr,writelen);
        if (COMMON_RETURN_SUCCESS != ret) {
            return ret;
        }
        pos += writelen;
        addr += writelen;
        len -= writelen;
        if (0 == len) {
            break;
        }
    }
    return COMMON_RETURN_SUCCESS;
}

/************************************************************
Function    : W25qFlashCopy
Description : write data to a flash
Input       : copy a data between different sector
Output      : 
Return      : 
Others      : 
*************************************************************/
RetVal W25qFlashCopy(unsigned int srcaddr,unsigned int dstaddr,unsigned int len){

    unsigned char page[256];
    unsigned int writelen = 0;
    RetVal ret;

    if (0 == len) {
        return COMMON_RETURN_ARGERR;
    }

    if ((srcaddr & 0xFFFFF000) == (dstaddr & 0xFFFFF000)) {
        /* in the same sector */
        return COMMON_RETURN_ARGERR;
    }
    
    while (1) {
        if ((dstaddr/256) != ((dstaddr + len)/256)) {
            /* not in the same page */
            writelen = 256 - (dstaddr%256);
        }else{
            writelen = len;
        }
        W25qFlashReadData(page,srcaddr,writelen);
        ret = W25qFlashWriteAPage(page,dstaddr,writelen);
        if (COMMON_RETURN_SUCCESS != ret) {
            return ret;
        }
        srcaddr += writelen;
        dstaddr += writelen;
        len -= writelen;
        if (0 == len) {
            break;
        }
    }
    return COMMON_RETURN_SUCCESS;
}


