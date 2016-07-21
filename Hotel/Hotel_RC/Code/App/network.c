#include "network.h"
#include "board_rs485.h"
#include <string.h>

static net_frame_tx_t net_frame_tx;
static net_frame_rx_t net_frame_rx;

void NetworkInit(unsigned char id) {

    net_frame_tx.head = NET_FRAME_HEAD;
    net_frame_tx.type = NET_DEV_TYPE_RC;
    net_frame_tx.len = 4;
    net_frame_tx.dst = 0xFF;
    net_frame_tx.src = id;
    net_frame_tx.keyval = 0x00;
    net_frame_tx.tail = NET_FRAME_TAIL;
    
}

void NetworkSend(net_cmd_t cmd, unsigned char keyval) {

    net_frame_tx.cmd = cmd;
    net_frame_tx.keyval = keyval;
    
    board_rs485_write_nbyte(&net_frame_tx, sizeof(net_frame_tx_t));
    
}

unsigned char NetworkRecv(net_frame_rx_t *rx) {

    uint16_t len = 0;

    if (SUCC != board_rs485_read(&net_frame_rx, &len)) {
        return 1;
    }
#if 0
    {
        unsigned char i = 0;
        unsigned char *p = (unsigned char *)&net_frame_rx;
        for (i = 0; i < len; i++) {
            LOG("%02x ", *p++);
        }
        LOG("\r\n");
    }
#endif
    if ((NET_DEV_TYPE_RC != net_frame_rx.type) &&
        (NET_DEV_TYPE_RCU!= net_frame_rx.type)) {
        return 1;
    }
    if (NULL != rx) {
        memcpy(rx, &net_frame_rx, sizeof(net_frame_rx_t));
    }

    return 0;
}


