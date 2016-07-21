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
    NET_WARN_TYPE_PIR = 0x01,   /* 红外探测器 */
    NET_WARN_TYPE_DOOR,         /* 门磁探测器 */
    NET_WARN_TYPE_SMOKE,        /* 烟雾探测器 */
    NET_WARN_TYPE_NETINFO,      /* 网络信息 */
    NET_WARN_TYPE_WG_RESET,
    NET_WARN_TYPE_PIR_MATCH_STA,
    NET_WARN_TYPE_DOOR_MATCH_STA,
    NET_WARN_TYPE_SMOKE_MATCH_STA,
    NET_WARN_TYPE_LOCAL_WARNING,
    NET_WARN_TYPE_CANCEL_WARNING,
    NET_WARN_TYPE_UPDATE_FINISH,
};

enum {
    NET_CTL_TYPE_APP_ONLINE = 0x01, /* APP在线  */
    NET_CTL_TYPE_APP_OFFLINE,       /* APP离线  */
    NET_CTL_TYPE_BF,                /* 布防 */
    NET_CTL_TYPE_CF,                /* 撤防 */
    NET_CTL_TYPE_REMOTE_WARN,       /* 远程报警 */
    NET_CTL_TYPE_OUTLET,
    NET_CTL_TYPE_DOOR_LINKAGE,      /* 门磁联动 */
    NET_CTL_TYPE_PIR_LINKAGE,       /* 红外联动 */
    NET_CTL_TYPE_SMOKE_LINKAGE,     /* 烟雾联动 */
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

#define NET_SERVER_IP     "111.206.45.12"
#define NET_SERVER_PORT   30399


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


