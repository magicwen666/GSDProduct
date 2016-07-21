#ifndef __NETWORK_H__
#define __NETWORK_H__

#define NET_FRAME_HEAD  0xFD
#define NET_FRAME_TAIL  0xFE

typedef enum {
    NET_DEV_TYPE_NONE,
    NET_DEV_TYPE_SGW,   /* s1网关 */
    NET_DEV_TYPE_ELEC,  /* 取电开关 */
    NET_DEV_TYPE_RC,    /* 门牌控制面板 */
    NET_DEV_TYPE_RCU,   /* 客房控制器 */
} net_dev_type_t;

typedef enum {
    NET_CMD_LOAD_CLOSE,
    NET_CMD_LOAD_OPEN,
    NET_CMD_ASSOC_CLOSE,
    NET_CMD_ASSOC_OPEN,
    NET_CMD_DOOR_CLOSE,
    NET_CMD_DOOR_OPEN,
    NET_CMD_ASSOC_SET,
    NET_CMD_SAVE_SET,
    NET_CMD_CANCEL_SET,
    NET_CMD_DEL_SET,
    NET_CMD_MAINKEY,
} net_cmd_t;

#pragma pack(1)
typedef struct {
    unsigned char head;
    net_dev_type_t type;
    unsigned char len;
    unsigned char dst;
    unsigned char src;
    net_cmd_t cmd;
    unsigned char keyval;
    unsigned char tail;
} net_frame_tx_t;

typedef struct {
    net_dev_type_t type;
    unsigned char len;
    unsigned char dst;
    unsigned char src;
    net_cmd_t cmd;
    unsigned char keyval;
} net_frame_rx_t;

#pragma pack()

void NetworkInit(unsigned char id);
void NetworkSend(net_cmd_t cmd, unsigned char keyval);
unsigned char NetworkRecv(net_frame_rx_t *rx);

#endif


