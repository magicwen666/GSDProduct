/****************************************Copyright (c)****************************************************
**                            Guangzhou ZHIYUAN electronics Co.,LTD.
**
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  I2CINT.c
** Latest modified Date:       2009-6-8
** Latest Version:
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chenmingji
** Created date:               2009-6-8
** Version:
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Modified by:                Zhangrong
** Modified date:              2009-6-8
** Version:
** Descriptions:               
*********************************************************************************************************/

#include "I2CINT.h"

                                                                        /* 定义用于和I2C中断传递信息的  */
                                                                        /* 全局变量                     */
volatile INT8U     I2C_sla;                                             /* I2C器件从地址                */
volatile INT32U    I2C_suba;                                            /* I2C器件内部子地址            */
volatile INT8U     I2C_suba_num;                                        /* I2C子地址字节数              */
volatile INT8U     *I2C_buf;                                            /* 数据缓冲区指针               */
volatile INT32U    I2C_num;                                             /* 要读取/写入的数据个数        */
volatile INT8U     I2C_end;                                             /* I2C总线结束标志：结束总线是  */
                                                                        /* 置1                          */
volatile INT8U     I2C_suba_en;         /*  子地址控制。
                                         *  0--子地址已经处理或者不需要子地址
                                         *  1--读取操作
                                         *  2--写操作
                                         */

/*********************************************************************************************************
** Function name:           Wait_I2c_End
** Descriptions:            软件延时，提供给I2C总线等待超时使用
** input parameters:        dly        延时参数，值越大，延时越久
** Output parameters:       NONE
** Returned value:          NONE
*********************************************************************************************************/
INT8U  Wait_I2c_End(INT32U  dly)
{  
    INT32U  i;

    if( I2C_end==1 ){     
        return (TRUE);
    }
    for(; dly>0; dly--){
        for(i=0; i<5000; i++)
        {
            if( I2C_end==1 ){ 
                return (TRUE);
            }
        }
    }
    return (FALSE);
}

/*********************************************************************************************************
** Function name:       ISendByte
** Descriptions:        向无子地址器件发送1字节数据。
** input parameters:    sla     器件地址
**                      dat     要发送的数据
** Output parameters:   返回值为0时表示出错，为1时表示操作正确。
** Returned value:      NONE
*********************************************************************************************************/
INT8U  ISendByte(INT8U sla, INT8U dat)
{                                                                      /* 参数设置                     */
    I2C_sla     = sla;                                                 /* 写操作的器件地址             */
    I2C_buf     = &dat;                                                /* 待发送的数据                 */
    I2C_num     = 1;                                                   /* 发送1字节数据                */
    I2C_suba_en = 0;                                                   /* 无子地址                     */
    I2C_end     = 0;

    LPC_I2C->CONCLR = 0x2C;
    LPC_I2C->CONSET = 0x60;                                            /* 设置为主机，并启动总线       */

    return( Wait_I2c_End(20));

}
/*********************************************************************************************************
** Function name:       IRcvByte
** Descriptions:        向无子地址器件读取1字节数据。
** input parameters:    sla     器件地址
**                      dat     要发送的数据
** Output parameters:   返回值为0时表示出错，为1时表示操作正确。
** Returned value:      NONE
*********************************************************************************************************/
INT8U  IRcvByte(INT8U sla, INT8U *dat)
{                                                                      /* 参数设置                     */
    I2C_sla     = sla+1;                                               /* 读操作的器件地址             */
    I2C_buf     = dat;
    I2C_num     = 1;
    I2C_suba_en = 0;                                                   /* 无子地址                     */
    I2C_end     = 0;

    LPC_I2C->CONCLR = 0x2C;
    LPC_I2C->CONSET = 0x60;                                            /* 设置为主机，并启动总线       */

    return( Wait_I2c_End(20));

}
/*********************************************************************************************************
** Function name:       I2C_ReadNByte
** Descriptions:        从有子地址器件任意地址开始读取N字节数据
** input parameters:    sla         器件地址
**                      suba_type   子地址结构    1－单字节地址    2－8+X结构    2－双字节地址
**                      suba        器件子地址
**                      s           数据接收缓冲区指针
**                      num         读取的个数
** Output parameters:   返回值为0时表示出错，为1时表示操作正确。
** Returned value:      NONE
*********************************************************************************************************/
INT8U I2C_ReadNByte (INT8U sla, INT8U suba_type, INT32U suba, INT8U *s, INT32U num)
{
    if (num > 0)                                                        /* 判断num个数的合法性          */
    {                                                                   /* 参数设置                     */
        if (suba_type == 1)
        {                                                               /* 子地址为单字节               */
            I2C_sla         = sla + 1;                                  /* 读器件的从地址，R=1          */
            I2C_suba        = suba;                                     /* 器件子地址                   */
            I2C_suba_num    = 1;                                        /* 器件子地址为1字节            */
        }
        if (suba_type == 2)
        {                                                               /* 子地址为2字节                */
            I2C_sla         = sla + 1;                                  /* 读器件的从地址，R=1          */
            I2C_suba        = suba;                                     /* 器件子地址                   */
            I2C_suba_num    = 2;                                        /* 器件子地址为2字节            */
        }
        if (suba_type == 3)
        {                                                               /* 子地址结构为8+X              */
            I2C_sla         = sla + ((suba >> 7 )& 0x0e) + 1;           /* 读器件的从地址，R=1          */
            I2C_suba        = suba & 0x0ff;                             /* 器件子地址                   */
            I2C_suba_num    = 1;                                        /* 器件子地址为8+x              */
        }
        I2C_buf     = s;                                                /* 数据接收缓冲区指针           */
        I2C_num     = num;                                              /* 要读取的个数                 */
        I2C_suba_en = 1;                                                /* 有子地址读                   */
        I2C_end     = 0;

                                                                        /* 清除STA,SI,AA标志位          */
        LPC_I2C->CONCLR = (1 << 2)|                                     /* AA                           */
                          (1 << 3)|                                     /* SI                           */
                          (1 << 5);                                     /* STA                          */

                                                                        /* 置位STA,启动I2C总线          */
        LPC_I2C->CONSET = (1 << 5)|                                     /* STA                          */
                          (1 << 6);                                     /* I2CEN                        */
                                                             
                                                                        /* 等待I2C操作完成              */
        return( Wait_I2c_End(20));

    }
    return (FALSE);
}
/*********************************************************************************************************
** Function name:       I2C_WriteNByte
** Descriptions:        向有子地址器件写入N字节数据
** input parameters:    sla         器件地址
**                      suba_type   子地址结构    1－单字节地址    2－8+X结构    2－双字节地址
**                      suba        器件子地址
**                      s           将要写入的数据的指针
**                      num         将要写入的数据的个数
** Output parameters:   返回值为0时表示出错，为1时表示操作正确。
** Returned value:      NONE
*********************************************************************************************************/
INT8U I2C_WriteNByte(INT8U sla, INT8U suba_type, INT32U suba, INT8U *s, INT32U num)
{
    if (num > 0)                                                        /* 如果读取的个数为0，则返回错  */
                                                                        /* 误                           */
    {                                                                   /* 设置参数                     */
        if (suba_type == 1)
        {                                                               /* 子地址为单字节               */
            I2C_sla         = sla;                                      /* 读器件的从地址               */
            I2C_suba        = suba;                                     /* 器件子地址                   */
            I2C_suba_num    = 1;                                        /* 器件子地址为1字节            */
        }
        if (suba_type == 2)
        {                                                               /* 子地址为2字节                */
            I2C_sla         = sla;                                      /* 读器件的从地址               */
            I2C_suba        = suba;                                     /* 器件子地址                   */
            I2C_suba_num    = 2;                                        /* 器件子地址为2字节            */
        }
        if (suba_type == 3)
        {                                                               /* 子地址结构为8+X              */
            I2C_sla         = sla + ((suba >> 7 )& 0x0e);               /* 读器件的从地址               */
            I2C_suba        = suba & 0x0ff;                             /* 器件子地址                   */
            I2C_suba_num    = 1;                                        /* 器件子地址为8+X              */
        }

        I2C_buf     = s;                                                /* 数据                         */
        I2C_num     = num;                                              /* 数据个数                     */
        I2C_suba_en = 2;                                                /* 有子地址，写操作             */
        I2C_end     = 0;

                                                                        /* 清除STA,SI,AA标志位          */
        LPC_I2C->CONCLR = (1 << 2)|                                     /* AA                           */
                          (1 << 3)|                                     /* SI                           */
                          (1 << 5);                                     /* STA                          */

                                                                        /* 置位STA,启动I2C总线          */
        LPC_I2C->CONSET = (1 << 5)|                                     /* STA                          */
                          (1 << 6);                                     /* I2CEN                        */

                                                                        /* 等待I2C操作完成              */
        return( Wait_I2c_End(20));
    }
    return (FALSE);
}
/*********************************************************************************************************
** Function name:       I2C_IRQHandler
** Descriptions:        硬件I2C中断服务程序。
** input parameters:    无
** Output parameters:   无
** Returned value:      注意处理子地址为2字节的情况。
7       6       5       4       3       2       1       0
-       I2EN    STA     STO     SI      AA      -       -   
8       4       2       1       8       4       2       1
*********************************************************************************************************/
void  I2C_IRQHandler(void)
{   /*
     *读取I2C状态寄存器I2DAT,按照全局变量的设置进行操作及设置软件标志,清除中断逻辑,中断返回
     */
    switch (LPC_I2C->STAT & 0xF8)
    {   /* 根据状态码进行相应的处理     */
        case 0x08:    /* 已发送起始条件,主发送和主接收都有,装入SLA+W或者SLA+R */
        if(I2C_suba_en == 1) {/* SLA+R */                               /* 指定子地址读                 */
           
             LPC_I2C->DAT = I2C_sla & 0xFE;                             /* 先写入地址                   */
        } else {
                                                                        /* SLA+W                        */
             LPC_I2C->DAT = I2C_sla;                                    /* 否则直接发送从机地址         */
        }
                                                                        /* 清零SI位                     */
        LPC_I2C->CONCLR = (1 << 3)|                                     /* SI                           */
                          (1 << 5);                                     /* STA                          */
        break;

        case 0x10:    /*已发送重复起始条件 */                           /* 主发送和主接收都有           */
                                                                        /* 装入SLA+W或者SLA+R           */
            LPC_I2C->DAT = I2C_sla;                                     /* 重起总线后，重发从地址       */
            LPC_I2C->CONCLR = 0x28;                                     /* 清零SI,STA                   */
        break;

        case 0x18:
        case 0x28:                                                      /* 已发送I2DAT中的数据,已接收ACK*/
        if (I2C_suba_en == 0)
        {
            if (I2C_num > 0) {
                LPC_I2C->DAT = *I2C_buf++;              
                LPC_I2C->CONCLR = 0x28;                                 /* 清零SI,STA                   */
                I2C_num--;
            }  else {
                                                                        /* 没有数据发送了               */
                                                                        /* 停止总线                     */
                LPC_I2C->CONSET = (1 << 4);                             /* STO                          */
                LPC_I2C->CONCLR = 0x28;                                 /* 清零SI,STA                   */
                I2C_end = 1;                                            /* 总线已经停止                 */
            }
        }

        if(I2C_suba_en == 1)                                            /* 若是指定地址读,则重新启动总线*/
        {
            if (I2C_suba_num == 2) {
                LPC_I2C->DAT = ((I2C_suba >> 8) & 0xff);
                LPC_I2C->CONCLR = 0x28;                                 /* 清零SI,STA                   */
                I2C_suba_num--;
                break;
            }

            if(I2C_suba_num == 1) {
                LPC_I2C->DAT = (I2C_suba & 0xff);
                LPC_I2C->CONCLR = 0x28;                                 /* 清零SI,STA                   */
                I2C_suba_num--;
                break;
            }

            if (I2C_suba_num == 0){
                LPC_I2C->CONCLR = 0x08;
                LPC_I2C->CONSET = 0x20;
                I2C_suba_en = 0;                                        /* 子地址己处理                 */
                break;
            }
        }

        if (I2C_suba_en == 2)                                           /* 指定子地址写,子地址尚未指定, */
                                                                        /* 则发送子地址                 */
        {
            if (I2C_suba_num > 0) {
                if (I2C_suba_num == 2) {
                    LPC_I2C->DAT = ((I2C_suba >> 8) & 0xff);
                    LPC_I2C->CONCLR = 0x28;
                    I2C_suba_num--;
                    break;
                }
                if (I2C_suba_num == 1) {
                    LPC_I2C->DAT    = (I2C_suba & 0xff);
                    LPC_I2C->CONCLR = 0x28;
                    I2C_suba_num--;
                    I2C_suba_en  = 0;
                    break;
                }
            }
        }
        break;

        case 0x40:                                                      /* 已发送SLA+R,已接收ACK        */
        if (I2C_num <= 1) {                                             /* 如果是最后一个字节           */
            LPC_I2C->CONCLR = 1 << 2;                                   /* 下次发送非应答信号           */
        } else {
            LPC_I2C->CONSET = 1 << 2;                                   /* 下次发送应答信号             */
        }
        LPC_I2C->CONCLR = 0x28;                                         /* 清零SI,STA                   */
        break;

        case 0x20:                                                      /* 已发送SLA+W,已接收非应答     */
        case 0x30:                                                      /* 已发送I2DAT中的数据，已接收  */
                                                                        /* 非应答                       */
        case 0x38:                                                      /* 在SLA+R/W或数据字节中丢失仲  */
                                                                        /* 裁                           */
        case 0x48:                                                      /* 已发送SLA+R,已接收非应答     */
        LPC_I2C->CONCLR = 0x28;
        I2C_end = 0xFF;
        break;

        case 0x50:                                                      /* 已接收数据字节，已返回ACK    */
        *I2C_buf++ = LPC_I2C->DAT;
        I2C_num--;
        if (I2C_num == 1) {                                             /* 接收最后一个字节             */
           LPC_I2C->CONCLR = 0x2C;                                      /* STA,SI,AA = 0                */
        } else {
        
            LPC_I2C->CONSET = 0x04;                                     /* AA=1                         */
            LPC_I2C->CONCLR = 0x28;
        }
        break;

        case 0x58:                                                      /* 已接收数据字节，已返回非应答 */
        *I2C_buf++ = LPC_I2C->DAT;                                      /* 读取最后一字节数据           */
        LPC_I2C->CONSET = 0x10;                                         /* 结束总线                     */
        LPC_I2C->CONCLR = 0x28;
        I2C_end = 1;
        break;

        default:
        break;
    }
}

/*********************************************************************************************************
** End Of File
*********************************************************************************************************/
