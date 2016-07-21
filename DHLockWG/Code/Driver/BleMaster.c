#include "BleMaster.h"
#include "chip.h"
#include <string.h>

#define BleInterrupt    UART1_IRQHandler

#define BLE_MASTER_MAX_LEN  300
static unsigned char BleRBuf[BLE_MASTER_MAX_LEN] = {0x00};
#define BLE_RECV_IDLE   0
#define BLE_RECV_BUSY   1
#define BLE_RECV_FINISH 2
static volatile unsigned char BleRecvFlag = BLE_RECV_IDLE;
#define BLE_RECV_MASK   0x3FFF
static volatile unsigned short BleRLen = 0x00;

void BleInterrupt(void) {

    volatile unsigned char r = 0;

    r = LPC_UART1->RBR;
    if (BLE_RECV_BUSY == BleRecvFlag) {
        BleRBuf[BleRLen++] = r;
        if (BleRLen != (BleRBuf[4] + 6)) {
            return;
        }
        /* recv finish */
        BleRecvFlag = BLE_RECV_FINISH;
        return ;
    }
    if ((r == 0xA6) && (BLE_RECV_IDLE == BleRecvFlag)) {
        BleRBuf[BleRLen++] = r;
        BleRecvFlag = BLE_RECV_BUSY;
    }
    
}

ble_ret_t BleMasterInit(void) {

    syscon_ahb_enable(SYSCON_CLK_IOCON);
    /* U1_TX P0_6 : 22 */
    LPC_IOCON->PIO0_6 |= IOCON_FUNC3 | IOCON_MODE_PULLUP;
    /* U1_RX P0_7 : 23 */
    LPC_IOCON->PIO0_7 |= IOCON_FUNC3 | IOCON_MODE_PULLUP;

    uart1_init(115200);

    return BLE_RET_SUCCESS;
}

ble_ret_t BleMasterSend(void *data, int len) {

    char *p = (char *)data;

    while (len--) {
        uart1_send_byte(*p++);
    }
    
    return BLE_RET_SUCCESS;
}

ble_ret_t BleMasterRecv(void *data, unsigned int *len) {

    if (BLE_RECV_FINISH != BleRecvFlag) {
        return BLE_RET_FAILURE;
    }
    BleRecvFlag = BLE_RECV_IDLE;
    
    if (NULL != len) {
        *len = BleRLen;
    }
    if (NULL != data) {
        memcpy(data, BleRBuf, BleRLen);   
    }
#if 1  
{   
        unsigned int i = 0;
        SYSLOG("Resp : ");
        for (i = 0; i < BleRLen; i++) {
            SYSLOG("%02x ", BleRBuf[i]);
        }
        SYSLOG("\r\n");
}
#endif
    BleRLen = 0;
    memset(BleRBuf, 0x00, BLE_MASTER_MAX_LEN);

    return BLE_RET_SUCCESS;
}


