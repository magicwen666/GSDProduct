#include "JwLock.h"
#include "SysInfo.h"
#include "BleS02.h"
#include "shell.h"
#include "chip.h"
#include "delay.h"
#include "frame.h"
#include "KeyMgr.h"
#include <string.h>

typedef enum {
    JWLOCK_WAIT_RSP_NONE,
    JWLOCK_WAIT_RSP_OPEN,
    JWLOCK_WAIT_RSP_CLOSE,
    JWLOCK_WAIT_RSP_STA,
    JWLOCK_WAIT_RSP_SET_TIME,
    JWLOCK_WAIT_RSP_OPEN_RECORD,
} JwLockWaitRsp_t;

static volatile JwLockWaitRsp_t JwLockWaitRsp = JWLOCK_WAIT_RSP_NONE;

#define JWLOCK_FRAME_RECV_HEAD  {0xFF ,0xFF, 0x44, 0x5A, 0x46, 0x5F, 0x4C, 0x3A}
#define JWLOCK_FRAME_HEAD       {0x44, 0x5A, 0x46, 0x5F, 0x43, 0x4D, 0x44, 0x3A, 0x06, 0x07, 0x00, 0x00, 0x03}
#define JWLOCK_FRAME_HEAD_LEN   13
#define JWLOCK_FRAME_WRITE      0x53
#define JWLOCK_FRAME_READ       0x52
#define JWLOCK_CMD_OPEN         0x4F
#define JWLOCK_CMD_CLOSE        0x43
#define JWLOCK_CMD_SET_TIME     0x54
#define JWLOCK_CMD_GET_STA      0x53
#define JWLOCK_CMD_OPEN_RECORD  0x52

#define JWLOCK_TICK_TIMEOUT  5000
static volatile unsigned int JwLockTick = 0;
static volatile unsigned char JwLockOpenFlag = 0;

void JwLockHandler(void) {

    if (0 == JwLockOpenFlag) {
        return ;
    }
    if (0 != JwLockTick) {
        JwLockTick--;
        return ;
    } 
    JwLockOpenFlag = 0;
    JwLockClose();
    
}

void JwLockInit(void) {

    BleS02Info_t info[BLES02_SCAN_MAX_NUM];
    unsigned char cnt = 0;
    unsigned char i = 0;
    sys_room_t room;
    unsigned char ScanFlag = 0;
    unsigned int timeout = 5000;
    unsigned char data[128] = {0X00};
    JwLockSta_t sta;
    unsigned char mac[32] = {0X00};

    /* check room number */
    SysInfoRoomNumGet(&room);
    if ((0x00 == room.floor) && (0x00 == room.num)) {
        LOG("Must Config Room Number\r\n");
        while (1) {
            DOG_FEED();
            ShellCmdRecv();
            KeyMgrListened();
        }
    }

    /* have mac, link */
    if (0 == SysInfoBleIsNull(mac)) {
        unsigned char cnt = 0;
        BleS02Standby();
        while (1 == BleS02LinkWithMac(mac)) {
            DOG_FEED();
            KeyMgrListened();
            /* link fail, retry scan */
            if (cnt++ >= 3) {
                goto JWLOCK_SCAN;
            }
        }
        return ;
    }

JWLOCK_SCAN :
    
    while (1) {
        DOG_FEED();
        KeyMgrListened();
        /* scan bluetooth slave */
        if (0 == ScanFlag) {
            BleS02Standby();
            LOG("scan bluetooth slave \r\n");
            if (1 == BleS02GlobalScan(info, &cnt)) {
                continue ;
            }
            if (0 != cnt) {
                ScanFlag = 1;
            }
        }
        /* find room number equal */
        for (i = 0; i < cnt; i++) {
            if (1 == BleS02LinkWithMac(info[i].mac)) {
                continue;
            }
            /* link success */
            JwLockGetSta();
            LOG("Wait Resp Data \r\n");
            timeout = 5000;
            while (timeout--) {
                DOG_FEED();
                delay_ms(1);
                if (1 == BleS02Recv(data, NULL)) {
                    continue ;
                }
                if (JWLOCK_WAIT_RSP_STA != JwLockWaitRsp) {
                    break;
                }
                JwLockWaitRsp = JWLOCK_WAIT_RSP_NONE;
                sta = *((JwLockSta_t *)data);
                if ((room.floor != sta.info.floor) ||
                    (room.num != sta.info.num)) {
                    break;
                }
                /* equal */
                SysInfoBleInfoSet(info[i].mac, info[i].name);
                return;
            }
        }
        /* not find */
        ScanFlag = 0;
    }
    
}

static void JwLockDealLockStaInfo(unsigned char *data) {

    JwLockSta_t sta;
    
    sta = *((JwLockSta_t *)data);
    LOG("20%02x年%02x月%02x日 %02x:%02x:%02x 星期%d\r\n", 
        sta.time.year, sta.time.month, sta.time.day,
        sta.time.hour, sta.time.min, sta.time.sec, sta.time.week);
    LOG("电池电量:%0.2f\r\n", sta.bat * 0.047266);
    LOG("记录 : %d\r\n", (sta.record.high << 8) | sta.record.low);
    LOG("门状态 : ");
    switch (sta.sta) {
        case JWLOCK_DOOR_STA_OPEN : {
            LOG("开\r\n");
        } break;
        case JWLOCK_DOOR_STA_CLOSE : {
            LOG("关\r\n");
        } break;
        case JWLOCK_DOOR_STA_BACK_LOCK : {
            LOG("反锁\r\n");
        } break;
    }

    LOG("房间号:%02x:%02x:%02x:%02x\r\n", 
        sta.info.gaffer, sta.info.clean,
        sta.info.floor, sta.info.num);
    
}

static JwLockTime_t JwLockCreateTime(unsigned char min, unsigned hour, unsigned char *date) {

    JwLockTime_t time;

    time.min = min;
    time.hour = hour;
    time.day = date[0] & 0x1F;  /* 5bit */
    time.month = ((date[1] & 0xFE) << 3) | ((date[0] & 0xE0) >> 5); /* 4bit */
    time.year = date[1] >> 1;

    return time;
}

static void JwLockDealOpenRecord(unsigned char *data, unsigned int len) {
    
    JwLockOpenRecord_t record[5];
    unsigned char i = 0;
    JwLockTime_t time;
    
    len = (len - 2) / sizeof(JwLockOpenRecord_t);
    for (i = 0; i < len; i++) {
        record[i] = *((JwLockOpenRecord_t *)data);
        LOG("ID   : %02x : %02x\r\n", record[i].id.high, record[i].id.low);
        time = JwLockCreateTime(record[i].min, record[i].hour, record[i].date);
        LOG("Date :%d_%d_%d %02x:%02x\r\n", 
            time.year, time.month, time.day, time.hour, time.min);
        LOG("Attr : %02x\r\n", record[i].attr);
        data += sizeof(JwLockOpenRecord_t);
        LOG("---------------------------------\r\n");
    }
}

void JwLockRecv(void) {

    unsigned char data[256] = {0X00};
    unsigned int len = 0;

    if (1 == BleS02Recv(data, &len)) {
        return ;
    }

    switch (JwLockWaitRsp) {
        case JWLOCK_WAIT_RSP_STA : {
            JwLockDealLockStaInfo(data);
        } break;
        case JWLOCK_WAIT_RSP_OPEN_RECORD : {
            JwLockDealOpenRecord(data, len);
        } break;
        case JWLOCK_WAIT_RSP_NONE : {
            unsigned char head[] = JWLOCK_FRAME_RECV_HEAD;
            if (0 != memcmp(data, head, 8)) {
                break;
            }
            FrameSend(FRAME_CMD_LOCK_OPEN_EVENT, NULL, 0);
        } break;
        default : {
        } break;
    }
    JwLockWaitRsp = JWLOCK_WAIT_RSP_NONE;
}

void JwLockOpen(void) {

    unsigned char data[64] = JWLOCK_FRAME_HEAD;
    unsigned char *pos = data + JWLOCK_FRAME_HEAD_LEN;
    
    *pos++ = JWLOCK_FRAME_WRITE;
    *pos++ = JWLOCK_CMD_OPEN;
    JwLockWaitRsp = JWLOCK_WAIT_RSP_OPEN;
    BleS02Send(data, JWLOCK_FRAME_HEAD_LEN + 2);
    JwLockOpenFlag = 1;
    JwLockTick = JWLOCK_TICK_TIMEOUT;
}

void JwLockClose(void) {
    
    unsigned char data[64] = JWLOCK_FRAME_HEAD;
    unsigned char *pos = data + JWLOCK_FRAME_HEAD_LEN;
    
    *pos++ = JWLOCK_FRAME_WRITE;
    *pos++ = JWLOCK_CMD_CLOSE;
    JwLockWaitRsp = JWLOCK_WAIT_RSP_CLOSE;
    BleS02Send(data, JWLOCK_FRAME_HEAD_LEN + 2);
    JwLockOpenFlag = 0;
}

void JwLockGetSta(void) {
    
    unsigned char data[64] = JWLOCK_FRAME_HEAD;
    unsigned char *pos = data + JWLOCK_FRAME_HEAD_LEN;
    
    *pos++ = JWLOCK_FRAME_READ;
    *pos++ = JWLOCK_CMD_GET_STA;
    JwLockWaitRsp = JWLOCK_WAIT_RSP_STA;
    BleS02Send(data, JWLOCK_FRAME_HEAD_LEN + 2);
}

void JwLockSetTime(JwLockTime_t time) {

    unsigned char data[64] = JWLOCK_FRAME_HEAD;
    unsigned char *pos = data + JWLOCK_FRAME_HEAD_LEN;

    data[8] = 0x0D;
    *pos++ = JWLOCK_FRAME_WRITE;
    *pos++ = JWLOCK_CMD_SET_TIME;
    memcpy(pos, &time, sizeof(JwLockTime_t));
    JwLockWaitRsp = JWLOCK_WAIT_RSP_SET_TIME;
    BleS02Send(data, JWLOCK_FRAME_HEAD_LEN + 2 + sizeof(JwLockTime_t));
}

void JwLockGetOpenRecord(unsigned int start, unsigned char num) {

    unsigned char data[64] = JWLOCK_FRAME_HEAD;
    unsigned char *pos = data + JWLOCK_FRAME_HEAD_LEN;
    unsigned char len = 0;

    data[8] = 0x08;
    *pos++ = JWLOCK_FRAME_READ;
    *pos++ = JWLOCK_CMD_OPEN_RECORD;
    if (start < 256) {
        *pos++ = start & 0x00FF;
        len = 4;
    } else {
        *pos++ = (start & 0xFF00 >> 8);
        *pos++ = start & 0x00FF;
        len = 5;
    }
    *pos++ = num;
    JwLockWaitRsp = JWLOCK_WAIT_RSP_OPEN_RECORD;
    BleS02Send(data, JWLOCK_FRAME_HEAD_LEN + len);
}

