#include "sys.h"
#include "board_key.h"
#include "board_led.h"
#include "board_rs485.h"
#include "board_light.h"
#include "board_ir.h"
#include "network.h"
#include "door.h"
#include "board_wdt.h"
#include "delay.h"
#include "board_flash.h"

//#define BL_WEAKUP   0

int fputc(int ch, FILE *f) {
    
    uart0_send_byte(ch);
    
    return ch;
}

static void LogInit(void) {
    
    syscon_ahb_enable(SYSCON_CLK_IOCON);
    LPC_IOCON->PIO1_6 |= (IOCON_FUNC1 | IOCON_MODE_PULLUP);
    LPC_IOCON->PIO1_7 |= (IOCON_FUNC1 | IOCON_MODE_PULLUP);
    
    LOGInit();
    LOG("the system start up[%s %s]\r\n", __DATE__, __TIME__);

}

static void DriverInit(void) {

    LogInit();
    board_time_init();
    board_key_init();
    board_led_init();
    board_rs485_init();
    board_ir_init();
#ifdef BL_WEAKUP
    board_irps_init();
#endif
    board_light_init();
    DoorInit();
    board_wdt_init();
    
}

int main(void) {

    DriverInit();
    SysInfoInit();
    NetworkInit(1);
    
    while (1) {
        FEED_DOG;
        KeyTask();
        IrTask();
 #ifdef BL_WEAKUP
        PsTask();
 #endif
        NetWorkTask();
        WaitTask();
        DoorTask();
    }
    
}




#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line){ 
    
    printf("Assert Fail: file %s on line %d\r\n", file, line);
    while (1);
}
#endif

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/

