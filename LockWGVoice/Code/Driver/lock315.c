#include "lock315.h"
#include "chip.h"
#include "typedef.h"
#include "rftx_open.h"
#include "rfrx_open.h"
#include "delay.h"
#include "SysInfo.h"
#include "network.h"
#include "led.h"
#include "network.h"
#include "wt588d.h"
#include <string.h>

#define LOCK_DATA_MAX_LEN   64
#define LOCK_TX_NORMAL_LEN  0
static unsigned char lockData[LOCK_DATA_MAX_LEN];
static lock_tx_t lockTx;
static lock_rx_t lockRx;
#define LOCK_RECV_TICK  2000
static volatile unsigned short lockRecvTick = 0;
static volatile unsigned short lockOpenEventTick = 0;
#define LOCK_OPEN_EVENT_TICK   10000
#define LOCK_ASSOC_STA_CLOSE    0
#define LOCK_ASSOC_STA_OPEN     1
static volatile unsigned char lockAssocSta = LOCK_ASSOC_STA_CLOSE;
#define LOCK_ASSOC_TIMEOUT    30000
static volatile unsigned short lockAssocTick = 0;

void LockHandler(void) {

    if (0 != lockRecvTick) {
        lockRecvTick--;
    }

    if (0x00 != (sys_event & SYS_EVENT_OPEN_LOCK)) {
        if (0 != lockOpenEventTick) {
            lockOpenEventTick--;
        } else {
            sys_event &= ~SYS_EVENT_OPEN_LOCK;
        }
    }
    
    if (0 != lockAssocTick) {
        lockAssocTick--;
        if (0 == lockAssocTick) {
            lockAssocSta = LOCK_ASSOC_STA_CLOSE;
        }
    }
    
}

void Lock315Init(void) {

    unsigned char id[16] = {0x00};
    unsigned int snr = 0;
    
    syscon_ahb_enable(SYSCON_CLK_IOCON);
    /* RF_IN P2_6 */
    LPC_IOCON->PIO2_6 |= (IOCON_FUNC0 | IOCON_MODE_PULLUP);
    /* RF_OUT P2_0 */
    LPC_IOCON->PIO2_0 |= (IOCON_FUNC0 | IOCON_MODE_PULLUP);
    /* RF_EN p2_8 */
    LPC_IOCON->PIO2_8 |= (IOCON_FUNC0 | IOCON_MODE_PULLUP);

    syscon_ahb_enable(SYSCON_CLK_GPIO);
    gpio_set_dir(LPC_GPIO2, 6, GPIO_IN);
    gpio_set_dir(LPC_GPIO2, 0, GPIO_OUT);
    gpio_set_dir(LPC_GPIO2, 8, GPIO_OUT);
    RfTxOFF();

    lockTx.type = DEV_TYPE_END;
    SysInfoIDGet(id);
    lockTx.id.h = ((id[2] - 0x30) << 4) | (id[3] - 0x30);
    lockTx.id.m = ((id[4] - 0x30) << 4) | (id[5] - 0x30);
    lockTx.id.l = ((id[6] - 0x30) << 4) | (id[7] - 0x30);
    SysInfoLockSnrGet(&snr);
    lockTx.snr = snr;
    lockTx.data = lockData;
    lockRx.data = lockData;
    
}

unsigned char Lock315Recv(void) {

    unsigned char rbuf[64] = {0x00};
    lock_user_t lock_user;
    unsigned char ret = 0;

    ret = RfRx(rbuf, 64);
    if ((OK != ret) && (NOOK != ret)) {
        return 1;
    }
    
    memcpy(&lockRx, rbuf, LOCK_RX_OFFSET);
    memcpy(lockRx.data, rbuf+LOCK_RX_OFFSET, sizeof(rbuf)-LOCK_RX_OFFSET);
    if ((LOCK_CMD_ASSOC_DEV == (lockRx.cmd & 0x7F)) && 
        (LOCK_ASSOC_STA_OPEN == lockAssocSta)) {
        LOG("关联设备\r\n");
        //BeepShortRing();
        WT588DStartPlay(WT588D_CONTEX_LOCK_ASSOC_SUCC);
        LEDOpen(LED_COLOR_YELLOW, 3);
        NetWorkSendWarn(NET_WARN_TYPE_LOCK_ASSOC, NULL);
        SysInfoLockIDSave(lockRx.id);
        lockAssocSta = LOCK_ASSOC_STA_CLOSE;
        return 0;
    }
#ifdef DEBUG
    LOG("Frame Num : %d\r\n", lockRx.num);
    LOG("Frame Len : %d\r\n", lockRx.len);
    LOG("Frame Type: %02x\r\n", lockRx.type);
    LOG("Frame ID  : [%02x:%02x:%02x]\r\n", lockRx.id.h, lockRx.id.m, lockRx.id.l);
    LOG("Frame Snr : %ld\r\n", lockRx.snr);
    LOG("Frame Cmd : %d\r\n", lockRx.cmd);
#endif
    /* recv many */
    if (0 != lockRecvTick) {
        return 1;
    }
    lockRecvTick = LOCK_RECV_TICK;
    
    if (DEV_TYPE_LOCK != lockRx.type) {
        return 1;
    }
    
    if (1 == SysInfoLockIDIsAssoc(lockRx.id)) {
        SYSLOG("该锁未关联\r\n");
        return 1;
    }
    LEDOpen(LED_COLOR_YELLOW, 3);
    switch (lockRx.cmd & 0x7F) {
        case LOCK_CMD_CLOSE_INFO : {
            /*
            sprintf((char *)&lock_user, "%02x%02x", lockRx.data[1], lockRx.data[0]);
            if (LOCK_USER_TYPE_MGR == lock_user.type) {
                SYSLOG("管理用户 :");
            } else {
                SYSLOG("普通用户 :");
            }
            SYSLOG(" %s 关锁\r\n", lock_user.num);*/
        } break;
        case LOCK_CMD_OPEN_INFO : {
            sys_event |= SYS_EVENT_OPEN_LOCK;
            lockOpenEventTick = LOCK_OPEN_EVENT_TICK;
            sprintf((char *)&lock_user, "%02x%02x", lockRx.data[1], lockRx.data[0]);
            if (0x80 == (lockRx.cmd & 0x80)) {
                LOG("电量过低,请及时更换电池!!!\r\n");
                NetWorkSendWarn(NET_WARN_TYPE_LOW_VOL, NULL);
            }
            if (LOCK_USER_TYPE_MGR == lock_user.type) {
                LOG("管理用户 :");
            } else {
                LOG("普通用户 :");
            }
            SYSLOG(" %s 开锁\r\n", lock_user.num);
            NetWorkSendWarn(NET_WARN_TYPE_OPEN_LOCK, lock_user.num);
        } break;
        case LOCK_CMD_ILLEGAL_USER : {  /* 非法用户 */
            LOG("非法开锁\r\n");
            NetWorkSendWarn(NET_WARN_TYPE_ILLEGAL_LOCK, NULL);
        } break;
        case LOCK_CMD_DOORBELL : {  /* 门铃呼叫 */
            LOG("门铃响起\r\n");
            NetWorkSendWarn(NET_WARN_TYPE_DOORBEEL, NULL);
        } break;
        case LOCK_CMD_WARN_3S : {
            unsigned char arg[2] = {NET_WARN_LONG_CLICK, 0x00};
            LOG("长按告警\r\n");
            NetWorkSendWarn(NET_WARN_TYPE_WARN, arg);
        } break;
        case LOCK_CMD_WARN_3TIMES : {
            unsigned char arg[2] = {NET_WARN_THREE_CLICK, 0x00};
            LOG("3次告警\r\n");
            NetWorkSendWarn(NET_WARN_TYPE_WARN, arg);
        } break;
        case LOCK_CMD_WARN_CANCEL : {
            unsigned char arg[2] = {NET_WARN_SINGLE_CLICK, 0x00};
            NetWorkSendWarn(NET_WARN_TYPE_WARN, arg);
        } break;
    }
    
    return 0;
}

unsigned char Lock315RecvManageUser(unsigned char *user) {

    unsigned char rbuf[64] = {0x00};
    lock_user_t lock_user;
    unsigned char ret = 0;
    
    ret = RfRx(rbuf, 64);
    if ((OK != ret) && (NOOK != ret)) {
        return 1;
    }
    
    if (0 != lockRecvTick) {
        return 1;
    }
    lockRecvTick = LOCK_RECV_TICK;
    
    memcpy(&lockRx, rbuf, LOCK_RX_OFFSET);
    memcpy(lockRx.data, rbuf+LOCK_RX_OFFSET, sizeof(rbuf)-LOCK_RX_OFFSET);
    ret = SysInfoLockIDIsAssoc(lockRx.id);
    if (1 == ret) {
        LOG("该锁未关联\r\n");
        return 1;
    } 
    LEDOpen(LED_COLOR_YELLOW, 3);
    switch (lockRx.cmd & 0x7F) {
        case LOCK_CMD_OPEN_INFO : {
            sprintf((char *)&lock_user, "%02x%02x", lockRx.data[1], lockRx.data[0]);
            if (LOCK_USER_TYPE_MGR == lock_user.type) {
                SYSLOG("管理用户 : %s\r\n", lock_user.num);
                if (NULL != user) {
                    memcpy(user, lock_user.num, 3);
                }
                return 0;
            } 
        } break;
        default : {
        } break;
    }

    return 1;
}

void Lock315Send(lock_cmd_t cmd, unsigned char *data, unsigned char len) {

    lockTx.cmd = cmd;
    if (NULL != data) {
        memcpy(lockTx.data, data, len);
    }
    RfTx((u8 *)&lockTx, LOCK_TX_NORMAL_LEN+len, LOCK_SEND_TIMES);
    RfTxOFF();
    lockTx.snr++;
    SysInfoLock315SnrSave(lockTx.snr);
    LEDOpen(LED_COLOR_YELLOW, 3);
}

void Lock315Open(void) {
    
    Lock315Send(LOCK_CMD_OPEN, NULL, 0);
}

void Lock315Close(void) {

    Lock315Send(LOCK_CMD_CLOSE, NULL, 0);
}

void Lock315Query(void) {
    
    Lock315Send(LOCK_CMD_QUERY, NULL, 0);
}

void Lock315Assoc(void) {

    lockAssocSta = LOCK_ASSOC_STA_OPEN;
    lockAssocTick = LOCK_ASSOC_TIMEOUT;
    //WT588DStartPlay(WT588D_CONTEX_SEND_LOCK_ASSOC);
    Lock315Send(LOCK_CMD_ASSOC_LOCK, NULL, 0);
}

void Lock315Warn(void) {
    
    Lock315Send(LOCK_CMD_WARN_REMOTE, NULL, 0);
}

void Lock315WarnCancel(void) {
    
    Lock315Send(LOCK_CMD_WARN_CANCEL, NULL, 0);
}




