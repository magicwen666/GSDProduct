#ifndef __IOCON_H__
#define __IOCON_H__

#include "chip.h"

// LPC1114FBD48/333
#define IOCON_FUNC0             (0x00)
#define IOCON_FUNC1             (0x01)
#define IOCON_FUNC2             (0x02)
#define IOCON_FUNC3             (0x03)
#define IOCON_FUNC4             (0x04)
#define IOCON_FUNC5             (0x05)
#define IOCON_FUNC6             (0x06)
#define IOCON_FUNC7             (0x07)

#define IOCON_MODE_INACT        (0x0 << 3)		/*!< No addition pin function */
#define IOCON_MODE_PULLDOWN     (0x1 << 3)		/*!< Selects pull-down function */
#define IOCON_MODE_PULLUP       (0x2 << 3)		/*!< Selects pull-up function */
#define IOCON_MODE_REPEATER     (0x3 << 3)		/*!< Selects pin repeater function */

#define IOCON_HYS_DIS           (0x0 << 5)
#define IOCON_HYS_EN            (0x1 << 5)		/*!< Enables hysteresis */

#define IOCON_INV_DIS           (0x0 << 6)
#define IOCON_INV_EN            (0x1 << 6)		/*!< Enables invert function on input */

#define IOCON_ADMODE_EN         (0x0 << 7)		/*!< Enables analog input function (analog pins only) */
#define IOCON_DIGMODE_EN        (0x1 << 7)		/*!< Enables digital function (analog pins only) */

#define IOCON_I2CMODE_SFI2C     (0x0 << 8)		/*!< I2C standard mode/fast-mode */
#define IOCON_I2CMODE_STDIO    (0x1 << 8)		/*!< I2C standard I/O functionality */
#define IOCON_I2CMODE_FASTI2C   (0x2 << 8)		/*!< I2C Fast-mode Plus */

#define IOCON_FILT_DIS          (0x1 << 8)		/*!< Disables noise pulses filtering (10nS glitch filter) */

#define IOCON_OPENDRAIN_DIS     (0x0 << 10)
#define IOCON_OPENDRAIN_EN      (0x1 << 10)		/*!< Enables open-drain function */

// SSP1_MISO_LOC
#define SSP1_MISO_LOC_PIO2_2    (0x00)      // Selects SSP1_MISO function in pin location PIO2_2.
#define SSP1_MISO_LOC_PIO1_10   (0x01)      // Selects SSP1_MISO function in pin location PIO1_10.

// SSP1_MOSI_LOC
#define SSP1_MOSI_LOC_PIO2_3    (0x00)      // Selects SSP1_MOSI1 function in pin location PIO2_3.
#define SSP1_MOSI_LOC_PIO1_9    (0x01)      // Selects SSP1_MOSI function in pin location PIO1_9.

// CT32B0_CAP0_LOC
#define CT32B0_CAP0_LOC_PIO1_5  (0x00)      // Selects CT32B0_CAP0 function in pin location PIO1_5.
#define CT32B0_CAP0_LOC_PIO2_9  (0x01)      // Selects CT32B0_CAP0 function in pin location PIO2_9.

// U0_RXD_LOC
#define U0_RXD_LOC_PIO1_6       (0x00)      // Selects U0_RXD function in pin location PIO1_6.
#define U0_RXD_LOC_PIO2_7       (0x01)      // Selects RXD function in pin location PIO2_7.
#define U0_RXD_LOC_PIO3_4       (0x03)      // Selects RXD function in pin location PIO3_4.

// º¯Êý
void iocon_u0_rxd_loc(uint32_t u0_rxd_loc);

#endif

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


