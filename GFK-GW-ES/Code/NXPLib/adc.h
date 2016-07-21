#ifndef __ADC_H__
#define __ADC_H__

#include "chip.h"


/** Maximum sample rate in Hz (12-bit conversions) */
#define ADC_MAX_SAMPLE_RATE     48000000

/**
 * @brief ADC register support bitfields and mask
 */
/** ADC Control register bit fields */
#define ADC_CR_CLKDIV_MASK      (0xFF << 0)                  /*!< Mask for Clock divider value */
#define ADC_CR_CLKDIV_BITPOS    (0)                          /*!< Bit position for Clock divider value */
#define ADC_CR_MODE10BIT        (1 << 9)                     /*!< 10-bit mode enable bit */
#define ADC_CR_LPWRMODEBIT      (1 << 10)                    /*!< Low power mode enable bit */
#define ADC_CR_CALMODEBIT       (1 << 30)                    /*!< Self calibration cycle enable bit */
#define ADC_CR_BITACC(n)        ((((n) & 0x1) << 9))         /*!< 12-bit or 10-bit ADC accuracy */
#define ADC_CR_CLKDIV(n)        ((((n) & 0xFF) << 0))        /*!< The APB clock (PCLK) is divided by (this value plus one) to produce the clock for the A/D */
#define ADC_SAMPLE_RATE_CONFIG_MASK (ADC_CR_CLKDIV(0xFF) | ADC_CR_BITACC(0x01))

/** ADC Sequence Control register bit fields */
#define ADC_SEQ_CTRL_CHANSEL(n)   (1 << (n))                  /*!< Channel select macro */
#define ADC_SEQ_CTRL_CHANSEL_MASK (0xFFF)                     /*!< Channel select mask */

/** ADC hardware trigger sources in SEQ_CTRL */
#define ADC_SEQ_CTRL_HWTRIG_ARM_TXEV     (0 << 12)            /*!< HW trigger input - ARM TXEV */
#define ADC_SEQ_CTRL_HWTRIG_CT16B1_MAT3  (1 << 12)            /*!< HW trigger input - Match output 3 of CT16B1 */
#define ADC_SEQ_CTRL_HWTRIG_CT16B1_MAT2  (2 << 12)            /*!< HW trigger input - Match output 2 of CT16B1 */
#define ADC_SEQ_CTRL_HWTRIG_PIO0_2       (3 << 12)            /*!< HW trigger input - PIO0_2 */
#define ADC_SEQ_CTRL_HWTRIG_PIO0_7       (4 << 12)            /*!< HW trigger input - PIO0_7 */
#define ADC_SEQ_CTRL_HWTRIG_PIO0_8       (5 << 12)            /*!< HW trigger input - PIO0_8 */
#define ADC_SEQ_CTRL_HWTRIG_PIO0_9       (6 << 12)            /*!< HW trigger input - PIO0_9 */
#define ADC_SEQ_CTRL_HWTRIG_PIO2_0       (7 << 12)            /*!< HW trigger input - PIO2_0 */
#define ADC_SEQ_CTRL_HWTRIG_MASK         (0x3F << 12)      /*!< HW trigger input bitfield mask */

/** SEQ_CTRL register bit fields */
#define ADC_SEQ_CTRL_HWTRIG_POLPOS       (1 << 18)            /*!< HW trigger polarity - positive edge */
#define ADC_SEQ_CTRL_HWTRIG_SYNCBYPASS   (1 << 19)            /*!< HW trigger bypass synchronisation */
#define ADC_SEQ_CTRL_START               (1 << 26)            /*!< Start conversion enable bit */
#define ADC_SEQ_CTRL_BURST               (1 << 27)            /*!< Repeated conversion enable bit */
#define ADC_SEQ_CTRL_SINGLESTEP          (1 << 28)            /*!< Single step enable bit */
#define ADC_SEQ_CTRL_LOWPRIO             (1 << 29)            /*!< High priority enable bit (regardless of name) */
#define ADC_SEQ_CTRL_MODE_EOS            (1 << 30)            /*!< Mode End of sequence enable bit */
#define ADC_SEQ_CTRL_SEQ_ENA             (1UL << 31)          /*!< Sequence enable bit */

/** ADC global data register bit fields */
#define ADC_SEQ_GDAT_RESULT_MASK         (0xFFF << 4)         /*!< Result value mask */
#define ADC_SEQ_GDAT_RESULT_BITPOS       (4)                  /*!< Result start bit position */
#define ADC_SEQ_GDAT_THCMPRANGE_MASK     (0x3 << 16)          /*!< Comparion range mask */
#define ADC_SEQ_GDAT_THCMPRANGE_BITPOS   (16)                 /*!< Comparison range bit position */
#define ADC_SEQ_GDAT_THCMPCROSS_MASK     (0x3 << 18)          /*!< Comparion cross mask */
#define ADC_SEQ_GDAT_THCMPCROSS_BITPOS   (18)                 /*!< Comparison cross bit position */
#define ADC_SEQ_GDAT_CHAN_MASK           (0xF << 26)           /*!< Channel number mask */
#define ADC_SEQ_GDAT_CHAN_BITPOS         (26)                  /*!< Channel number bit position */
#define ADC_SEQ_GDAT_OVERRUN             (1 << 30)             /*!< Overrun bit */
#define ADC_SEQ_GDAT_DATAVALID           (1UL << 31)           /*!< Data valid bit */

/** ADC Data register bit fields */
#define ADC_DR_RESULT(n)           ((((n) >> 4) & 0xFFF))           /*!< Macro for getting the ADC data value */
#define ADC_DR_THCMPRANGE_MASK     (0x3 << 16)                      /*!< Comparion range mask */
#define ADC_DR_THCMPRANGE_BITPOS   (16)                              /*!< Comparison range bit position */
#define ADC_DR_THCMPRANGE(n)       (((n) >> ADC_DR_THCMPRANGE_BITPOS) & 0x3)
#define ADC_DR_THCMPCROSS_MASK     (0x3 << 18)                      /*!< Comparion cross mask */
#define ADC_DR_THCMPCROSS_BITPOS   (18)                              /*!< Comparison cross bit position */
#define ADC_DR_THCMPCROSS(n)       (((n) >> ADC_DR_THCMPCROSS_BITPOS) & 0x3)
#define ADC_DR_CHAN_MASK           (0xF << 26)                       /*!< Channel number mask */
#define ADC_DR_CHAN_BITPOS         (26)                              /*!< Channel number bit position */
#define ADC_DR_CHANNEL(n)          (((n) >> ADC_DR_CHAN_BITPOS) & 0xF)      /*!< Channel number bit position */
#define ADC_DR_OVERRUN             (1 << 30)                         /*!< Overrun bit */
#define ADC_DR_DATAVALID           (1UL << 31)                       /*!< Data valid bit */
#define ADC_DR_DONE(n)             (((n) >> 31))

/** ADC low/high Threshold register bit fields */
#define ADC_THR_VAL_MASK            (0xFFF << 4)                     /*!< Threshold value bit mask */
#define ADC_THR_VAL_POS             (4)                              /*!< Threshold value bit position */

/** ADC Threshold select register bit fields */
#define ADC_THRSEL_CHAN_SEL_THR1(n) (1 << ((n) + 1))                 /*!< Select THR1 register for channel n */

/** ADC Interrupt Enable register bit fields */
#define ADC_INTEN_SEQA_ENABLE       (1 << 0)                         /*!< Sequence A Interrupt enable bit */
#define ADC_INTEN_SEQB_ENABLE       (1 << 1)                         /*!< Sequence B Interrupt enable bit */
#define ADC_INTEN_SEQN_ENABLE(seq)  (1 << (seq))                     /*!< Sequence A/B Interrupt enable bit */
#define ADC_INTEN_OVRRUN_ENABLE     (1 << 2)                         /*!< Overrun Interrupt enable bit */
#define ADC_INTEN_CMP_DISBALE       (0)                              /*!< Disable comparison interrupt value */
#define ADC_INTEN_CMP_OUTSIDETH     (1)                              /*!< Outside threshold interrupt value */
#define ADC_INTEN_CMP_CROSSTH       (2)                              /*!< Crossing threshold interrupt value */
#define ADC_INTEN_CMP_MASK          (3)                              /*!< Comparison interrupt value mask */
#define ADC_INTEN_CMP_ENABLE(isel, ch) (((isel) & ADC_INTEN_CMP_MASK) << ((2 * (ch)) + 5))      /*!< Interrupt selection for channel */

/** ADC Flags register bit fields */
#define ADC_FLAGS_THCMP_MASK(ch)    (1 << (ch))                      /*!< Threshold comparison status for channel */
#define ADC_FLAGS_OVRRUN_MASK(ch)   (1 << (12 + (ch)))               /*!< Overrun status for channel */
#define ADC_FLAGS_SEQA_OVRRUN_MASK  (1 << 24)                        /*!< Seq A Overrun status */
#define ADC_FLAGS_SEQB_OVRRUN_MASK  (1 << 25)                        /*!< Seq B Overrun status */
#define ADC_FLAGS_SEQN_OVRRUN_MASK(seq) (1 << (25 + (seq)))          /*!< Seq A/B Overrun status */
#define ADC_FLAGS_SEQA_INT_MASK     (1 << 28)                        /*!< Seq A Interrupt status */
#define ADC_FLAGS_SEQB_INT_MASK     (1 << 29)                        /*!< Seq B Interrupt status */
#define ADC_FLAGS_SEQN_INT_MASK(seq) (1 << (29 + (seq)))             /*!< Seq A/B Interrupt status */
#define ADC_FLAGS_THCMP_INT_MASK    (1 << 30)                        /*!< Threshold comparison Interrupt status */
#define ADC_FLAGS_OVRRUN_INT_MASK   (1UL << 31)                      /*!< Overrun Interrupt status */

/** ADC Trim register bit fields */
#define ADC_TRIM_VRANGE_HIGHV       (0 << 5)                         /*!< Voltage range bit - High volatge (2.7V to 3.6V) */
#define ADC_TRIM_VRANGE_LOWV        (1 << 5)                        /*!< Voltage range bit - Low volatge (1.8V to 2.7V) */

#endif

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


