#include "pir.h"
#include "chip.h"
#include "network.h"
#include "SysInfo.h"
#include "led.h"

static volatile unsigned int  pir_ready = PIR_READY_TIME;
static volatile unsigned int  pir_space = PIR_SPACE_TIME;
static volatile unsigned char pir_cnt = 0;
static volatile unsigned int  pir_clear = 0;
static volatile unsigned char pir_flag = 0;

void PirHandler(void) {

    if (0 != pir_ready) {
        pir_ready--;
    }
    
    if (0 != pir_space) {
        pir_space--;
    }

    if (0 != pir_clear) {
        pir_clear--;
    }
    
}

void PirInit(void) {

    syscon_ahb_enable(SYSCON_CLK_IOCON);
    /* PIR P1_11 */
    LPC_IOCON->PIO1_11 |= (IOCON_FUNC0 | IOCON_MODE_PULLUP);
    syscon_ahb_enable(SYSCON_CLK_GPIO);
    gpio_set_dir(LPC_GPIO1, 11, GPIO_IN);
    gpio_set_bitval(LPC_GPIO1, 11, 0);

    //gpio_set_interrupt(LPC_GPIO1, 11, GPIO_INTMODE_RISE_EDGE);
    //gpio_enable_interrupt(LPC_GPIO1, 11);
    //NVIC_EnableIRQ(GPIO1_IRQn);
    pir_ready = PIR_READY_TIME;
    pir_space = PIR_SPACE_TIME;

}

void PirCheck(void) {

    mc_t mc;
    
    SysInfoMcGet(&mc);
    /* this function isn't open */
    if ('0' == mc.flag) {
        return ;
    }
    
    if ((0 == pir_clear) && (1 == pir_flag)) {
        if (0x01 == gpio_get_bitval(LPC_GPIO1, 11)) {
            pir_clear = PIR_CLEAR_TIME;
            pir_flag = 1;
        } else {
            pir_flag = 0;
            pir_cnt = 0;
        }
    }
    if ((0 != pir_space) || (0 != pir_ready)) {
        return ;
    }
    if (0x00 == gpio_get_bitval(LPC_GPIO1, 11)) {
        return ;
    }
    pir_flag = 1;
    pir_clear = PIR_CLEAR_TIME;
    pir_space = PIR_SPACE_TIME;
    pir_cnt++;
    LEDOpen(LED_COLOR_GREEN, 0);
    if (pir_cnt >= (mc.time / PIR_SPACE_TIME)) {
        pir_cnt = 0;
        LOG("门口有人逗留\r\n");
        NetWorkSendWarn(NET_WARN_TYPE_MC, NULL);
    }
    

}



