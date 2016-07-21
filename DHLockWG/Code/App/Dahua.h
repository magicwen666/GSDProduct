#ifndef __BLEDAHUA_H__
#define __BLEDAHUA_H__

#pragma pack(1)

#define DAHUA_CMD_TEST          0x01
#define DAHUA_CMD_TEST_RSP      0x02
#define DAHUA_CMD_GET_DEV       0x03
#define DAHUA_CMD_GET_DEV_RSP   0x04
#define DAHUA_CMD_RST           0x05
#define DAHUA_CMD_RST_RSP       0x06
#define DAHUA_CMD_NET_ALLOW     0x10
#define DAHUA_CMD_NEW_DEV_NOTIF 0x61
#define DAHUA_CMD_DELETE_DEV    0x12
#define DAHUA_CMD_REGISTER      0x25
#define DAHUA_CMD_REGISTER_RSP  0x26
#define DAHUA_CMD_REG_STA_NOTIF 0x26
#define DAHUA_CMD_SET_SECRETKEY 0x28
#define DAHUA_CMD_SET_MATCH_PWD 0x30
#define DAHUA_CMD_UNLOCK        0x20
#define DAHUA_CMD_UNLOCK_RSP    0x21
#define DAHUA_CMD_LOCK          0x43
#define DAHUA_CMD_LOCK_RSP      0x18
#define DAHUA_CMD_ACK           0x51
#define DAHUA_CMD_COMMON_RSP    0x80
#define DAHUA_CMD_QUERY         0x41
#define DAHUA_CMD_ALARM_CTL     0x44
#define DAHUA_CMD_UNLOCK_QUERY  0x22
#define DAHUA_CMD_GET_VERSION   0x60

#define DAHUA_CMD_ALARM_GRP     0x01
#define DAHUA_CMD_ALARM_CODE    0x11
#define DAHUA_CMD_UNLOCK_CODE   0x10

#define DAHUA_MAC_LEN   6

typedef enum {
    DAHUA_RET_SUCC,
    DAHUA_RET_FAIL,
    DAHUA_RET_ARGERR,
    DAHUA_RET_TIMEOUT,
} DahuaRet_t;

#define DAHUA_BLE_CMD_GRP   0x06

#define DAHUA_FRAME_MAX_LEN     262
#define DAHUA_FRMAE_LEN(_f)     (*((_f).len) + 6)

typedef struct {
    unsigned char *head;     /* 0xA6 */
    unsigned char *SlaveAddr;
    unsigned char *CmdGrp;
    unsigned char *CmdCode;
    unsigned char *len;
    unsigned char *data;
    unsigned char *chk;
} DahuaFrame_t;

typedef struct {
    unsigned char status;
} DahuaTestRsp_t;

#define DAHUA_RST_TIME_DEFAULT  0
typedef struct {
    unsigned char time; /* s */
} DahuaRst_t;

typedef struct {
    unsigned char status;
} DahuaRstRsp_t;


typedef enum {
    DAHUA_NET_ALLOW_FINISH = 0,
    DAHUA_NET_ALLOW_START,
} DahuaNetAllow_t;

/* from slave to master */
#define DAHUA_COMMON_RSP_FAIL   0
#define DAHUA_COMMON_RSP_SUCC   1
typedef struct {
    unsigned char cmd;
    unsigned char rsp;
} DahuaCommonRsp_t;

/* from master to slave */
#define DAHUA_COMMON_ACK_FAIL   2
#define DAHUA_COMMON_ACK_SUCC   1
typedef struct {
    unsigned char cmd;
    unsigned char ack;
} DahuaCommonAck_t;

#define DAHUA_DEV_IS_EXIST  0
#define DAHUA_DEV_IS_NEW    1
typedef struct {
    unsigned char NewFlag;
    unsigned char mac[DAHUA_MAC_LEN];    
} DahuaNewDev_t;

typedef enum {
    DAHUA_REGISTER_START = 0,
    DAHUA_REGISTER_FINISH,
} DahuaRegister_t;

typedef enum {
    DAHUA_BIND_STA_ENTER = 0,
    DAHUA_BIND_STA_NO_ENTER,
} DahuaBindSta_t;

typedef enum {
    DAHUA_REG_NOTIF_STA_NEW_BIND,
    DAHUA_REG_NOTIF_STA_EXIST_BIND,
    DAHUA_REG_NOTIF_STA_TIMEOUT,
} DahuaRegNotifSta_t;

typedef struct {
    DahuaRegNotifSta_t sta;
    unsigned char mac[DAHUA_MAC_LEN];
} DahuaRegNotif_t;

typedef struct {
    unsigned char key[DAHUA_MAC_LEN];   /* secret key */
} DahuaSecretKey_t;

typedef struct {
    unsigned char mac[DAHUA_MAC_LEN];   /* user's mac */
    unsigned char delay;    /* hold time , s */
    unsigned char zone;     /* time zone */
    unsigned int  time;     /* curr time */
} DahuaUnlock_t;

typedef struct {
    unsigned char rst;
    unsigned char BatSta;   /* bat status */
    unsigned int  pwd[5];   /* 5 group secret key */
} DahuaUnlockRsp_t;

typedef struct {
    unsigned char mac[DAHUA_MAC_LEN];   /* user's mac */
    unsigned char delay;    /* hold time , s */
    unsigned char zone;     /* time zone */
    unsigned int  time;     /* curr time */
} DahuaLock_t;

typedef struct {
    unsigned char rst;
    unsigned char BatSta;   /* bat status */
    unsigned int  pwd[5];   /* 5 group secret key */
} DahuaLockRsp_t;

typedef enum {
    DAHUA_ALARM_TYPE_NONE,
    DAHUA_ALARM_TYPE_BREAK,
    DAHUA_ALARM_TYPE_STRESS,
    DAHUA_ALARM_TYPE_KEY_OPEN,
    DAHUA_ALARM_TYPE_LOW_VOLTAGE,
    DAHUA_ALARM_TYPE_FALSE_LOCK,
    DAHUA_ALARM_TYPE_DOORBELL,
    DAHUA_ALARM_TYPE_ILLEGAL_USER,
} DahuaAlarmType_t;

typedef enum {
    DAHUA_ACTION_START = 0x01,
    DAHUA_ACTION_STOP,
    DAHUA_ACTION_PULSE,
} DahuaAction_t;

typedef struct {
    DahuaAlarmType_t type;
    DahuaAction_t action;
    unsigned int time;
} DahuaAlarmInfo_t;

typedef enum {
    DAHUA_QUERY_TYPE_DOORBELL,
    DAHUA_QUERY_TYPE_WARN1,
    DAHUA_QUERY_TYPE_WARN2,
    DAHUA_QUERY_TYPE_BREAK, /* */
} DahuaQueryType_t;

typedef struct {
    unsigned char type;
    unsigned char action;
} DahuaAlarmCtl_t;

typedef enum {
    DAHUA_UNLOCK_QUERY_TYPE_TIME,
    DAHUA_UNLOCK_QUERY_TYPE_TODAY,
    DAHUA_UNLOCK_QUERY_TYPE_YESTERDAY,
    DAHUA_UNLOCK_QUERY_TYPE_100,
    DAHUA_UNLOCK_QUERY_TYPE_200,
    DAHUA_UNLOCK_QUERY_TYPE_ALL,
} DahuaUnlockQueryType_t;

typedef struct {
    DahuaUnlockQueryType_t type;
    unsigned int StartTime;
    unsigned int EndTime;
} DahuaUnlockQuery_t;

typedef enum {
    DAHUA_LINK_STA_NO = 0,
    DAHUA_LINK_STA_YES = 1,
} DahuaLinkSta_t;

typedef enum {
    DAHUA_DELETE_TYPE_ALONE,
    DAHUA_DELETE_TYPE_ASSIGN,
    DAHUA_DELETE_TYPE_ALL,
} DahuaDeleteType_t;

typedef struct {
    DahuaDeleteType_t type;
    unsigned char mac[DAHUA_MAC_LEN];
} DahuaDelete_t;

typedef enum {
    DAHUA_UNLOCK_TYPE_CARD = 1,
    DAHUA_UNLOCK_TYPE_PWD,
    DAHUA_UNLOCK_TYPE_FINGER,
} DahuaUnlockType_t;

typedef struct {
    unsigned char ret;
    DahuaUnlockType_t type;
    unsigned char err;
    unsigned char id[4];
} DahuaUnlockInfo_t;

#pragma pack()

DahuaRet_t DahuaInit(void);
DahuaRet_t DahuaWatiResp(DahuaFrame_t *frame);
DahuaRet_t DahuaSend(unsigned char CmdGrp, unsigned char CmdCode, unsigned char *data, unsigned char len);
DahuaRet_t DahuaRecv(void);
DahuaRet_t DahuaGetLinkSta(DahuaLinkSta_t *sta);
DahuaRet_t DahuaReset(void);
DahuaRet_t DahuaGetBindDevice(void);
DahuaRet_t DahuaNetAllow(DahuaNetAllow_t allow);
DahuaRet_t DahuaNewDevNotif(DahuaNewDev_t dev);
DahuaRet_t DahuaRegRequest(DahuaRegister_t reg, DahuaBindSta_t *sta);
DahuaRet_t DahuaRegStaNotif(DahuaRegNotif_t *notif);
DahuaRet_t DahuaSetSecretKey(DahuaSecretKey_t key);
DahuaRet_t DahuaSetMatchPwd(int pwd);
DahuaRet_t DahuaUnlock(DahuaUnlock_t unlock, DahuaUnlockRsp_t *rsp);
DahuaRet_t DahuaLock(DahuaLock_t lock, DahuaLockRsp_t *rsp);
DahuaRet_t DahuaRecordQuery(DahuaQueryType_t type);
DahuaRet_t DahuaAlarmControl(DahuaAlarmCtl_t ctl);
DahuaRet_t DahuaUnlockRecordQuery(DahuaUnlockQuery_t query);
DahuaRet_t DahuaGetVersion(void);
DahuaRet_t DahuaDeleteDevice(DahuaDelete_t del);
DahuaRet_t DahuaLockAssoc(void);

#endif



