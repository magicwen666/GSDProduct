/*******************************************************************
Copyright (C):    GAKATO. Co., Ltd.
File name    :    W25qFlash.h
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
#ifndef __W25QFLASH_H__
#define __W25QFLASH_H__
/** include files **/
#include "chip.h"

/* Configure check */

/*
                +++++++++++++++++++++++++++++++++++++++++++++++++++++ 
                +   Page == 256 bytes                               +
                +   Sector == 16Page == 4096 bytes == 4KBytes       +
                +   Block == 16Sector == 65536 bytes == 64KBytes    +
                +++++++++++++++++++++++++++++++++++++++++++++++++++++ 
*/
/** local definitions **/

/** default settings **/

/** external functions **/

/** external data **/

/** internal functions **/

/** private data **/
#define W25Q_FLASH_WriteEnable      0x06
#define W25Q_FLASH_WriteDisable     0x04
#define W25Q_FLASH_ReadStatusReg    0x05 
#define W25Q_FLASH_WriteStatusReg   0x01 
#define W25Q_FLASH_ReadData         0x03 
#define W25Q_FLASH_FastReadData     0x0B 
#define W25Q_FLASH_FastReadDual     0x3B 
#define W25Q_FLASH_PageProgram      0x02 
#define W25Q_FLASH_BlockErase       0xD8 
#define W25Q_FLASH_SectorErase      0x20 
#define W25Q_FLASH_ChipErase        0xC7 
#define W25Q_FLASH_PowerDown        0xB9 
#define W25Q_FLASH_ReleasePowerDown 0xAB 
#define W25Q_FLASH_DeviceID         0xAB 
#define W25Q_FLASH_ManufactDeviceID 0x90 
#define W25Q_FLASH_JedecDeviceID    0x9F 
/** public data **/


#define W25Q_FLASH_PAGE_SIZE        256
#define W25Q_FLASH_SECTOR_SIZE      (16 * W25Q_FLASH_PAGE_SIZE)
#define W25Q_FLASH_BLOCK_SIZE       (16 * W25Q_FLASH_SECTOR_SIZE)

#define PIN_OUTPUT      0
#define PIN_INPUT       1


#define W25QFLASH_CS(v)     gpio_set_bitval(LPC_GPIO2, 2, v)
#define W25QFLASH_CLK(v)    gpio_set_bitval(LPC_GPIO1, 0, v)      
#define W25QFLASH_DO        gpio_get_bitval(LPC_GPIO0, 8) 
#define W25QFLASH_DI(v)     gpio_set_bitval(LPC_GPIO0, 9, v)   

typedef enum {
    COMMON_RETURN_SUCCESS,
    COMMON_RETURN_TIMEOUT,
    COMMON_RETURN_ARGERR,
    COMMON_RETURN_FAILURE,
} RetVal;

/** private functions **/
static unsigned char W25qFlashRWBytes(unsigned char w);
static void W25qFlashWriteStatusReg(unsigned char sr);
static void W25qFlashWriteControl(unsigned char en_or_dis);
/** public functions **/
unsigned char  W25qFlashInit(void);
unsigned char  W25qFlashIsBusy(void);
unsigned short W25qFlashReadID(void);
unsigned char  W25qFlashEraseChip(void);
RetVal W25qFlashReadData(void * buf,unsigned long addr,unsigned int len);
RetVal W25qFlashWriteData(void * buf,unsigned long addr,unsigned int len);
RetVal W25qFlashCopy(unsigned int srcaddr,unsigned int dstaddr,unsigned int len);
RetVal W25qFlashEraseSector(unsigned int sector);
#endif

