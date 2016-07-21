#ifndef __LOCK315_H__
#define __LOCK315_H__

#include "chip.h"

#define LOCK_SEND_TIMES 20

typedef enum {
    DEV_TYPE_LOCK = 0xFC,
    DEV_TYPE_END = 0xC1,    /* 0xc0~0xcf */
} dev_type_t;

typedef enum {
    LOCK_USER_TYPE_MGR = '8',       /* 管理用户 */
    LOCK_USER_TYPE_NORMAL = '0',    /* 普通用户 */
} lock_user_type_t;

typedef enum {
    LOCK_CMD_OPEN = 0x01,   /* lock open */
    LOCK_CMD_CLOSE,         /* lock close */
    LOCK_CMD_QUERY,         /* query */
    LOCK_CMD_CLOSE_INFO,    /* close information */
    LOCK_CMD_ASSOC_DEV,     /* assoc device */
    LOCK_CMD_ASSOC_LOCK,    /* assoc lock */
    LOCK_CMD_OPEN_INFO,     /* open information */
    LOCK_CMD_ILLEGAL_USER,  /* illegal user */
    LOCK_CMD_DELETE,        /* delete user */
    LOCK_CMD_BREAK_DOOR = 20,
    LOCK_CMD_WARN_3S = 23,
    LOCK_CMD_WARN_3TIMES = 24,
    LOCK_CMD_WARN_REMOTE = 25,
    LOCK_CMD_CLEAR_USER = 28,
    LOCK_CMD_DOORBELL = 29,
    LOCK_CMD_WARN_CANCEL = 30,
} lock_cmd_t;

#pragma pack(1)

typedef struct {
    unsigned char h;
    unsigned char m;
    unsigned char l;
} lock_id_t;

typedef struct {
    dev_type_t type;    /* device type */
    lock_id_t  id;      /* 315 enddevice id */
    unsigned int snr;   /* */
    lock_cmd_t cmd;     /* lock cmd */
    unsigned char *data;
} lock_tx_t;

#define LOCK_RX_OFFSET  11
typedef struct {
    unsigned char num;  /* frame num */
    unsigned char len;  /* frame len */    
    dev_type_t type;    /* device type */
    lock_id_t id;       /* device id */
    unsigned int snr;   /* */
    lock_cmd_t cmd;     /* lock cmd */
    unsigned char *data;
} lock_rx_t;

typedef struct {
    lock_user_type_t type;
    unsigned char num[3];
} lock_user_t;

#pragma pack()

void LockHandler(void);
void Lock315Init(void);
unsigned char Lock315Recv(void);
unsigned char Lock315RecvManageUser(unsigned char *user);
void Lock315Send(lock_cmd_t cmd, unsigned char *data, unsigned char len);
void Lock315Open(void);
void Lock315Close(void);
void Lock315Query(void);
void Lock315Assoc(void);
void Lock315Warn(void);
void Lock315WarnCancel(void);


#endif


