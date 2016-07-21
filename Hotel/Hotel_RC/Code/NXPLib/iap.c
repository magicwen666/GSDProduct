/****************************************Copyright (c)****************************************************
**                            Guangzhou ZHIYUAN electronics Co.,LTD.
**
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           IAP.c
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
#include "IAP.h"

/*
 *  定义函数指针  
 */
void (*IAP_Entry) (INT32U param_tab[], INT32U result_tab[]) = (void(*)())IAP_ENTER_ADR;

INT32U  paramin[8];                                                     /* IAP入口参数缓冲区            */
INT32U  paramout[8];                                                    /* IAP出口参数缓冲区            */

/*********************************************************************************************************
** Function name:       sectorPrepare
** Descriptions:        IAP操作扇区选择，命令代码50
** input parameters:    sec1:           起始扇区
**                      sec2:           终止扇区
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
INT32U  sectorPrepare (INT8U sec1, INT8U sec2)
{  
    paramin[0] = IAP_Prepare;                                           /* 设置命令字                   */
    paramin[1] = sec1;                                                  /* 设置参数                     */
    paramin[2] = sec2;                            
    (*IAP_Entry)(paramin, paramout);                                    /* 调用IAP服务程序              */
   
    return (paramout[0]);                                               /* 返回状态码                   */
}

/*********************************************************************************************************
** Function name:       ramCopy
** Descriptions:        复制RAM的数据到FLASH，命令代码51
** input parameters:    dst:            目标地址，即FLASH起始地址。以512字节为分界
**                      src:            源地址，即RAM地址。地址必须字对齐
**                      no:             复制字节个数，为512/1024/4096/8192
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
INT32U  ramCopy(INT32U dst, INT32U src, INT32U no)
{  
    paramin[0] = IAP_RAMTOFLASH;                                        /* 设置命令字                   */
    paramin[1] = dst;                                                   /* 设置参数                     */
    paramin[2] = src;
    paramin[3] = no;
    paramin[4] = IAP_FCCLK;
    (*IAP_Entry)(paramin, paramout);                                    /* 调用IAP服务程序              */
    
    return (paramout[0]);                                               /* 返回状态码                   */
}

/*********************************************************************************************************
** Function name:       sectorErase
** Descriptions:        扇区擦除，命令代码52
** input parameters:    sec1            起始扇区
**                      sec2            终止扇区92
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
INT32U  sectorErase (INT8U sec1, INT8U sec2)
{  
    paramin[0] = IAP_ERASESECTOR;                                       /* 设置命令字                   */
    paramin[1] = sec1;                                                  /* 设置参数                     */
    paramin[2] = sec2;
    paramin[3] = IAP_FCCLK;
    (*IAP_Entry)(paramin, paramout);                                    /* 调用IAP服务程序              */
   
    return (paramout[0]);                                               /* 返回状态码                   */
}

/*********************************************************************************************************
** Function name:       blankChk
** Descriptions:        扇区查空，命令代码53
** input parameters:    sec1:           起始扇区
**                      sec2:           终止扇区92
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
INT32U  blankChk (INT8U sec1, INT8U sec2)
{  
    paramin[0] = IAP_BLANKCHK;                                          /* 设置命令字                   */
    paramin[1] = sec1;                                                  /* 设置参数                     */
    paramin[2] = sec2;
    (*IAP_Entry)(paramin, paramout);                                    /* 调用IAP服务程序              */

    return (paramout[0]);                                               /* 返回状态码                   */
}

/*********************************************************************************************************
** Function name:       parIdRead
** Descriptions:        扇区查空，命令代码54
** input parameters:    无
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
INT32U  parIdRead (void)
{  
    paramin[0] = IAP_READPARTID;                                        /* 设置命令字                   */
    (*IAP_Entry)(paramin, paramout);                                    /* 调用IAP服务程序              */

    return (paramout[0]);                                               /* 返回状态码                   */
}

/*********************************************************************************************************
** Function name:       codeIdBoot
** Descriptions:        扇区查空，命令代码55
** input parameters:    无
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
INT32U  codeIdBoot (void)
{  
    paramin[0] = IAP_BOOTCODEID;                                        /* 设置命令字                   */
    (*IAP_Entry)(paramin, paramout);                                    /* 调用IAP服务程序              */

    return (paramout[0]);                                               /* 返回状态码                   */
}

/*********************************************************************************************************
** Function name:       dataCompare
** Descriptions:        校验数据，命令代码56
** input parameters:    dst:            目标地址，即RAM/FLASH起始地址。地址必须字对齐
**                      src:            源地址，即FLASH/RAM地址。地址必须字对齐
**                      no:             复制字节个数，必须能被4整除
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
INT32U  dataCompare (INT32U dst, INT32U src, INT32U no)
{  
    paramin[0] = IAP_COMPARE;                                           /* 设置命令字                   */
    paramin[1] = dst;                                                   /* 设置参数                     */
    paramin[2] = src;
    paramin[3] = no;
    (*IAP_Entry)(paramin, paramout);                                    /* 调用IAP服务程序              */

    return (paramout[0]);                                               /* 返回状态码                   */
}

/*********************************************************************************************************
  End Of File
*********************************************************************************************************/

