#include "door.h"
#include "sys.h"
#include "network.h"

static door_t door;
#define DOOR_TICK 5000  /* 5s */

void DoorHander(void) {

    if (0 == door.tick) {
        return ;
    }
    door.tick--;
    
}

void DoorInit(void) {

    syscon_ahb_enable(SYSCON_CLK_IOCON);
    // p1_0 INPUT
    LPC_IOCON->R_PIO1_0 = (IOCON_FUNC1|IOCON_MODE_PULLDOWN|IOCON_DIGMODE_EN);
    syscon_ahb_enable(SYSCON_CLK_GPIO);    
    gpio_set_dir(LPC_GPIO1, 0, GPIO_IN);

    door.sta = DOOR_STA_NONE;
    door.tick = 0;
    
}

door_sta_t DoorGetSta(void) {

    if (GPIO_L == gpio_get_bitval(LPC_GPIO1, 0)) {
        return DOOR_STA_OPEN;
    }
    
    return DOOR_STA_CLOSE;
}

void DoorOpenFeedback(void) {

    if (DOOR_STA_OPEN == door.sta) {
        return ;
    }
    door.sta = DOOR_STA_OPEN;
    NetworkSend(NET_CMD_DOOR_OPEN, 0);
    
}

void DoorCloseFeedback(void) {

    if (DOOR_STA_CLOSE == door.sta) {
        return ;
    }
    door.sta = DOOR_STA_CLOSE;
    NetworkSend(NET_CMD_DOOR_CLOSE, 0);
    
}

unsigned char DoorIsCanCheck(void) {

    if (0 != door.tick) {
        return 1;
    }
    door.tick = DOOR_TICK;

    return 0;
}


