#include "Dahua.h"
#include "BleMaster.h"
#include "chip.h"
#include "delay.h"
#include "wt588d.h"
#include "network.h"
#include "led.h"
#include <string.h>

static unsigned char DahuaFrameBuf[DAHUA_FRAME_MAX_LEN] = {0X00};
static DahuaFrame_t DahuaFrame;

DahuaRet_t DahuaInit(void) {

    unsigned char *pos = DahuaFrameBuf;

    BleMasterInit();
    DahuaFrame.head = pos++;
    DahuaFrame.SlaveAddr = pos++;
    DahuaFrame.CmdGrp = pos++;
    DahuaFrame.CmdCode = pos++;
    DahuaFrame.len = pos++;
    DahuaFrame.data = pos;

    return DAHUA_RET_SUCC;
}

static unsigned char DahuaCalcCheckSum(unsigned char len) {

    unsigned char sum = 0;
    unsigned short i = 0; 

    for (i = 0; i < len; i++) {
        sum += DahuaFrameBuf[i];
    }

    return sum;
}

DahuaRet_t DahuaWatiResp(DahuaFrame_t *frame) {

    unsigned int timeout = 10000;
    unsigned int len = 0;
    unsigned char *pos = DahuaFrameBuf;

    memset(DahuaFrameBuf, 0x00, sizeof(DAHUA_FRAME_MAX_LEN));
    while (timeout--) {
        DOG_FEED();
        delay_ms(1);
        if (BLE_RET_SUCCESS != BleMasterRecv(pos, &len)) {
            continue;
        }
        if (NULL == frame) {
            return DAHUA_RET_SUCC;
        }
        frame->head = pos++;
        frame->SlaveAddr = pos++;
        frame->CmdGrp = pos++;
        frame->CmdCode = pos++;
        frame->len = pos++;
        frame->data = pos;
        frame->chk = frame->data + *(frame->len);
        return DAHUA_RET_SUCC;
    }

    return DAHUA_RET_TIMEOUT;
}

DahuaRet_t DahuaSend(unsigned char CmdGrp, unsigned char CmdCode, unsigned char *data, unsigned char len) {

    memset(DahuaFrameBuf, 0x00, DAHUA_FRAME_MAX_LEN);
    
    *(DahuaFrame.head) = 0xA6;
    *(DahuaFrame.SlaveAddr) = 0;
    *(DahuaFrame.CmdGrp) = CmdGrp;
    *(DahuaFrame.CmdCode) = CmdCode;
    *(DahuaFrame.len) = len;
    if ((0 != len) && (NULL != data)) {
        memcpy(DahuaFrame.data, data, len);
    }
    DahuaFrame.chk = DahuaFrame.data + len;
    *(DahuaFrame.chk) = DahuaCalcCheckSum(DAHUA_FRMAE_LEN(DahuaFrame));

    BleMasterSend(DahuaFrame.head, DAHUA_FRMAE_LEN(DahuaFrame));
{   
    unsigned short i = 0;
    SYSLOG("Frame Data : ");
    for (i = 0; i < DAHUA_FRMAE_LEN(DahuaFrame); i++) {
        SYSLOG("%02x ", DahuaFrameBuf[i]);
    }
    SYSLOG("\r\n");
}
    return DAHUA_RET_SUCC;
}

static DahuaRet_t DahuaDealAlarmInfo(DahuaFrame_t frame) {

    DahuaAlarmInfo_t AlarmInfo;

    if (DAHUA_CMD_UNLOCK_CODE == *(frame.CmdCode)) {
        DahuaUnlockInfo_t info = *((DahuaUnlockInfo_t *)(frame.data));
        unsigned char UserNum[6] = {0x00};
        SYSLOG("type : %d\r\n", info.type);
        SYSLOG("ID   : %02x%02x%02x%02x\r\n", info.id[0], info.id[1], info.id[2], info.id[3]);
        sprintf((char *)UserNum, "%03d", info.id[3]);
        SYSLOG("UserNum : %s\r\n", UserNum);
        NetWorkSendWarn(NET_WARN_TYPE_OPEN_LOCK, UserNum);
        return DAHUA_RET_SUCC;
    }

    if (DAHUA_CMD_ALARM_CODE != *(frame.CmdCode)) {
        return DAHUA_RET_FAIL;
    }

    AlarmInfo = *((DahuaAlarmInfo_t *)(frame.data));
    switch (AlarmInfo.type) {
        case DAHUA_ALARM_TYPE_DOORBELL : {
            SYSLOG("主人,家里来客人了\r\n");
            NetWorkSendWarn(NET_WARN_TYPE_DOORBEEL, NULL);
        } break;
        case DAHUA_ALARM_TYPE_ILLEGAL_USER : {
            SYSLOG("主人,有人非法开锁\r\n");
            NetWorkSendWarn(NET_WARN_TYPE_ILLEGAL_LOCK, NULL);
        } break;
        default : {
        } break;
    }
    
    return DAHUA_RET_SUCC;
}

static DahuaRet_t DahuaDealBleRspInfo(DahuaFrame_t frame) {

    switch (*(frame.CmdCode)) {
        case DAHUA_CMD_NEW_DEV_NOTIF : {
            SYSLOG("关联响应\r\n");
            WT588DStartPlay(WT588D_CONTEX_LOCK_ASSOC_SUCC);
            LEDOpen(LED_COLOR_YELLOW, 3);
            NetWorkSendWarn(NET_WARN_TYPE_LOCK_ASSOC, NULL);
        } break;
        default : {
        } break;
    }
    
    return DAHUA_RET_SUCC;
}

DahuaRet_t DahuaRecv(void) {

    unsigned int len = 0;
    unsigned char *pos = DahuaFrameBuf;
    DahuaFrame_t frame;

    if (BLE_RET_SUCCESS != BleMasterRecv(pos, &len)) {
        return DAHUA_RET_FAIL;
    }
    frame.head = pos++;
    frame.SlaveAddr = pos++;
    frame.CmdGrp = pos++;
    frame.CmdCode = pos++;
    frame.len = pos++;
    frame.data = pos;
    frame.chk = frame.data + *(frame.len);

    switch (*(frame.CmdGrp)) {
        case DAHUA_CMD_ALARM_GRP : {
            DahuaDealAlarmInfo(frame);
        } break;
        case DAHUA_BLE_CMD_GRP : {
            DahuaDealBleRspInfo(frame);
        } break;
        default : {
        } break;
    }

    return DAHUA_RET_SUCC;
}

DahuaRet_t DahuaGetLinkSta(DahuaLinkSta_t *sta) {

    DahuaFrame_t frame;

    DahuaSend(DAHUA_BLE_CMD_GRP, DAHUA_CMD_TEST, NULL, 0);
    if (DAHUA_RET_SUCC != DahuaWatiResp(&frame)) {
        return DAHUA_RET_FAIL;
    }

    if ((DAHUA_BLE_CMD_GRP != *(frame.CmdGrp)) ||
        (DAHUA_CMD_TEST_RSP != *(frame.CmdCode))) {
        return DAHUA_RET_FAIL;
    }
    if (NULL != sta) {
        *sta = (DahuaLinkSta_t )(frame.data[0]);
    }
    
    return DAHUA_RET_SUCC;
}

DahuaRet_t DahuaReset(void) {
    
    DahuaRst_t rst;
    
    rst.time = DAHUA_RST_TIME_DEFAULT;

    DahuaSend(DAHUA_BLE_CMD_GRP, DAHUA_CMD_RST, (unsigned char *)&rst, sizeof(DahuaRst_t));
    if (DAHUA_RET_SUCC != DahuaWatiResp(NULL)) {
        return DAHUA_RET_FAIL;
    }

    return DAHUA_RET_SUCC;
}

DahuaRet_t DahuaGetBindDevice(void) {

    DahuaFrame_t frame;
    unsigned char i = 0;
    unsigned char *pos = NULL;
    
    DahuaSend(DAHUA_BLE_CMD_GRP, DAHUA_CMD_GET_DEV, NULL, 0);
    if (DAHUA_RET_SUCC != DahuaWatiResp(&frame)) {
        return DAHUA_RET_FAIL;
    }

    if (DAHUA_CMD_GET_DEV_RSP != *(frame.CmdCode)) {
        return DAHUA_RET_FAIL;
    }
    LOG("MAC Number : %d\r\n", frame.data[0]);
    pos = frame.data + 1;
    for (i = 0; i < frame.data[0]; i++) {
        LOG("%d : %02x:%02x:%02x:%02x:%02x:%02x\r\n", i + 1, 
            pos[0], pos[1], pos[2], pos[3], pos[4], pos[5]);
        pos += 6;
    }

    return DAHUA_RET_SUCC;
}

DahuaRet_t DahuaNetAllow(DahuaNetAllow_t allow) {

    DahuaFrame_t frame;
    DahuaCommonRsp_t rsp;
    
    DahuaSend(DAHUA_BLE_CMD_GRP, DAHUA_CMD_NET_ALLOW, (unsigned char *)&allow, 1);
    if (DAHUA_RET_SUCC != DahuaWatiResp(&frame)) {
        return DAHUA_RET_FAIL;
    }
    
    if (DAHUA_CMD_COMMON_RSP != *(frame.CmdCode)) {
        return DAHUA_RET_FAIL;
    }
    rsp = *((DahuaCommonRsp_t *)frame.data);
    if ((DAHUA_CMD_NET_ALLOW != rsp.cmd) ||
        (DAHUA_COMMON_RSP_FAIL == rsp.rsp)) {
        return DAHUA_RET_FAIL;
    }
    
    return DAHUA_RET_SUCC;
}

DahuaRet_t DahuaNewDevNotif(DahuaNewDev_t dev) {

    DahuaFrame_t frame;
    DahuaCommonRsp_t rsp;
    
    DahuaSend(DAHUA_BLE_CMD_GRP, DAHUA_CMD_NEW_DEV_NOTIF, (unsigned char *)&dev, sizeof(DahuaNewDev_t));
    if (DAHUA_RET_SUCC != DahuaWatiResp(&frame)) {
        return DAHUA_RET_FAIL;
    }
    
    if (DAHUA_CMD_COMMON_RSP != *(frame.CmdCode)) {
        return DAHUA_RET_FAIL;
    }
    rsp = *((DahuaCommonRsp_t *)frame.data);
    if ((DAHUA_CMD_NEW_DEV_NOTIF != rsp.cmd) ||
        (DAHUA_COMMON_RSP_FAIL == rsp.rsp)) {
        return DAHUA_RET_FAIL;
    }
    
    return DAHUA_RET_SUCC;
}

DahuaRet_t DahuaRegRequest(DahuaRegister_t reg, DahuaBindSta_t *sta) {

    DahuaFrame_t frame;
    
    DahuaSend(DAHUA_BLE_CMD_GRP, DAHUA_CMD_REGISTER, (unsigned char *)&reg, sizeof(DahuaRegister_t));
    if (DAHUA_RET_SUCC != DahuaWatiResp(&frame)) {
        return DAHUA_RET_FAIL;
    }
    if (DAHUA_CMD_REGISTER_RSP != *(frame.CmdCode)) {
        return DAHUA_RET_FAIL;
    }
    if (NULL != sta) {
        *sta = (DahuaBindSta_t)(frame.data[0]);
    }
    
    return DAHUA_RET_SUCC;
}

DahuaRet_t DahuaRegStaNotif(DahuaRegNotif_t *notif) {
    
    return DAHUA_RET_SUCC;
}

DahuaRet_t DahuaSetSecretKey(DahuaSecretKey_t key) {

    DahuaFrame_t frame;
    DahuaCommonRsp_t rsp;
    
    DahuaSend(DAHUA_BLE_CMD_GRP, DAHUA_CMD_SET_SECRETKEY, (unsigned char *)&key, sizeof(DahuaSecretKey_t));
    if (DAHUA_RET_SUCC != DahuaWatiResp(&frame)) {
        return DAHUA_RET_FAIL;
    }
    if (DAHUA_CMD_COMMON_RSP != *(frame.CmdCode)) {
        return DAHUA_RET_FAIL;
    }
    rsp = *((DahuaCommonRsp_t *)frame.data);
    if ((DAHUA_CMD_SET_SECRETKEY != rsp.cmd) ||
        (DAHUA_COMMON_RSP_FAIL == rsp.rsp)) {
        return DAHUA_RET_FAIL;
    }

    return DAHUA_RET_SUCC;
}

DahuaRet_t DahuaSetMatchPwd(int pwd) {

    DahuaFrame_t frame;
    DahuaCommonRsp_t rsp;
    unsigned char data[4] = {0X00};
    unsigned char i = 0;
    
    /* big endian */
    for (i = 4; i > 0; i--) {
        data[i-1] = (unsigned char)(pwd & 0x000000FF);
        pwd >>= 8;
    }
    DahuaSend(DAHUA_BLE_CMD_GRP, DAHUA_CMD_SET_MATCH_PWD, data, 4);
    if (DAHUA_RET_SUCC != DahuaWatiResp(&frame)) {
        return DAHUA_RET_FAIL;
    }
    if (DAHUA_CMD_COMMON_RSP != *(frame.CmdCode)) {
        return DAHUA_RET_FAIL;
    }
    rsp = *((DahuaCommonRsp_t *)frame.data);
    if ((DAHUA_CMD_SET_MATCH_PWD != rsp.cmd) ||
        (DAHUA_COMMON_RSP_FAIL == rsp.rsp)) {
        return DAHUA_RET_FAIL;
    }

    return DAHUA_RET_SUCC;
}

DahuaRet_t DahuaUnlock(DahuaUnlock_t unlock, DahuaUnlockRsp_t *rsp) {

    DahuaFrame_t frame;

    DahuaSend(0x06, DAHUA_CMD_UNLOCK, (unsigned char *)&unlock, sizeof(DahuaUnlock_t));
    if (DAHUA_RET_SUCC != DahuaWatiResp(&frame)) {
        return DAHUA_RET_FAIL;
    }
    if (DAHUA_CMD_UNLOCK_RSP != *(frame.CmdCode)) {
        return DAHUA_RET_FAIL;
    }

    if (NULL != rsp) {
        *rsp = *((DahuaUnlockRsp_t *)(frame.data));
    }
    
    return DAHUA_RET_SUCC;
}

DahuaRet_t DahuaLock(DahuaLock_t lock, DahuaLockRsp_t *rsp) {

    DahuaFrame_t frame;
    DahuaCommonAck_t ack;

    DahuaSend(0x10, DAHUA_CMD_LOCK, (unsigned char *)&lock, sizeof(DahuaLock_t));
    if (DAHUA_RET_SUCC != DahuaWatiResp(&frame)) {
        return DAHUA_RET_FAIL;
    }
    ack.cmd = DAHUA_CMD_LOCK_RSP;
    if ((0x01 != *(frame.CmdGrp)) ||
        (DAHUA_CMD_LOCK_RSP != *(frame.CmdCode))) {
        ack.ack = DAHUA_COMMON_ACK_FAIL;
        DahuaSend(0x20, DAHUA_CMD_ACK, (unsigned char *)&ack, sizeof(DahuaCommonAck_t));
        return DAHUA_RET_FAIL;
    }
    if (NULL != rsp) {
        *rsp = *((DahuaLockRsp_t *)(frame.data));
    }
    ack.ack = DAHUA_COMMON_ACK_SUCC;
    DahuaSend(0x20, DAHUA_CMD_ACK, (unsigned char *)&ack, sizeof(DahuaCommonAck_t));
    
    return DAHUA_RET_SUCC;
}

DahuaRet_t DahuaRecordQuery(DahuaQueryType_t type) {

    DahuaFrame_t frame;
    
    DahuaSend(0x10, DAHUA_CMD_QUERY, (unsigned char *)&type, sizeof(DahuaQueryType_t));
    if (DAHUA_RET_SUCC != DahuaWatiResp(&frame)) {
        return DAHUA_RET_FAIL;
    }

    return DAHUA_RET_SUCC;
}

DahuaRet_t DahuaAlarmControl(DahuaAlarmCtl_t ctl) {
    
    DahuaFrame_t frame;
    
    DahuaSend(0x10, DAHUA_CMD_ALARM_CTL, (unsigned char *)&ctl, sizeof(DahuaAlarmCtl_t));
    if (DAHUA_RET_SUCC != DahuaWatiResp(&frame)) {
        return DAHUA_RET_FAIL;
    }

    return DAHUA_RET_SUCC;
}

DahuaRet_t DahuaUnlockRecordQuery(DahuaUnlockQuery_t query) {

    DahuaFrame_t frame;
    
    DahuaSend(0x06, DAHUA_CMD_UNLOCK_QUERY, (unsigned char *)&query, sizeof(DahuaUnlockQuery_t));
    if (DAHUA_RET_SUCC != DahuaWatiResp(&frame)) {
        return DAHUA_RET_FAIL;
    }

    return DAHUA_RET_SUCC;

}

DahuaRet_t DahuaGetVersion(void) {

    DahuaSend(0x06, DAHUA_CMD_GET_VERSION, NULL, 0);

    return DAHUA_RET_SUCC;
}

DahuaRet_t DahuaDeleteDevice(DahuaDelete_t del) {

    DahuaFrame_t frame;
    unsigned char len = sizeof(DahuaDelete_t);

    if (DAHUA_DELETE_TYPE_ALL == del.type) {
        len -= DAHUA_MAC_LEN;
    }
    
    DahuaSend(DAHUA_BLE_CMD_GRP, DAHUA_CMD_DELETE_DEV, (unsigned char *)&del, len);
    if (DAHUA_RET_SUCC != DahuaWatiResp(&frame)) {
        return DAHUA_RET_FAIL;
    }

    if ((DAHUA_BLE_CMD_GRP != *(frame.CmdGrp)) ||
        (DAHUA_CMD_COMMON_RSP != *(frame.CmdCode))) {
        return DAHUA_RET_FAIL;
    }
    
    return DAHUA_RET_SUCC;
}

DahuaRet_t DahuaLockAssoc(void) {

    DahuaDelete_t del = {DAHUA_DELETE_TYPE_ALL};

    if (DAHUA_RET_SUCC != DahuaDeleteDevice(del)) {
        return DAHUA_RET_FAIL;
    }

    if (DAHUA_RET_SUCC != DahuaReset()) {
        return DAHUA_RET_FAIL;
    }
    
    delay_ms(1000);
    if (DAHUA_RET_SUCC != DahuaNetAllow(DAHUA_NET_ALLOW_START)) {
        return DAHUA_RET_FAIL;
    }

    return DAHUA_RET_SUCC;
}




