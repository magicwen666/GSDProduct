#ifndef __SPI_H__
#define __SPI_H__

#include "chip.h"

// CR0
#define SSP_CR0_DSS4        (0x03)
#define SSP_CR0_DSS5        (0x04)
#define SSP_CR0_DSS6        (0x05)
#define SSP_CR0_DSS7        (0x06)
#define SSP_CR0_DSS8        (0x07)
#define SSP_CR0_DSS9        (0x08)
#define SSP_CR0_DSS10       (0x09)
#define SSP_CR0_DSS11       (0x0A)
#define SSP_CR0_DSS12       (0x0B)
#define SSP_CR0_DSS13       (0x0C)
#define SSP_CR0_DSS14       (0x0D)
#define SSP_CR0_DSS15       (0x0E)
#define SSP_CR0_DSS16       (0x0F)

#define SSP_CR0_FRF_SPI     (0x00<<4)
#define SSP_CR0_FRF_TI      (0x01<<4)
#define SSP_CR0_Microwire   (0x02<<4)

#define SSP_CR0_CPOL_L      (0<<6)
#define SSP_CR0_CPOL_H      (1<<6)

#define SSP_CR0_CPHA_FIR    (0<<7)
#define SSP_CR0_CPHA_SEC    (1<<7)

// CR1
#define SSP_CR1_LBM_NORMAL  (0<<0)
#define SSP_CR1_LBM_LOOP    (1<<0)
#define SSP_CR1_SSE_DIS     (0<<1)
#define SSP_CR1_SSE_EN      (1<<1)
#define SSP_CR1_MS_MASTER   (0<<2)
#define SSP_CR1_MS_SLAVE    (1<<2)
#define SSP_CR1_SOD_DIS     (1<<3)

// SR
#define SSP_SR_TFE          (1<<0)
#define SSP_SR_TNF          (1<<1)
#define SSP_SR_RNE          (1<<2)
#define SSP_SR_RFF          (1<<3)
#define SSP_SR_BSY          (1<<4)

// IMSC
#define SSP_IMSC_RORIM      (1<<0)
#define SSP_IMSC_RTIM       (1<<1)
#define SSP_IMSC_RXIM       (1<<2)
#define SSP_IMSC_TXIM       (1<<3)

// RIS
#define SSP_RIS_RORRIS      (1<<0)
#define SSP_RIS_RTRIS       (1<<1)
#define SSP_RIS_RXRIS       (1<<2)
#define SSP_RIS_TXRIS       (1<<3)

// MIS
#define SSP_MIS_RORMIS      (1<<0)
#define SSP_MIS_RTMIS       (1<<1)
#define SSP_MIS_RXMIS       (1<<2)
#define SSP_MIS_TXMIS       (1<<3)

// ICR
#define SSP_ICR_RORIC       (1<<0)
#define SSP_ICR_RTIC        (1<<1)

#endif

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


