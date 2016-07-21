#ifndef __I2C_H__
#define __I2C_H__

#include "chip.h"
#include "I2CINT.h"

// 寄存器配置定义
// CONSET
#define I2C_CONSET_AA       (1<<2)
#define I2C_CONSET_SI       (1<<3)
#define I2C_CONSET_STO      (1<<4)
#define I2C_CONSET_STA      (1<<5)
#define I2C_CONSET_I2EN     (1<<6)


// CONCLR
#define I2C_CONCLR_AAC      (1<<2)
#define I2C_CONCLR_SIC      (1<<3)
#define I2C_CONCLR_STAC     (1<<5)
#define I2C_CONCLR_I2ENC    (1<<6)

// MMCTRL
#define I2C_MMCTRL_MM_ENA       (1<<0)
#define I2C_MMCTRL_ENA_SCL      (1<<1)
#define I2C_MMCTRL_MATCH_ALL    (1<<2)

// I2C功能配置定义
#define I2C_MODE_MASTER (0x00)
#define I2C_MODE_SLAVE  (0x01)

#define I2C_SPEED_STD   (100000)
#define I2C_SPEED_FAST  (400000)
#define I2C_SPEED_PLUS  (1000000)

void i2c_init(uint32_t clk);
uint8_t i2c_send_byte(uint8_t addr, uint8_t data);
uint8_t i2c_recv_byte(uint8_t addr, uint8_t *pdata);
uint8_t i2c_write_nbyte(uint8_t addr, 
                        uint8_t addr_type, 
                        uint32_t sub_addr, 
                        uint8_t *pdata, 
                        uint32_t num);
uint8_t i2c_read_nbyte( uint8_t addr, 
                        uint8_t addr_type, 
                        uint32_t sub_addr, 
                        uint8_t *pdata, 
                        uint32_t num);

#endif

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


