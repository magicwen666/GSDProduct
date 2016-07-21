;/**************************************************************************//**
; * @file     startup_LPC112x.s
; * @brief    CMSIS Cortex-M0 Core Device Startup File for
; *           NXP LPC112x Device Series
; * @version  V1.02
; * @date     19. January 2015
; *
; * @note
; * Copyright (C) 2013 - 2015 ARM Limited. All rights reserved.
; *
; * @par
; * ARM Limited (ARM) is supplying this software for use with Cortex-M
; * processor based microcontrollers.  This file can be freely distributed
; * within development tools that are supporting such ARM based processors.
; *
; * @par
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; *
; ******************************************************************************/

; *------- <<< Use Configuration Wizard in Context Menu >>> ------------------

; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000400

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors

__Vectors       DCD     __initial_sp              ;     Top of Stack
                DCD     Reset_Handler             ;     Reset Handler
                DCD     NMI_Handler               ;     NMI Handler
                DCD     HardFault_Handler         ;     Hard Fault Handler
                DCD     0                         ;     Reserved
                DCD     0                         ;     Reserved
                DCD     0                         ;     Reserved
                DCD     0                         ;     Reserved
                DCD     0                         ;     Reserved
                DCD     0                         ;     Reserved
                DCD     0                         ;     Reserved
                DCD     SVC_Handler               ;     SVCall Handler
                DCD     0                         ;     Reserved
                DCD     0                         ;     Reserved
                DCD     PendSV_Handler            ;     PendSV Handler
                DCD     SysTick_Handler           ;     SysTick Handler

                ; External Interrupts
                DCD     PIO0_0_IRQHandler         ;  0: PIO0_0
                DCD     PIO0_1_IRQHandler         ;  1: PIO0_1
                DCD     PIO0_2_IRQHandler         ;  2: PIO0_2
                DCD     PIO0_3_IRQHandler         ;  3: PIO0_3
                DCD     PIO0_4_IRQHandler         ;  4: PIO0_4
                DCD     PIO0_5_IRQHandler         ;  5: PIO0_5
                DCD     PIO0_6_IRQHandler         ;  6: PIO0_6
                DCD     PIO0_7_IRQHandler         ;  7: PIO0_7
                DCD     PIO0_8_IRQHandler         ;  8: PIO0_8
                DCD     PIO0_9_IRQHandler         ;  9: PIO0_9
                DCD     PIO0_10_IRQHandler        ; 10: PIO0_10
                DCD     PIO0_11_IRQHandler        ; 11: PIO0_11
                DCD     PIO1_0_IRQHandler         ; 12: PIO1_0
                DCD     ADC_B_IRQHandler          ; 13: ADC_B
                DCD     SSP1_IRQHandler           ; 14: SSP1
                DCD     I2C_IRQHandler            ; 15: I2C
                DCD     CT16B0_IRQHandler         ; 16: CT16B0
                DCD     CT16B1_IRQHandler         ; 17: CT16B1
                DCD     CT32B0_IRQHandler         ; 18: CT32B0
                DCD     CT32B1_IRQHandler         ; 19: CT32B1
                DCD     SSP0_IRQHandler           ; 20: SSP0
                DCD     UART0_IRQHandler          ; 21: UART0
                DCD     UART1_IRQHandler          ; 22: UART1
                DCD     UART2_IRQHandler          ; 23: UART2
                DCD     ADC_A_IRQHandler          ; 24: ADC_A
                DCD     WDT_IRQHandler            ; 25: WDT
                DCD     BOD_IRQHandler            ; 26: BOD
                DCD     FLASH_IRQHandler          ; 27: FLASH
                DCD     GPIO3_IRQHandler          ; 28: GPIO3
                DCD     GPIO2_IRQHandler          ; 29: GPIO2
                DCD     GPIO1_IRQHandler          ; 30: GPIO1
                DCD     GPIO0_IRQHandler          ; 31: GPIO0

; <h> Code Read Protection
;   <o> Code Read Protection  <0xFFFFFFFF=>CRP Disabled
;                             <0x12345678=>CRP Level 1
;                             <0x87654321=>CRP Level 2
;                             <0x43218765=>CRP Level 3 (ARE YOU SURE?)
;                             <0x4E697370=>NO ISP (ARE YOU SURE?)
; </h>
                IF      :LNOT::DEF:NO_CRP
                AREA    |.ARM.__at_0x02FC|, CODE, READONLY
                DCD     0xFFFFFFFF
                ENDIF

                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ;IMPORT hard_fault_handler_c
                ;TST LR, #4
                ;ITE EQ
                ;MRSEQ R0, MSP
                ;MRSNE R0, PSP
                ;B hard_fault_handler_c 
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP

Default_Handler PROC

                EXPORT  PIO0_0_IRQHandler         [WEAK]
                EXPORT  PIO0_1_IRQHandler         [WEAK]
                EXPORT  PIO0_2_IRQHandler         [WEAK]
                EXPORT  PIO0_3_IRQHandler         [WEAK]
                EXPORT  PIO0_4_IRQHandler         [WEAK]
                EXPORT  PIO0_5_IRQHandler         [WEAK]
                EXPORT  PIO0_6_IRQHandler         [WEAK]
                EXPORT  PIO0_7_IRQHandler         [WEAK]
                EXPORT  PIO0_8_IRQHandler         [WEAK]
                EXPORT  PIO0_9_IRQHandler         [WEAK]
                EXPORT  PIO0_10_IRQHandler        [WEAK]
                EXPORT  PIO0_11_IRQHandler        [WEAK]
                EXPORT  PIO1_0_IRQHandler         [WEAK]
                EXPORT  ADC_B_IRQHandler          [WEAK]
                EXPORT  SSP1_IRQHandler           [WEAK]
                EXPORT  I2C_IRQHandler            [WEAK]
                EXPORT  CT16B0_IRQHandler         [WEAK]
                EXPORT  CT16B1_IRQHandler         [WEAK]
                EXPORT  CT32B0_IRQHandler         [WEAK]
                EXPORT  CT32B1_IRQHandler         [WEAK]
                EXPORT  SSP0_IRQHandler           [WEAK]
                EXPORT  UART0_IRQHandler          [WEAK]
                EXPORT  UART1_IRQHandler          [WEAK]
                EXPORT  UART2_IRQHandler          [WEAK]
                EXPORT  ADC_A_IRQHandler          [WEAK]
                EXPORT  WDT_IRQHandler            [WEAK]
                EXPORT  BOD_IRQHandler            [WEAK]
                EXPORT  FLASH_IRQHandler          [WEAK]
                EXPORT  GPIO3_IRQHandler          [WEAK]
                EXPORT  GPIO2_IRQHandler          [WEAK]
                EXPORT  GPIO1_IRQHandler          [WEAK]
                EXPORT  GPIO0_IRQHandler          [WEAK]

PIO0_0_IRQHandler
PIO0_1_IRQHandler
PIO0_2_IRQHandler
PIO0_3_IRQHandler
PIO0_4_IRQHandler
PIO0_5_IRQHandler
PIO0_6_IRQHandler
PIO0_7_IRQHandler
PIO0_8_IRQHandler
PIO0_9_IRQHandler
PIO0_10_IRQHandler
PIO0_11_IRQHandler
PIO1_0_IRQHandler
ADC_B_IRQHandler
SSP1_IRQHandler
I2C_IRQHandler
CT16B0_IRQHandler
CT16B1_IRQHandler
CT32B0_IRQHandler
CT32B1_IRQHandler
SSP0_IRQHandler
UART0_IRQHandler
UART1_IRQHandler
UART2_IRQHandler
ADC_A_IRQHandler
WDT_IRQHandler
BOD_IRQHandler
FLASH_IRQHandler
GPIO3_IRQHandler
GPIO2_IRQHandler
GPIO1_IRQHandler
GPIO0_IRQHandler

                B       .

                ENDP


                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
__user_initial_stackheap

                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR

                ALIGN

                ENDIF


                END
