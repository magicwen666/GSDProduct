#include "DoorMagnet.h"
#include "chip.h"
#include "SysInfo.h"
#include "lock315.h"
#include "network.h"

typedef enum {
    RF_433M_STA_SYNC,     /* 同步码 */
    RF_433M_STA_DATA,     /* 数据 */
} rf_433m_sta_t;

typedef struct {
    volatile rf_433m_sta_t sta;
    volatile unsigned int tick;
    volatile unsigned int htime;
    volatile unsigned int ltime;
    volatile unsigned int data;
    volatile unsigned char flag;
} rf_433m_t;
static rf_433m_t rf_433m;
volatile unsigned char isCheck433M = 0;
volatile unsigned int  DoorMagnetEventTick = 0;

void DoorMagnetHandler(void) {

    rf_433m.tick++;
}

void DoorMagnetMsHandler(void) {

    if (0x00 != (sys_event & SYS_EVENT_DOOR_MAGENT)) {
        if (0 != DoorMagnetEventTick) {
            DoorMagnetEventTick--;
        }
    }
    
}

void DoorMagnetInit(void) {

    syscon_ahb_enable(SYSCON_CLK_IOCON);
    /* P2_1  */
    LPC_IOCON->PIO2_1 = (IOCON_FUNC0 | IOCON_MODE_PULLUP);
    syscon_ahb_enable(SYSCON_CLK_GPIO);
    gpio_set_dir(LPC_GPIO2, 1, GPIO_IN);
    
    gpio_set_interrupt(LPC_GPIO2, 1, GPIO_INTMODE_DROP_EDGE);
    gpio_enable_interrupt(LPC_GPIO2, 1);

    NVIC_EnableIRQ(GPIO2_IRQn);

    rf_433m.sta = RF_433M_STA_SYNC;
    rf_433m.tick = 0;
    rf_433m.htime = 0;
    rf_433m.ltime = 0;
    rf_433m.data = 0;
    rf_433m.flag = 0;
    
    
}

static volatile unsigned char rf_data_cnt = 24;
static volatile unsigned int  rf_data = 0;

static void Rf433Get(void) {

    switch (rf_433m.sta) {
        case RF_433M_STA_SYNC : {
            if ((rf_433m.ltime >= 320) && (rf_433m.ltime <= 340)) {
                rf_433m.sta = RF_433M_STA_DATA;
            } else {
                rf_433m.sta = RF_433M_STA_SYNC;
            }
        } break;
        case RF_433M_STA_DATA : {
            while (1) {
                DOG_FEED();
                if (0 == rf_433m.flag) {
                    continue;
                }
                rf_433m.flag = 0;
                rf_433m.ltime = rf_433m.tick;
                while (!gpio_get_bitval(LPC_GPIO2, 1));
                rf_433m.ltime = rf_433m.tick - rf_433m.ltime;
                rf_433m.tick = 0;
                if ((rf_433m.ltime >= 20) && (rf_433m.ltime <= 40)) {
                    /* data low */
                    rf_data_cnt--;
                    rf_data >>= 1;
                } else {
                    /* data high */
                    rf_data_cnt--;
                    rf_data >>= 1;
                    rf_data |= 0x00800000;
                }
                /* recv complete */
                if (0 == rf_data_cnt) {
                    //LOG("RF 433M Data : %08x\r\n", rf_data);
                    rf_433m.data = rf_data;
                    rf_433m.sta = RF_433M_STA_SYNC;
                    rf_data_cnt = 24;
                    rf_data = 0;
                    return ;
                }
            }
        };
        default : {
        } break;
    }

    
}


void GPIO2_IRQHandler(void) {

    if (FALSE == gpio_get_intstatus(LPC_GPIO2, 1)) {
        return ;
    }
    rf_433m.flag = 1;
    gpio_clear_interrupt(LPC_GPIO2, 1);
}

void DoorMagnetRecv(void) {

    unsigned char bf = 0;
    
    SysInfoBfGet(&bf);
    if (0 == bf) {
        return ;
    }
    if ((0x00 != (sys_event & SYS_EVENT_DOOR_MAGENT)) && (0 == DoorMagnetEventTick)) {
        if (0x00 == (sys_event & SYS_EVENT_OPEN_LOCK)) {
            Lock315Warn();
            NetWorkSendWarn(NET_WARN_TYPE_ILLEGAL_LOCK, NULL);
        }
        sys_event = SYS_EVENT_NONE;
    }
    
    if (0 == rf_433m.flag) {
        return ;
    }
    rf_433m.flag = 0;
    rf_433m.ltime = rf_433m.tick;
    while (!gpio_get_bitval(LPC_GPIO2, 1));
    rf_433m.ltime = rf_433m.tick - rf_433m.ltime;
    rf_433m.tick = 0;
    Rf433Get();
    
    if (0 == SysInfoGetRf433Data()) {
        SysInfoSetRf433Data(rf_433m.data);
    } else {
        if (rf_433m.data != SysInfoGetRf433Data()) {
            rf_433m.data = 0;
            return ;
        }
    }
    //SYSLOG("433 data : %08x\r\n", rf_433m.data);
    DoorMagnetEventTick = 3000;
    sys_event |= SYS_EVENT_DOOR_MAGENT; 
    rf_433m.data = 0;
    
}



