#include "wt588d.h"
#include "chip.h"

#define WT588D_DATA(v)  gpio_set_bitval(LPC_GPIO2, 5, v)
#define WT588D_BUSY     gpio_get_bitval(LPC_GPIO3, 5)

void WT588DInit(void) {

    syscon_ahb_enable(SYSCON_CLK_IOCON);
    /* p2_5  */
    LPC_IOCON->PIO2_5 |= IOCON_FUNC0 | IOCON_MODE_PULLUP;
    LPC_IOCON->PIO3_5 |= IOCON_FUNC0 | IOCON_MODE_PULLUP;
    syscon_ahb_enable(SYSCON_CLK_GPIO);
    gpio_set_dir(LPC_GPIO2, 5, GPIO_OUT);
    gpio_set_dir(LPC_GPIO3, 5, GPIO_IN);

    WT588DAdjustVolume(WT588D_VOLUME_8);
}

static void WT588DDelay5ms(void) {

    unsigned int cnt = 4900;
    unsigned char ms = 5;
    
    while (ms--) {
        while (cnt--);
        cnt = 4900;
    }
}

static void WT588DDelay600us(void) {

    unsigned char cnt = 4;
    unsigned int us = 556;
    
    while (us--) {
        while (cnt--);
        cnt = 3;
    }
}

static void WT588DDelay35ms(void) {

    unsigned int cnt = 4900;
    unsigned char ms = 35;
    
    while (ms--) {
        while (cnt--);
        cnt = 4900;
    }
}

static void WT588DDelay200us(void) {

    unsigned char cnt = 4;
    unsigned int us = 191;
    
    while (us--) {
        while (cnt--);
        cnt = 3;
    }
}


void WT588DSendByte(unsigned char byte) {

    unsigned char i = 0;

    WT588D_DATA(0);
    WT588DDelay5ms();

    for (i = 0; i < 8; i++) {
        WT588D_DATA(1);
        if (byte & 0x01) {  /* higl : low = 600 : 200, 1 */
            WT588DDelay600us();
            WT588D_DATA(0);
            WT588DDelay200us();
        } else {            /* higl : low = 200 : 600, 0 */
            WT588DDelay200us();
            WT588D_DATA(0);
            WT588DDelay600us();
        }
        byte >>= 1;
    }
    
    WT588D_DATA(1);
    WT588DDelay35ms();
    
}

void WT588DStartPlay(unsigned char addr) {

    WT588DSendByte(addr);
}

void WT588DStopPlay(void) {

    WT588DSendByte(WT588D_STOP_PLAY);
}

void WT588DLoopPlay(void) {

    WT588DSendByte(WT588D_LOOP_PLAY);
}

void WT588DAdjustVolume(WT588DVolume_t volume) {

    WT588DSendByte(volume);
}

