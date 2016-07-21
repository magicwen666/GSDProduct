#ifndef __NETWORK_H__
#define __NETWORK_H__

#define NET_FRAME_SOF   0X02
#define NET_FRAME_EOF   0X03

enum {
    NET_CMD_LOGIN = 0x01,   /* 注册/登录 */
    NET_CMD_HEART,          /* 心跳包 */
    NET_CMD_WARN,           /* 设备报警 */
    NET_CMD_APP_UPDATE,     /* 应用升级 */
};

enum {
    NET_WARN_TYPE_PICK_LOCK = 0x01, /* 撬锁 */
    NET_WARN_TYPE_PICK_DOOR,        /* 撬门 */
    NET_WARN_TYPE_MC,               /* 门口侦测 */
    NET_WARN_TYPE_WARN,             /* 静默告警 */
    NET_WARN_TYPE_OPEN_LOCK,        /* 合法开锁 */
    NET_WARN_TYPE_ILLEGAL_LOCK,     /* 非法开锁 */
    NET_WARN_TYPE_DOORBEEL,         /* 门铃联动 */
    NET_WARN_TYPE_LOW_VOL,          /* 低电量 */
    NET_WARN_TYPE_WG_RESET,         /* 网关重置 */
    NET_WARN_TYPE_NETINFO,          /* 网络信息 */
    NET_WARN_TYPE_LOCK_ASSOC,       /* 锁已关联 */
    NET_WARN_TYPE_UPDATE_FINISH,    /* 升级完成 */
};

enum {
    NET_WARN_THREE_CLICK = '1',     /* 三击 */
    NET_WARN_LONG_CLICK,            /* 长按 */
    NET_WARN_SINGLE_CLICK,          /* 单击 */
};

enum {
    NET_CTL_TYPE_APP_ONLINE = 0x01, /* APP在线  */
    NET_CTL_TYPE_APP_OFFLINE,       /* APP离线  */
    NET_CTL_TYPE_WARN_REMOTE,       /* 远程报警 */
    NET_CTL_TYPE_WARN_CANCEL,       /* 取消报警 */
    NET_CTL_TYPE_BF,                /* 入侵布防 */
    NET_CTL_TYPE_OLOCK_REMOTE,      /* 远程开锁 */
    NET_CTL_TYPE_CLOCK_REMOTE,      /* 远程关锁 */
    NET_CTL_TYPE_MC,                /* 移动侦测 */
    NET_CTL_TYPE_WIRELESS_PWD,      /* 无线密码 */
    NET_CTL_TYPE_LOCK_ASSOC,        /* 锁关联 */
    NET_CTL_TYPE_RESET,             /* 恢复出厂设置 */
    NET_CTL_TYPE_APP_UPDATE,        /* 应用升级 */
};

enum {
    NET_UPDATE_REQUEST = 1,
    NET_UPDATE_TRANSMIT,
};

#define NET_RSP_SUCCESS '0'
#define NET_RSP_FAILURE '1'

#define NET_FRAME_LEN(_p)   (6 + *((unsigned short *)(_p).len))
#define NET_FRAME_MAX_LEN   256

#pragma pack(1)
typedef struct {
    unsigned char *sof;
    unsigned char *cmd;
    unsigned short *len;
    unsigned char *data;
    unsigned char *lrc;
    unsigned char *eof;
} net_frame_t;

enum {
    NET_LINK_STA_GET_IP = '2',
    NET_LINK_STA_LINK_YES = '3',
    NET_LINK_STA_LINK_NO = '4',
};

typedef struct {
    unsigned char sta;
    unsigned int  id;
    unsigned char type[4];  /* TCP/UDP */
    unsigned char ip[16];
    unsigned int port;
} net_link_info_t;

typedef enum {
    NET_WAIT_RSP_NULL =  0x00,
    NET_WAIT_RSP_LOGIN = 0x01,
    NET_WAIT_RSP_HEART = 0x02,
    NET_WAIT_RSP_WARN =  0x04,
    NET_WAIT_RSP_CTL  =  0x08,
} net_wait_rsp_t;


typedef unsigned int net_tick_t;
#define APP_LINE_FLAG_ONLINE    1
#define APP_LINE_FLAG_OFFLINE   0
#define NET_LOGIN_TIMEOUT       3000
#define NET_HEART_OFFLINE_TICK  3000    /* 3s */
#define NET_HEART_ONLINE_TICK   5000
#define NET_HEART_RSP_TIMEOUT   2500    /* 2.5s */

typedef struct {
    unsigned char AppLineFlag;
    unsigned char MutexLink;    /* 0:single link 1: mutex link */
    net_tick_t LoginTick;
    net_tick_t HeartTick;
    net_tick_t HeartRspTick;
    unsigned char HeartChkFlag;
    net_wait_rsp_t WaitRsp;
    unsigned char JoinFlag;     /* 是否加入wifi */
    unsigned int port;
} network_t;

#pragma pack()

//#define NET_SERVER_IP     "192.168.1.100"
//#define NET_SERVER_PORT   8000
#define NET_SERVER_IP     "111.206.45.12"
#define NET_SERVER_PORT1  30066
#define NET_SERVER_PORT2  30151  


void NetWorkHandler(void);
void NetWorkInit(void);
unsigned char NetWorkDataParse(net_frame_t *frame);
unsigned char NetWorkRecv(void);
unsigned char NetWorkCtlTypeParse(net_frame_t *frame, unsigned int *type, unsigned char *data);
void NetWorkGetLinkInfo(net_link_info_t *info);
void NetWorkSwapServerPort(void);
void NetWorkDataCreate(unsigned char cmd, unsigned char *data);
unsigned char NetWorkCreateLink(net_link_info_t *info);
unsigned char NetWorkCloseLink(void);
unsigned char NetWorkSend(unsigned int id);
unsigned char NetWorkLogin(void);
unsigned char NetWorkSendWarn(unsigned char type, unsigned char *data);
unsigned char NetWorkSendHeart(void);
unsigned char NetWorkCheckHeart(void);
unsigned char NetWorkSendRsp(unsigned char cmd, unsigned char rsp);
void NetWorkSetRspType(net_wait_rsp_t rsp);
void NetWorkClrRspType(net_wait_rsp_t rsp);
unsigned char NetWorkIsRspSuccess(net_wait_rsp_t rsp);
void NetWorkReportResetFlag(void);
void NetWorkDealAppUpdate(unsigned char *data);

#endif


