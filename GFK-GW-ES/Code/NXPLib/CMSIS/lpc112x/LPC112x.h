
/****************************************************************************************************//**
 * @file     LPC112x.h
 *
 * @brief    CMSIS Cortex-M0 Peripheral Access Layer Header File for
 *           LPC112x from .
 *
 * @version  V0.3
 * @date     19. January 2015
 *
 * @note     Generated with SVDConv V2.84c 
 *           from CMSIS SVD File 'LPC112x.svd' Version 0.3,
 *******************************************************************************************************/



/** @addtogroup (null)
  * @{
  */

/** @addtogroup LPC112x
  * @{
  */

#ifndef LPC112X_H
#define LPC112X_H

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum {
/* -------------------  Cortex-M0 Processor Exceptions Numbers  ------------------- */
  Reset_IRQn                    = -15,              /*!<   1  Reset Vector, invoked on Power up and warm reset                 */
  NonMaskableInt_IRQn           = -14,              /*!<   2  Non maskable Interrupt, cannot be stopped or preempted           */
  HardFault_IRQn                = -13,              /*!<   3  Hard Fault, all classes of Fault                                 */
  SVCall_IRQn                   =  -5,              /*!<  11  System Service Call via SVC instruction                          */
  DebugMonitor_IRQn             =  -4,              /*!<  12  Debug Monitor                                                    */
  SysTick_IRQn                  =  -1,              /*!<  15  System Tick Timer                                                */
/* ---------------------  LPC112x Specific Interrupt Numbers  --------------------- */
  PIO0_0_IRQn                   =   0,              /*!<   0  PIO0_0                                                           */
  PIO0_1_IRQn                   =   1,              /*!<   1  PIO0_1                                                           */
  PIO0_2_IRQn                   =   2,              /*!<   2  PIO0_2                                                           */
  PIO0_3_IRQn                   =   3,              /*!<   3  PIO0_3                                                           */
  PIO0_4_IRQn                   =   4,              /*!<   4  PIO0_4                                                           */
  PIO0_5_IRQn                   =   5,              /*!<   5  PIO0_5                                                           */
  PIO0_6_IRQn                   =   6,              /*!<   6  PIO0_6                                                           */
  PIO0_7_IRQn                   =   7,              /*!<   7  PIO0_7                                                           */
  PIO0_8_IRQn                   =   8,              /*!<   8  PIO0_8                                                           */
  PIO0_9_IRQn                   =   9,              /*!<   9  PIO0_9                                                           */
  PIO0_10_IRQn                  =  10,              /*!<  10  PIO0_10                                                          */
  PIO0_11_IRQn                  =  11,              /*!<  11  PIO0_11                                                          */
  PIO1_0_IRQn                   =  12,              /*!<  12  PIO1_0                                                           */
  ADC_B_IRQn                    =  13,              /*!<  13  ADC_B                                                            */
  SSP1_IRQn                     =  14,              /*!<  14  SSP1                                                             */
  I2C_IRQn                      =  15,              /*!<  15  I2C                                                              */
  CT16B0_IRQn                   =  16,              /*!<  16  CT16B0                                                           */
  CT16B1_IRQn                   =  17,              /*!<  17  CT16B1                                                           */
  CT32B0_IRQn                   =  18,              /*!<  18  CT32B0                                                           */
  CT32B1_IRQn                   =  19,              /*!<  19  CT32B1                                                           */
  SSP0_IRQn                     =  20,              /*!<  20  SSP0                                                             */
  UART0_IRQn                    =  21,              /*!<  21  UART0                                                            */
  UART1_IRQn                    =  22,              /*!<  22  UART1                                                            */
  UART2_IRQn                    =  23,              /*!<  23  UART2                                                            */
  ADC_A_IRQn                    =  24,              /*!<  24  ADC_A                                                            */
  WDT_IRQn                      =  25,              /*!<  25  WDT                                                              */
  BOD_IRQn                      =  26,              /*!<  26  BOD                                                              */
  FLASH_IRQn                    =  27,              /*!<  27  FLASH                                                            */
  GPIO3_IRQn                    =  28,              /*!<  28  GPIO3                                                            */
  GPIO2_IRQn                    =  29,              /*!<  29  GPIO2                                                            */
  GPIO1_IRQn                    =  30,              /*!<  30  GPIO1                                                            */
  GPIO0_IRQn                    =  31               /*!<  31  GPIO0                                                            */
} IRQn_Type;


/** @addtogroup Configuration_of_CMSIS
  * @{
  */


/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* ----------------Configuration of the Cortex-M0 Processor and Core Peripherals---------------- */
#define __CM0_REV                 0x0000            /*!< Cortex-M0 Core Revision                                               */
#define __MPU_PRESENT                  0            /*!< MPU present or not                                                    */
#define __NVIC_PRIO_BITS               2            /*!< Number of Bits used for Priority Levels                               */
#define __Vendor_SysTickConfig         0            /*!< Set to 1 if different SysTick Config is used                          */
/** @} */ /* End of group Configuration_of_CMSIS */

#include "core_cm0.h"                               /*!< Cortex-M0 processor and core peripherals                              */
#include "system_LPC112x.h"                         /*!< LPC112x System                                                        */


/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */


/** @addtogroup Device_Peripheral_Registers
  * @{
  */


/* -------------------  Start of section using anonymous unions  ------------------ */
#if defined(__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined(__ICCARM__)
  #pragma language=extended
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__TMS470__)
/* anonymous unions are enabled by default */
#elif defined(__TASKING__)
  #pragma warning 586
#else
  #warning Not supported compiler type
#endif



/* ================================================================================ */
/* ================                       I2C                      ================ */
/* ================================================================================ */


/**
  * @brief I2C-bus interface (I2C)
  */

typedef struct {                                    /*!< (@ 0x40000000) I2C Structure                                          */
  __IO uint32_t  CONSET;                            /*!< (@ 0x40000000) I2C Control Set Register. When a one is written
                                                         to a bit of this register, the corresponding bit in the I2C
                                                          control register is set. Writing a zero has no effect on the
                                                          corresponding bit in the I2C control register.                       */
  __I  uint32_t  STAT;                              /*!< (@ 0x40000004) I2C Status Register. During I2C operation, this
                                                         register provides detailed status codes that allow software
                                                          to determine the next action needed.                                 */
  __IO uint32_t  DAT;                               /*!< (@ 0x40000008) I2C Data Register. During master or slave transmit
                                                         mode, data to be transmitted is written to this register. During
                                                          master or slave receive mode, data that has been received may
                                                          be read from this register.                                          */
  __IO uint32_t  ADR0;                              /*!< (@ 0x4000000C) I2C Slave Address Register 0. Contains the 7-bit
                                                         slave address for operation of the I2C interface in slave mode,
                                                          and is not used in master mode. The least significant bit determines
                                                          whether a slave responds to the General Call address.                */
  __IO uint32_t  SCLH;                              /*!< (@ 0x40000010) SCH Duty Cycle Register High Half Word. Determines
                                                         the high time of the I2C clock.                                       */
  __IO uint32_t  SCLL;                              /*!< (@ 0x40000014) SCL Duty Cycle Register Low Half Word. Determines
                                                         the low time of the I2C clock. I2nSCLL and I2nSCLH together
                                                          determine the clock frequency generated by an I2C master and
                                                          certain times used in slave mode.                                    */
  __O  uint32_t  CONCLR;                            /*!< (@ 0x40000018) I2C Control Clear Register. When a one is written
                                                         to a bit of this register, the corresponding bit in the I2C
                                                          control register is cleared. Writing a zero has no effect on
                                                          the corresponding bit in the I2C control register.                   */
  __IO uint32_t  MMCTRL;                            /*!< (@ 0x4000001C) Monitor mode control register.                         */
  __IO uint32_t  ADR1;                              /*!< (@ 0x40000020) I2C Slave Address Register 1. Contains the 7-bit
                                                         slave address for operation of the I2C interface in slave mode,
                                                          and is not used in master mode. The least significant bit determines
                                                          whether a slave responds to the General Call address.                */
  __IO uint32_t  ADR2;                              /*!< (@ 0x40000024) I2C Slave Address Register 1. Contains the 7-bit
                                                         slave address for operation of the I2C interface in slave mode,
                                                          and is not used in master mode. The least significant bit determines
                                                          whether a slave responds to the General Call address.                */
  __IO uint32_t  ADR3;                              /*!< (@ 0x40000028) I2C Slave Address Register 1. Contains the 7-bit
                                                         slave address for operation of the I2C interface in slave mode,
                                                          and is not used in master mode. The least significant bit determines
                                                          whether a slave responds to the General Call address.                */
  __I  uint32_t  DATA_BUFFER;                       /*!< (@ 0x4000002C) Data buffer register. The contents of the 8 MSBs
                                                         of the I2DAT shift register will be transferred to the DATA_BUFFER
                                                          automatically after every nine bits (8 bits of data plus ACK
                                                          or NACK) has been received on the bus.                               */
  __IO uint32_t  MASK0;                             /*!< (@ 0x40000030) I2C Slave address mask register 0. This mask
                                                         register is associated with I2ADR0 to determine an address match.
                                                          The mask register has no effect when comparing to the General
                                                          Call address (0000000).                                              */
  __IO uint32_t  MASK1;                             /*!< (@ 0x40000034) I2C Slave address mask register 0. This mask
                                                         register is associated with I2ADR0 to determine an address match.
                                                          The mask register has no effect when comparing to the General
                                                          Call address (0000000).                                              */
  __IO uint32_t  MASK2;                             /*!< (@ 0x40000038) I2C Slave address mask register 0. This mask
                                                         register is associated with I2ADR0 to determine an address match.
                                                          The mask register has no effect when comparing to the General
                                                          Call address (0000000).                                              */
  __IO uint32_t  MASK3;                             /*!< (@ 0x4000003C) I2C Slave address mask register 0. This mask
                                                         register is associated with I2ADR0 to determine an address match.
                                                          The mask register has no effect when comparing to the General
                                                          Call address (0000000).                                              */
} LPC_I2C_Type;


/* ================================================================================ */
/* ================                      WWDT                      ================ */
/* ================================================================================ */


/**
  * @brief Windowed WatchDog Timer (WDT) (WWDT)
  */

typedef struct {                                    /*!< (@ 0x40004000) WWDT Structure                                         */
  __IO uint32_t  MOD;                               /*!< (@ 0x40004000) Watchdog mode register. This register contains
                                                         the basic mode and status of the Watchdog Timer.                      */
  __IO uint32_t  TC;                                /*!< (@ 0x40004004) Watchdog timer constant register. This register
                                                         determines the time-out value.                                        */
  __O  uint32_t  FEED;                              /*!< (@ 0x40004008) Watchdog feed sequence register. Writing 0xAA
                                                         followed by 0x55 to this register reloads the Watchdog timer
                                                          with the value contained in TC.                                      */
  __I  uint32_t  TV;                                /*!< (@ 0x4000400C) Watchdog timer value register. This register
                                                         reads out the current value of the Watchdog timer.                    */
  __I  uint32_t  RESERVED;
  __IO uint32_t  WARNINT;                           /*!< (@ 0x40004014) Watchdog Warning Interrupt compare value.              */
  __IO uint32_t  WINDOW;                            /*!< (@ 0x40004018) Watchdog Window compare value.                         */
} LPC_WWDT_Type;


/* ================================================================================ */
/* ================                      UART0                     ================ */
/* ================================================================================ */


/**
  * @brief UART0 (UART0)
  */

typedef struct {                                    /*!< (@ 0x40008000) UART0 Structure                                        */
  
  union {
    __IO uint32_t  DLL;                             /*!< (@ 0x40008000) Divisor Latch LSB. Least significant byte of
                                                         the baud rate divisor value. The full divisor is used to generate
                                                          a baud rate from the fractional rate divider. (DLAB=1)               */
    __O  uint32_t  THR;                             /*!< (@ 0x40008000) Transmit Holding Register. The next character
                                                         to be transmitted is written here. (DLAB=0)                           */
    __I  uint32_t  RBR;                             /*!< (@ 0x40008000) Receiver Buffer Register. Contains the next received
                                                         character to be read. (DLAB=0)                                        */
  };
  
  union {
    __IO uint32_t  IER;                             /*!< (@ 0x40008004) Interrupt Enable Register. Contains individual
                                                         interrupt enable bits for the 7 potential UART interrupts. (DLAB=0)   */
    __IO uint32_t  DLM;                             /*!< (@ 0x40008004) Divisor Latch MSB. Most significant byte of the
                                                         baud rate divisor value. The full divisor is used to generate
                                                          a baud rate from the fractional rate divider. (DLAB=1)               */
  };
  
  union {
    __O  uint32_t  FCR;                             /*!< (@ 0x40008008) FIFO Control Register. Controls UART FIFO usage
                                                         and modes.                                                            */
    __I  uint32_t  IIR;                             /*!< (@ 0x40008008) Interrupt ID Register. Identifies which interrupt(s)
                                                         are pending.                                                          */
  };
  __IO uint32_t  LCR;                               /*!< (@ 0x4000800C) Line Control Register. Contains controls for
                                                         frame formatting and break generation.                                */
  __IO uint32_t  MCR;                               /*!< (@ 0x40008010) Modem Control Register.                                */
  __I  uint32_t  LSR;                               /*!< (@ 0x40008014) Line Status Register. Contains flags for transmit
                                                         and receive status, including line errors.                            */
  __I  uint32_t  MSR;                               /*!< (@ 0x40008018) Modem Status Register.                                 */
  __IO uint32_t  SCR;                               /*!< (@ 0x4000801C) Scratch Pad Register. Eight-bit temporary storage
                                                         for software.                                                         */
  __IO uint32_t  ACR;                               /*!< (@ 0x40008020) Auto-baud Control Register. Contains controls
                                                         for the auto-baud feature.                                            */
  __I  uint32_t  RESERVED;
  __IO uint32_t  FDR;                               /*!< (@ 0x40008028) Fractional Divider Register. Generates a clock
                                                         input for the baud rate divider.                                      */
  __I  uint32_t  RESERVED1;
  __IO uint32_t  TER;                               /*!< (@ 0x40008030) Transmit Enable Register. Turns off UART transmitter
                                                         for use with software flow control.                                   */
  __I  uint32_t  RESERVED2[6];
  __IO uint32_t  RS485CTRL;                         /*!< (@ 0x4000804C) RS-485/EIA-485 Control. Contains controls to
                                                         configure various aspects of RS-485/EIA-485 modes.                    */
  __IO uint32_t  RS485ADRMATCH;                     /*!< (@ 0x40008050) RS-485/EIA-485 address match. Contains the address
                                                         match value for RS-485/EIA-485 mode.                                  */
  __IO uint32_t  RS485DLY;                          /*!< (@ 0x40008054) RS-485/EIA-485 direction control delay.                */
} LPC_UART0_Type;


/* ================================================================================ */
/* ================                     CT16B0                     ================ */
/* ================================================================================ */


/**
  * @brief 16-bit counter/timer CT16B0/1  (CT16B0)
  */

typedef struct {                                    /*!< (@ 0x4000C000) CT16B0 Structure                                       */
  __IO uint32_t  IR;                                /*!< (@ 0x4000C000) Interrupt register (IR). The IR can be written
                                                         to clear interrupts. The IR can be read to identify which of
                                                          five possible interrupt sources are pending.                         */
  __IO uint32_t  TCR;                               /*!< (@ 0x4000C004) Timer Control register (TCR). The TCR is used
                                                         to control the Timer Counter functions. The Timer Counter can
                                                          be disabled or reset through the TCR.                                */
  __IO uint32_t  TC;                                /*!< (@ 0x4000C008) Timer Counter (TC). The 32-bit TC is incremented
                                                         every PR+1 cycles of PCLK. The TC is controlled through the
                                                          TCR.                                                                 */
  __IO uint32_t  PR;                                /*!< (@ 0x4000C00C) Prescale register (PR). When the Prescale Counter
                                                         (below) is equal to this value, the next clock increments the
                                                          TC and clears the PC.                                                */
  __IO uint32_t  PC;                                /*!< (@ 0x4000C010) Prescale Counter (PC). The 16-bit PC is a counter
                                                         which is incremented to the value stored in PR. When the value
                                                          in PR is reached, the TC is incremented and the PC is cleared.
                                                          The PC is observable and controllable through the bus interface.     */
  __IO uint32_t  MCR;                               /*!< (@ 0x4000C014) Match Control register (MCR). The MCR is used
                                                         to control if an interrupt is generated and if the TC is reset
                                                          when a Match occurs.                                                 */
  __IO uint32_t  MR[4];                             /*!< (@ 0x4000C018) Match register 0 (MR0). MR0 can be enabled through
                                                         the MCR to reset the TC, stop both the TC and PC, and/or generate
                                                          an interrupt every time MR0 matches the TC.                          */
  __IO uint32_t  CCR;                               /*!< (@ 0x4000C028) Capture Control register (CCR). The CCR controls
                                                         which edges of the capture inputs are used to load the Capture
                                                          registers and whether or not an interrupt is generated when
                                                          a capture takes place.                                               */
  __I  uint32_t  CR[2];                             /*!< (@ 0x4000C02C) Capture register. CR is loaded with the value
                                                         of TC when there is an event on the CT16B0_CAP input.                 */
  __I  uint32_t  RESERVED[2];
  __IO uint32_t  EMR;                               /*!< (@ 0x4000C03C) External Match register (EMR). The EMR controls
                                                         the match function and the external match pins CT16B0_MAT[2:0].       */
  __I  uint32_t  RESERVED1[12];
  __IO uint32_t  CTCR;                              /*!< (@ 0x4000C070) Count Control register (CTCR). The CTCR selects
                                                         between Timer and Counter mode, and in Counter mode selects
                                                          the signal and edge(s) for counting.                                 */
  __IO uint32_t  PWMC;                              /*!< (@ 0x4000C074) PWM Control register (PWMCON). The PWMCON enables
                                                         PWM mode for the external match pins CT16B0_MAT[2:0].                 */
} LPC_CT16B0_Type;


/* ================================================================================ */
/* ================                     CT32B0                     ================ */
/* ================================================================================ */


/**
  * @brief 32-bit counter/timer CT32B0/1  (CT32B0)
  */

typedef struct {                                    /*!< (@ 0x40014000) CT32B0 Structure                                       */
  __IO uint32_t  IR;                                /*!< (@ 0x40014000) Interrupt register (IR). The IR can be written
                                                         to clear interrupts. The IR can be read to identify which of
                                                          five possible interrupt sources are pending.                         */
  __IO uint32_t  TCR;                               /*!< (@ 0x40014004) Timer Control register (TCR). The TCR is used
                                                         to control the Timer Counter functions. The Timer Counter can
                                                          be disabled or reset through the TCR.                                */
  __IO uint32_t  TC;                                /*!< (@ 0x40014008) Timer Counter (TC). The 32-bit TC is incremented
                                                         every PR+1 cycles of PCLK. The TC is controlled through the
                                                          TCR.                                                                 */
  __IO uint32_t  PR;                                /*!< (@ 0x4001400C) Prescale register (PR). When the Prescale Counter
                                                         (below) is equal to this value, the next clock increments the
                                                          TC and clears the PC.                                                */
  __IO uint32_t  PC;                                /*!< (@ 0x40014010) Prescale Counter (PC). The 32-bit PC is a counter
                                                         which is incremented to the value stored in PR. When the value
                                                          in PR is reached, the TC is incremented and the PC is cleared.
                                                          The PC is observable and controllable through the bus interface.     */
  __IO uint32_t  MCR;                               /*!< (@ 0x40014014) Match Control register (MCR). The MCR is used
                                                         to control if an interrupt is generated and if the TC is reset
                                                          when a Match occurs.                                                 */
  __IO uint32_t  MR[4];                             /*!< (@ 0x40014018) Match register 0 (MR0). MR0 can be enabled through
                                                         the MCR to reset the TC, stop both the TC and PC, and/or generate
                                                          an interrupt every time MR0 matches the TC.                          */
  __IO uint32_t  CCR;                               /*!< (@ 0x40014028) Capture Control register (CCR). The CCR controls
                                                         which edges of the capture inputs are used to load the Capture
                                                          registers and whether or not an interrupt is generated when
                                                          a capture takes place.                                               */
  __I  uint32_t  CR[2];                             /*!< (@ 0x4001402C) Capture register 0 (CR0). CR0 is loaded with
                                                         the value of TC when there is an event on the CT32B0_CAP0 input.      */
  __I  uint32_t  RESERVED[2];
  __IO uint32_t  EMR;                               /*!< (@ 0x4001403C) External Match register (EMR). The EMR controls
                                                         the match function and the external match pins CT32B0_MAT[3:0].       */
  __I  uint32_t  RESERVED1[12];
  __IO uint32_t  CTCR;                              /*!< (@ 0x40014070) Count Control register (CTCR). The CTCR selects
                                                         between Timer and Counter mode, and in Counter mode selects
                                                          the signal and edge(s) for counting.                                 */
  __IO uint32_t  PWMC;                              /*!< (@ 0x40014074) PWM Control register (PWMCON). The PWMCON enables
                                                         PWM mode for the external match pins CT32B0_MAT[3:0].                 */
} LPC_CT32B0_Type;


/* ================================================================================ */
/* ================                       ADC                      ================ */
/* ================================================================================ */


/**
  * @brief 12-bit ADC (ADC)
  */

typedef struct {                                    /*!< (@ 0x4001C000) ADC Structure                                          */
  __IO uint32_t  CTRL;                              /*!< (@ 0x4001C000) A/D Control Register. Contains the clock divide
                                                         value, enable bits for each sequence and the A/D power-down
                                                          bit.                                                                 */
  __I  uint32_t  RESERVED;
  __IO uint32_t  SEQA_CTRL;                         /*!< (@ 0x4001C008) A/D Conversion Sequence-A control Register: Controls
                                                         triggering and channel selection for conversion sequence-A.
                                                          Also specifies interrupt mode for sequence-A.                        */
  __IO uint32_t  SEQB_CTRL;                         /*!< (@ 0x4001C00C) A/D Conversion Sequence-B Control Register: Controls
                                                         triggering and channel selection for conversion sequence-B.
                                                          Also specifies interrupt mode for sequence-B.                        */
  __IO uint32_t  SEQA_GDAT;                         /*!< (@ 0x4001C010) A/D Sequence-A Global Data Register. This register
                                                         contains the result of the most recent A/D conversion performed
                                                          under sequence-A                                                     */
  __IO uint32_t  SEQB_GDAT;                         /*!< (@ 0x4001C014) A/D Sequence-B Global Data Register. This register
                                                         contains the result of the most recent A/D conversion performed
                                                          under sequence-B                                                     */
  __I  uint32_t  RESERVED1[3];
  __I  uint32_t  DAT1;                              /*!< (@ 0x4001C024) A/D Channel 1 Data Register. This register contains
                                                         the result of the most recent conversion completed on channel
                                                          1.                                                                   */
  __I  uint32_t  DAT2;                              /*!< (@ 0x4001C028) A/D Channel 1 Data Register. This register contains
                                                         the result of the most recent conversion completed on channel
                                                          1.                                                                   */
  __I  uint32_t  DAT3;                              /*!< (@ 0x4001C02C) A/D Channel 1 Data Register. This register contains
                                                         the result of the most recent conversion completed on channel
                                                          1.                                                                   */
  __I  uint32_t  DAT4;                              /*!< (@ 0x4001C030) A/D Channel 1 Data Register. This register contains
                                                         the result of the most recent conversion completed on channel
                                                          1.                                                                   */
  __I  uint32_t  DAT5;                              /*!< (@ 0x4001C034) A/D Channel 1 Data Register. This register contains
                                                         the result of the most recent conversion completed on channel
                                                          1.                                                                   */
  __I  uint32_t  DAT6;                              /*!< (@ 0x4001C038) A/D Channel 1 Data Register. This register contains
                                                         the result of the most recent conversion completed on channel
                                                          1.                                                                   */
  __I  uint32_t  DAT7;                              /*!< (@ 0x4001C03C) A/D Channel 1 Data Register. This register contains
                                                         the result of the most recent conversion completed on channel
                                                          1.                                                                   */
  __I  uint32_t  DAT8;                              /*!< (@ 0x4001C040) A/D Channel 1 Data Register. This register contains
                                                         the result of the most recent conversion completed on channel
                                                          1.                                                                   */
  __I  uint32_t  RESERVED2[3];
  __IO uint32_t  THR0_LOW;                          /*!< (@ 0x4001C050) A/D Low Compare Threshold Register 0 : Contains
                                                         the lower threshold level for automatic threshold comparison
                                                          for any channels linked to threshold pair 0.                         */
  __IO uint32_t  THR1_LOW;                          /*!< (@ 0x4001C054) A/D Low Compare Threshold Register 1: Contains
                                                         the lower threshold level for automatic threshold comparison
                                                          for any channels linked to threshold pair 1.                         */
  __IO uint32_t  THR0_HIGH;                         /*!< (@ 0x4001C058) A/D High Compare Threshold Register 0: Contains
                                                         the upper threshold level for automatic threshold comparison
                                                          for any channels linked to threshold pair 0.                         */
  __IO uint32_t  THR1_HIGH;                         /*!< (@ 0x4001C05C) A/D High Compare Threshold Register 1: Contains
                                                         the upper threshold level for automatic threshold comparison
                                                          for any channels linked to threshold pair 1.                         */
  __IO uint32_t  CHAN_THRSEL;                       /*!< (@ 0x4001C060) A/D Channel-Threshold Select Register. Specifies
                                                         which set of threshold compare registers are to be used for
                                                          each channel                                                         */
  __IO uint32_t  INTEN;                             /*!< (@ 0x4001C064) A/D Interrupt Enable Register. This register
                                                         contains enable bits that enable the sequence-A, sequence-B,
                                                          threshold compare and data overrun interrupts to be generated.       */
  __I  uint32_t  FLAGS;                             /*!< (@ 0x4001C068) A/D Flags Register. Contains the four interrupt
                                                         request flags and the individual component overrun and threshold-compare
                                                          flags. (The overrun bits replicate information stored in the
                                                          result registers).                                                   */
  __IO uint32_t  TRM;                               /*!< (@ 0x4001C06C) ADC trim register.                                     */
} LPC_ADC_Type;


/* ================================================================================ */
/* ================                       PMU                      ================ */
/* ================================================================================ */


/**
  * @brief Power management unit (PMU)
  */

typedef struct {                                    /*!< (@ 0x40038000) PMU Structure                                          */
  __IO uint32_t  PCON;                              /*!< (@ 0x40038000) Power control register                                 */
  __IO uint32_t  GPREG[4];                          /*!< (@ 0x40038004) General purpose register 0                             */
  __IO uint32_t  GPREG4;                            /*!< (@ 0x40038014) General purpose register 4                             */
} LPC_PMU_Type;


/* ================================================================================ */
/* ================                    FLASHCTRL                   ================ */
/* ================================================================================ */


/**
  * @brief Flash controller (FLASHCTRL)
  */

typedef struct {                                    /*!< (@ 0x4003C000) FLASHCTRL Structure                                    */
  __I  uint32_t  RESERVED[4];
  __IO uint32_t  FLASHCFG;                          /*!< (@ 0x4003C010) Flash access time configuration register               */
  __I  uint32_t  RESERVED1[3];
  __IO uint32_t  FMSSTART;                          /*!< (@ 0x4003C020) Signature start address register                       */
  __IO uint32_t  FMSSTOP;                           /*!< (@ 0x4003C024) Signature stop-address register                        */
  __I  uint32_t  RESERVED2;
  __I  uint32_t  FMSW0;                             /*!< (@ 0x4003C02C) Word 0 [31:0]                                          */
  __I  uint32_t  FMSW1;                             /*!< (@ 0x4003C030) Word 1 [63:32]                                         */
  __I  uint32_t  FMSW2;                             /*!< (@ 0x4003C034) Word 2 [95:64]                                         */
  __I  uint32_t  FMSW3;                             /*!< (@ 0x4003C038) Word 3 [127:96]                                        */
  __I  uint32_t  RESERVED3[1001];
  __I  uint32_t  FMSTAT;                            /*!< (@ 0x4003CFE0) Signature generation status register                   */
  __I  uint32_t  RESERVED4;
  __O  uint32_t  FMSTATCLR;                         /*!< (@ 0x4003CFE8) Signature generation status clear register             */
} LPC_FLASHCTRL_Type;


/* ================================================================================ */
/* ================                      SSP0                      ================ */
/* ================================================================================ */


/**
  * @brief SSP0 (SSP0)
  */

typedef struct {                                    /*!< (@ 0x40040000) SSP0 Structure                                         */
  __IO uint32_t  CR0;                               /*!< (@ 0x40040000) Control register 0. Selects the serial clock
                                                         rate, bus type, and data size.                                        */
  __IO uint32_t  CR1;                               /*!< (@ 0x40040004) Control register 1. Selects master/slave and
                                                         other modes.                                                          */
  __IO uint32_t  DR;                                /*!< (@ 0x40040008) Data register. Writes fill the transmit FIFO,
                                                         and reads empty the receive FIFO.                                     */
  __I  uint32_t  SR;                                /*!< (@ 0x4004000C) Status register                                        */
  __IO uint32_t  CPSR;                              /*!< (@ 0x40040010) Clock Prescale register                                */
  __IO uint32_t  IMSC;                              /*!< (@ 0x40040014) Interrupt Mask Set and Clear register                  */
  __I  uint32_t  RIS;                               /*!< (@ 0x40040018) Raw Interrupt Status register                          */
  __I  uint32_t  MIS;                               /*!< (@ 0x4004001C) Masked Interrupt Status register                       */
  __O  uint32_t  ICR;                               /*!< (@ 0x40040020) SSPICR Interrupt Clear register                        */
} LPC_SSP0_Type;


/* ================================================================================ */
/* ================                      IOCON                     ================ */
/* ================================================================================ */


/**
  * @brief I/O configuration (IOCON)  (IOCON)
  */

typedef struct {                                    /*!< (@ 0x40044000) IOCON Structure                                        */
  __IO uint32_t  PIO2_6;                            /*!< (@ 0x40044000) I/O configuration for pin PIO2_6/ CT32B0_MAT1          */
  __I  uint32_t  RESERVED;
  __IO uint32_t  PIO2_0;                            /*!< (@ 0x40044008) I/O configuration for pin PIO2_0/U0_DTR/SSP1_SSEL/
                                                         ADC_PIN_TRIG4                                                         */
  __IO uint32_t  RESET_PIO0_0;                      /*!< (@ 0x4004400C) I/O configuration for pin RESET/PIO0_0                 */
  __IO uint32_t  PIO0_1;                            /*!< (@ 0x40044010) I/O configuration for pin PIO0_1/CLKOUT/CT32B0_MAT2    */
  __I  uint32_t  RESERVED1;
  __IO uint32_t  PIO1_8;                            /*!< (@ 0x40044018) I/O configuration for pin PIO1_8/CT16B1_CAP0/R/U2_TXD  */
  __IO uint32_t  PIO0_2;                            /*!< (@ 0x4004401C) I/O configuration for pin PIO0_2/SSP0_SSEL/CT16B0_CAP0/
                                                         ADC_PIN_TRIG0                                                         */
  __IO uint32_t  PIO2_7;                            /*!< (@ 0x40044020) I/O configuration for pin PIO2_7/ CT32B0_MAT2/U0_RXD   */
  __IO uint32_t  PIO2_8;                            /*!< (@ 0x40044024) I/O configuration for pin PIO2_8/ CT32B0_MAT3/U0_TXD   */
  __IO uint32_t  PIO2_1;                            /*!< (@ 0x40044028) I/O configuration for pin PIO2_1/U0_DSR/SSP1_SCK       */
  __I  uint32_t  RESERVED2;
  __IO uint32_t  PIO0_4;                            /*!< (@ 0x40044030) I/O configuration for pin PIO0_4/I2C0_SCL              */
  __IO uint32_t  PIO0_5;                            /*!< (@ 0x40044034) I/O configuration for pin PIO0_5/I2C0_SDA              */
  __IO uint32_t  PIO1_9;                            /*!< (@ 0x40044038) I/O configuration for pin PIO1_9/CT16B1_MAT0/SSP1_
                                                         MOSI                                                                  */
  __IO uint32_t  PIO3_4;                            /*!< (@ 0x4004403C) I/O configuration for pin PIO3_4/ CT16B0_CAP1/U0_RXD   */
  __IO uint32_t  PIO2_4;                            /*!< (@ 0x40044040) I/O configuration for pin PIO2_4/ CT16B1_MAT1/
                                                         SSP1_SSEL                                                             */
  __IO uint32_t  PIO2_5;                            /*!< (@ 0x40044044) I/O configuration for pin PIO2_5/ CT32B0_MAT0          */
  __IO uint32_t  PIO3_5;                            /*!< (@ 0x40044048) I/O configuration for pin PIO3_5/ CT16B1_CAP1/U0_TXD   */
  __I  uint32_t  RESERVED3[2];
  __IO uint32_t  PIO2_9;                            /*!< (@ 0x40044054) I/O configuration for pin PIO2_9/ CT32B0_CAP0          */
  __IO uint32_t  PIO2_10;                           /*!< (@ 0x40044058) I/O configuration for pin PIO2_10                      */
  __IO uint32_t  PIO2_2;                            /*!< (@ 0x4004405C) I/O configuration for pin PIO2_2/U0_DCD/SSP1_MISO      */
  __IO uint32_t  PIO0_8;                            /*!< (@ 0x40044060) I/O configuration for pin PIO0_8/SSP0_MISO/CT16B0_MAT0/R/AD
                                                         C_PIN_TRIG2                                                           */
  __IO uint32_t  PIO0_9;                            /*!< (@ 0x40044064) I/O configuration for pin PIO0_9SSP0_MOSI/CT16B0_MAT1/R/ADC
                                                         _PIN_TRIG3                                                            */
  __IO uint32_t  SWCLK_PIO0_10;                     /*!< (@ 0x40044068) I/O configuration for pin SWCLK/PIO0_10/ SSP0_SCK/CT16B0_MA
                                                         T2                                                                    */
  __IO uint32_t  PIO1_10;                           /*!< (@ 0x4004406C) I/O configuration for pin PIO1_10/ADC_8/CT16B1_MAT1/
                                                         SSP1_MISO                                                             */
  __I  uint32_t  RESERVED4;
  __IO uint32_t  R_PIO0_11;                         /*!< (@ 0x40044074) I/O configuration for pin R/PIO0_11/ADC_7/CT32B0_MAT3  */
  __IO uint32_t  R_PIO1_0;                          /*!< (@ 0x40044078) I/O configuration for pin R/PIO1_0/ADC_6/CT32B1_CAP0   */
  __IO uint32_t  R_PIO1_1;                          /*!< (@ 0x4004407C) I/O configuration for pin R/PIO1_1/ADC_5/CT32B1_MAT0   */
  __IO uint32_t  R_PIO1_2;                          /*!< (@ 0x40044080) I/O configuration for pin R/PIO1_2/ADC_4/CT32B1_MAT1   */
  __IO uint32_t  PIO3_0;                            /*!< (@ 0x40044084) I/O configuration for pin PIO3_0/U0_DTR/CT16B0_MAT0/
                                                         U0_TXD                                                                */
  __I  uint32_t  RESERVED5;
  __IO uint32_t  PIO2_3;                            /*!< (@ 0x4004408C) I/O configuration for pin PIO2_3/U0_RI/SSP1_MOSI       */
  __IO uint32_t  SWDIO_PIO1_3;                      /*!< (@ 0x40044090) I/O configuration for pin SWDIO/PIO1_3/ADC_3/CT32B1_MAT2 */
  __IO uint32_t  PIO1_4;                            /*!< (@ 0x40044094) I/O configuration for pin PIO1_4/ADC_2/CT32B1_MAT3     */
  __IO uint32_t  PIO1_11;                           /*!< (@ 0x40044098) I/O configuration for pin PIO1_11/ADC_1/CT32B1_CAP1    */
  __IO uint32_t  PIO3_2;                            /*!< (@ 0x4004409C) I/O configuration for pin PIO3_2/U0_DCD/ CT16B0_MAT2/SSP1_S
                                                         CK                                                                    */
  __IO uint32_t  PIO1_5;                            /*!< (@ 0x400440A0) I/O configuration for pin PIO1_5/U0_RTS/CT32B0_CAP0    */
  __IO uint32_t  PIO1_6;                            /*!< (@ 0x400440A4) I/O configuration for pin PIO1_6/U0_RXD/CT32B0_MAT0    */
  __IO uint32_t  PIO1_7;                            /*!< (@ 0x400440A8) I/O configuration for pin PIO1_7/U0_TXD/CT32B0_MAT1    */
  __IO uint32_t  PIO3_3;                            /*!< (@ 0x400440AC) I/O configuration for pin PIO3_3/U0_RI/ CT16B0_CAP0    */
  __IO uint32_t  SCK0_LOC;                          /*!< (@ 0x400440B0) SCK0 pin location select register                      */
  __IO uint32_t  DSR_LOC;                           /*!< (@ 0x400440B4) DSR pin location select register                       */
  __IO uint32_t  DCD_LOC;                           /*!< (@ 0x400440B8) DCD pin location select register                       */
  __IO uint32_t  RI_LOC;                            /*!< (@ 0x400440BC) RI pin location select register                        */
  __IO uint32_t  PIO0_3;                            /*!< (@ 0x400440C0) I/O configuration for pin PIO0_3/R/R/U2_RXD            */
  __IO uint32_t  PIO0_6;                            /*!< (@ 0x400440C4) I/O configuration for pin PIO0_6/SSP0_SCK/U1_TXD       */
  __IO uint32_t  PIO0_7;                            /*!< (@ 0x400440C8) I/O configuration for pin PIO0_7/U0_CTS/ADC_PIN_TRIG1/U1_RX
                                                         D                                                                     */
  __IO uint32_t  SSEL1_LOC;                         /*!< (@ 0x400440CC) SSEL1 pin location select register                     */
  __IO uint32_t  CT16B0_CAP0_LOC;                   /*!< (@ 0x400440D0) CT16B0_CAP0 pin location select register               */
  __IO uint32_t  SCK1_LOC;                          /*!< (@ 0x400440D4) SCK1 pin location select register                      */
  __IO uint32_t  MISO1_LOC;                         /*!< (@ 0x400440D8) MISO1 pin location select register                     */
  __IO uint32_t  MOSI1_LOC;                         /*!< (@ 0x400440DC) MOSI1 pin location select register                     */
  __IO uint32_t  CT32B0_CAP0_LOC;                   /*!< (@ 0x400440E0) CT32B0_CAP0 pin location select register               */
  __IO uint32_t  RXD_LOC;                           /*!< (@ 0x400440E4) RXD pin location select register                       */
} LPC_IOCON_Type;


/* ================================================================================ */
/* ================                     SYSCON                     ================ */
/* ================================================================================ */


/**
  * @brief System configuration (SYSCON) (SYSCON)
  */

typedef struct {                                    /*!< (@ 0x40048000) SYSCON Structure                                       */
  __IO uint32_t  SYSMEMREMAP;                       /*!< (@ 0x40048000) System memory remap                                    */
  __IO uint32_t  PRESETCTRL;                        /*!< (@ 0x40048004) Peripheral reset control                               */
  __IO uint32_t  SYSPLLCTRL;                        /*!< (@ 0x40048008) System PLL control                                     */
  __I  uint32_t  SYSPLLSTAT;                        /*!< (@ 0x4004800C) System PLL status                                      */
  __I  uint32_t  RESERVED[4];
  __IO uint32_t  SYSOSCCTRL;                        /*!< (@ 0x40048020) System oscillator control                              */
  __IO uint32_t  WDTOSCCTRL;                        /*!< (@ 0x40048024) Watchdog oscillator control                            */
  __IO uint32_t  IRCCTRL;                           /*!< (@ 0x40048028) IRC control                                            */
  __I  uint32_t  RESERVED1;
  __IO uint32_t  SYSRSTSTAT;                        /*!< (@ 0x40048030) System reset status register                           */
  __I  uint32_t  RESERVED2[3];
  __IO uint32_t  SYSPLLCLKSEL;                      /*!< (@ 0x40048040) System PLL clock source select                         */
  __IO uint32_t  SYSPLLCLKUEN;                      /*!< (@ 0x40048044) System PLL clock source update enable                  */
  __I  uint32_t  RESERVED3[10];
  __IO uint32_t  MAINCLKSEL;                        /*!< (@ 0x40048070) Main clock source select                               */
  __IO uint32_t  MAINCLKUEN;                        /*!< (@ 0x40048074) Main clock source update enable                        */
  __IO uint32_t  SYSAHBCLKDIV;                      /*!< (@ 0x40048078) System AHB clock divider                               */
  __I  uint32_t  RESERVED4;
  __IO uint32_t  SYSAHBCLKCTRL;                     /*!< (@ 0x40048080) System AHB clock control                               */
  __I  uint32_t  RESERVED5[4];
  __IO uint32_t  SSP0CLKDIV;                        /*!< (@ 0x40048094) SSP0 clock divider                                     */
  __IO uint32_t  UART0CLKDIV;                       /*!< (@ 0x40048098) UART0 clock divider                                    */
  __IO uint32_t  SSP1CLKDIV;                        /*!< (@ 0x4004809C) SSP1 clock divider                                     */
  __IO uint32_t  UART1CLKDIV;                       /*!< (@ 0x400480A0) UART1 clock divider                                    */
  __IO uint32_t  UART2CLKDIV;                       /*!< (@ 0x400480A4) UART2 clock divider                                    */
  __I  uint32_t  RESERVED6[10];
  __IO uint32_t  WDTCLKSEL;                         /*!< (@ 0x400480D0) WDT clock source select                                */
  __IO uint32_t  WDTCLKUEN;                         /*!< (@ 0x400480D4) WDT clock source update enable                         */
  __IO uint32_t  WDTCLKDIV;                         /*!< (@ 0x400480D8) WDT clock divider                                      */
  __I  uint32_t  RESERVED7;
  __IO uint32_t  CLKOUTCLKSEL;                      /*!< (@ 0x400480E0) CLKOUT clock source select                             */
  __IO uint32_t  CLKOUTUEN;                         /*!< (@ 0x400480E4) CLKOUT clock source update enable                      */
  __IO uint32_t  CLKOUTCLKDIV;                      /*!< (@ 0x400480E8) CLKOUT clock divider                                   */
  __I  uint32_t  RESERVED8[5];
  __I  uint32_t  PIOPORCAP0;                        /*!< (@ 0x40048100) POR captured PIO status 0                              */
  __I  uint32_t  PIOPORCAP1;                        /*!< (@ 0x40048104) POR captured PIO status 1                              */
  __I  uint32_t  RESERVED9[18];
  __IO uint32_t  BODCTRL;                           /*!< (@ 0x40048150) BOD control                                            */
  __IO uint32_t  SYSTCKCAL;                         /*!< (@ 0x40048154) System tick counter calibration                        */
  __I  uint32_t  RESERVED10[6];
  __IO uint32_t  IRQLATENCY;                        /*!< (@ 0x40048170) IQR delay. Allows trade-off between interrupt
                                                         latency and determinism.                                              */
  __IO uint32_t  NMISRC;                            /*!< (@ 0x40048174) NMI source selection                                   */
  __I  uint32_t  RESERVED11[34];
  __IO uint32_t  STARTAPRP0;                        /*!< (@ 0x40048200) Start logic edge control register 0                    */
  __IO uint32_t  STARTERP0;                         /*!< (@ 0x40048204) Start logic signal enable register 0                   */
  __O  uint32_t  STARTRSRP0CLR;                     /*!< (@ 0x40048208) Start logic reset register 0                           */
  __I  uint32_t  STARTSRP0;                         /*!< (@ 0x4004820C) Start logic status register 0                          */
  __I  uint32_t  RESERVED12[8];
  __IO uint32_t  PDSLEEPCFG;                        /*!< (@ 0x40048230) Power-down states in Deep-sleep mode                   */
  __IO uint32_t  PDAWAKECFG;                        /*!< (@ 0x40048234) Power-down states after wake-up from Deep-sleep
                                                         mode                                                                  */
  __IO uint32_t  PDRUNCFG;                          /*!< (@ 0x40048238) Power-down configuration register                      */
} LPC_SYSCON_Type;


/* ================================================================================ */
/* ================                      GPIO0                     ================ */
/* ================================================================================ */


/**
  * @brief GPIO0 (GPIO0)
  */

typedef struct {                                    /*!< (@ 0x50000000) GPIO0 Structure                                        */
  __IO  uint32_t  RESERVED[4095];
  __IO uint32_t  DATA;                              /*!< (@ 0x50003FFC) Port n data register for pins PIOn_0 to PIOn_11        */
  __I  uint32_t  RESERVED1[4096];
  __IO uint32_t  DIR;                               /*!< (@ 0x50008000) Data direction register for port n                     */
  __IO uint32_t  IS;                                /*!< (@ 0x50008004) Interrupt sense register for port n                    */
  __IO uint32_t  IBE;                               /*!< (@ 0x50008008) Interrupt both edges register for port n               */
  __IO uint32_t  IEV;                               /*!< (@ 0x5000800C) Interrupt event register for port n                    */
  __IO uint32_t  IE;                                /*!< (@ 0x50008010) Interrupt mask register for port n                     */
  __I  uint32_t  RIS;                               /*!< (@ 0x50008014) Raw interrupt status register for port n               */
  __I  uint32_t  MIS;                               /*!< (@ 0x50008018) Masked interrupt status register for port n            */
  __O  uint32_t  IC;                                /*!< (@ 0x5000801C) Interrupt clear register for port n                    */
} LPC_GPIO0_Type;


/* --------------------  End of section using anonymous unions  ------------------- */
#if defined(__CC_ARM)
  #pragma pop
#elif defined(__ICCARM__)
  /* leave anonymous unions enabled */
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined(__TASKING__)
  #pragma warning restore
#else
  #warning Not supported compiler type
#endif




/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */

#define LPC_I2C_BASE                    0x40000000UL
#define LPC_WWDT_BASE                   0x40004000UL
#define LPC_UART0_BASE                  0x40008000UL
#define LPC_CT16B0_BASE                 0x4000C000UL
#define LPC_CT16B1_BASE                 0x40010000UL
#define LPC_CT32B0_BASE                 0x40014000UL
#define LPC_CT32B1_BASE                 0x40018000UL
#define LPC_ADC_BASE                    0x4001C000UL
#define LPC_UART1_BASE                  0x40020000UL
#define LPC_UART2_BASE                  0x40024000UL
#define LPC_PMU_BASE                    0x40038000UL
#define LPC_FLASHCTRL_BASE              0x4003C000UL
#define LPC_SSP0_BASE                   0x40040000UL
#define LPC_IOCON_BASE                  0x40044000UL
#define LPC_SYSCON_BASE                 0x40048000UL
#define LPC_SSP1_BASE                   0x40058000UL
#define LPC_GPIO0_BASE                  0x50000000UL
#define LPC_GPIO1_BASE                  0x50010000UL
#define LPC_GPIO2_BASE                  0x50020000UL
#define LPC_GPIO3_BASE                  0x50030000UL


/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */

#define LPC_I2C                         ((LPC_I2C_Type            *) LPC_I2C_BASE)
#define LPC_WWDT                        ((LPC_WWDT_Type           *) LPC_WWDT_BASE)
#define LPC_UART0                       ((LPC_UART0_Type          *) LPC_UART0_BASE)
#define LPC_CT16B0                      ((LPC_CT16B0_Type         *) LPC_CT16B0_BASE)
#define LPC_CT16B1                      ((LPC_CT16B0_Type         *) LPC_CT16B1_BASE)
#define LPC_CT32B0                      ((LPC_CT32B0_Type         *) LPC_CT32B0_BASE)
#define LPC_CT32B1                      ((LPC_CT32B0_Type         *) LPC_CT32B1_BASE)
#define LPC_ADC                         ((LPC_ADC_Type            *) LPC_ADC_BASE)
#define LPC_UART1                       ((LPC_UART0_Type          *) LPC_UART1_BASE)
#define LPC_UART2                       ((LPC_UART0_Type          *) LPC_UART2_BASE)
#define LPC_PMU                         ((LPC_PMU_Type            *) LPC_PMU_BASE)
#define LPC_FLASHCTRL                   ((LPC_FLASHCTRL_Type      *) LPC_FLASHCTRL_BASE)
#define LPC_SSP0                        ((LPC_SSP0_Type           *) LPC_SSP0_BASE)
#define LPC_IOCON                       ((LPC_IOCON_Type          *) LPC_IOCON_BASE)
#define LPC_SYSCON                      ((LPC_SYSCON_Type         *) LPC_SYSCON_BASE)
#define LPC_SSP1                        ((LPC_SSP0_Type           *) LPC_SSP1_BASE)
#define LPC_GPIO0                       ((LPC_GPIO0_Type          *) LPC_GPIO0_BASE)
#define LPC_GPIO1                       ((LPC_GPIO0_Type          *) LPC_GPIO1_BASE)
#define LPC_GPIO2                       ((LPC_GPIO0_Type          *) LPC_GPIO2_BASE)
#define LPC_GPIO3                       ((LPC_GPIO0_Type          *) LPC_GPIO3_BASE)


/** @} */ /* End of group Device_Peripheral_Registers */
/** @} */ /* End of group LPC112x */
/** @} */ /* End of group (null) */

#ifdef __cplusplus
}
#endif


#endif  /* LPC112x_H */

