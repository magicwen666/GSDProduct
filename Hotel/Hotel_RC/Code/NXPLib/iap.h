/****************************************Copyright (c)****************************************************
**                            Guangzhou ZHIYUAN electronics Co.,LTD.
**
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           IAP.h
** Last modified Date:  2010-02-04
** Last Version:        V1.0
** Descriptions:        IAP升级-IAP函数
**
**--------------------------------------------------------------------------------------------------------
** Created by:          Lanwuqiang
** Created date:        2010-02-05
** Version:             V1.0
** Descriptions:        添加用户应用程序
**
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
** Rechecked by:        
*********************************************************************************************************/
#ifndef __IAP_H
#define __IAP_H

#include <LPC112x.h>
#define INT32U    uint32_t
#define INT16U    uint16_t
#define INT8U     uint8_t

/* 
 *  定义CCLK值大小，单位为KHz 
 */
#define  IAP_FCCLK            (48000)
#define  IAP_ENTER_ADR        0x1FFF1FF1                                /* IAP入口地址定义              */


/* 
 *  定义IAP命令字
 */
#define     IAP_Prepare                                50               /* 选择扇区                     */
                                                                        /* 【起始扇区号、结束扇区号】   */
#define     IAP_RAMTOFLASH                             51               /* 拷贝数据 FLASH目标地址       */
                                                                        /* RAM源地址    【、写入字节数  */
                                                                        /* 系统时钟频率】               */
#define     IAP_ERASESECTOR                            52               /* 擦除扇区    【起始扇区号     */
                                                                        /* 结束扇区号、系统时钟频率】   */
#define     IAP_BLANKCHK                               53               /* 查空扇区    【起始扇区号、   */
                                                                        /* 结束扇区号】                 */
#define     IAP_READPARTID                             54               /* 读器件ID    【无】           */
#define     IAP_BOOTCODEID                             55               /* 读Boot版本号【无】           */
#define     IAP_COMPARE                                56               /* 比较命令    【Flash起始地址  */
                                                                        /* RAM起始地址、需要比较的      */
                                                                        /* 字节数】                     */
/*
 *  定义IAP返回状态字
 */
#define     CMD_SUCCESS                                0
#define     INVALID_COMMAND                            1
#define     SRC_ADDR_ERROR                             2
#define     DST_ADDR_ERROR                             3
#define     SRC_ADDR_NOT_MAPPED                        4
#define     DST_ADDR_NOT_MAPPED                        5
#define     COUNT_ERROR                                6
#define     INVALID_SECTOR                             7
#define     SECTOR_NOT_BLANK                           8
#define     SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION    9
#define     COMPARE_ERROR                              10
#define     BUSY                                       11

extern void (*IAP_Entry) (INT32U param_tab[], INT32U result_tab[]);

/*********************************************************************************************************
** Function name:       sectorPrepare
** Descriptions:        IAP操作扇区选择，命令代码50
** input parameters:    sec1:           起始扇区
**                      sec2:           终止扇区
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
extern INT32U  sectorPrepare (INT8U sec1, INT8U sec2);

/*********************************************************************************************************
** Function name:       ramCopy
** Descriptions:        复制RAM的数据到FLASH，命令代码51
** input parameters:    dst:            目标地址，即FLASH起始地址。以512字节为分界
**                      src:            源地址，即RAM地址。地址必须字对齐
**                      no:             复制字节个数，为512/1024/4096/8192
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
extern INT32U  ramCopy(INT32U dst, INT32U src, INT32U no);

/*********************************************************************************************************
** Function name:       sectorErase
** Descriptions:        扇区擦除，命令代码52
** input parameters:    sec1            起始扇区
**                      sec2            终止扇区92
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
extern INT32U  sectorErase (INT8U sec1, INT8U sec2);

/*********************************************************************************************************
** Function name:       blankChk
** Descriptions:        扇区查空，命令代码53
** input parameters:    sec1:           起始扇区
**                      sec2:           终止扇区92
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
extern INT32U  blankChk (INT8U sec1, INT8U sec2);

/*********************************************************************************************************
** Function name:       parIdRead
** Descriptions:        扇区查空，命令代码54
** input parameters:    无
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
extern  INT32U  parIdRead (void);

/*********************************************************************************************************
** Function name:       codeIdBoot
** Descriptions:        扇区查空，命令代码55
** input parameters:    无
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
extern INT32U  codeIdBoot (void);

/*********************************************************************************************************
** Function name:       dataCompare
** Descriptions:        校验数据，命令代码56
** input parameters:    dst:            目标地址，即RAM/FLASH起始地址。地址必须字对齐
**                      src:            源地址，即FLASH/RAM地址。地址必须字对齐
**                      no:             复制字节个数，必须能被4整除
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
extern INT32U  dataCompare (INT32U dst, INT32U src, INT32U no);

#endif                                                                  /*  _IAP_H_                     */

/*********************************************************************************************************
  End Of File
*********************************************************************************************************/

