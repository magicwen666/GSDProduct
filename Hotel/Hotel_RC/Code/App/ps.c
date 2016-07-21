#include "ps.h"
#include "sys.h"
#include "board_ir.h"
#include "board_led.h"

ps_t g_ps;

void PsInit(void) {

    g_ps.delay_tick = PS_BL_TICK;
    g_ps.bl_sta = PS_BL_STATE_ON;

}

void PsHandler(void) {

    if (0 != g_ps.delay_tick ) {
        g_ps.delay_tick --;
    }
    
}

void PsReset(void) {

    g_ps.delay_tick = PS_BL_TICK;
}

void PsOn(void) {

    board_irps_start();
}

void PsOff(void) {

    board_irps_stop();
}

void PsSleep(void) {

    rgb_t color_tmp = {0,0,0};

    PsOn();
    board_led_display(LEDALL, color_tmp);
    board_led_display_dis();
    g_ps.bl_sta = PS_BL_STATE_OFF;
    
}


void PsWake(void) {

    PsOff();
    PsReset();
    board_led_display_en();
    board_led_refresh();
    g_ps.bl_sta = PS_BL_STATE_ON;
    
}



