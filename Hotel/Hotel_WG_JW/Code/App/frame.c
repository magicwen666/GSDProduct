#include "frame.h"
#include "rs485.h"
#include "chip.h"
#include "JwLock.h"
#include "UpdateInfo.h"
#include "delay.h"
#include <string.h>

static void FrameInit(pFrame_t frame) {

    unsigned char dst[FRAME_ADDR_LEN] = {0xFF, 0XFF, 0XFF, 0XFF};
    unsigned char src[FRAME_ADDR_LEN] = {0xC3, 0x00, 0x00, 0x01};

    frame->head = FRAME_HEAD;
    frame->tail = FRAME_TAIL;
    frame->type = FRAME_JWLOCK_WG_TYPE;
    memcpy(frame->data.dst, dst, FRAME_ADDR_LEN);
    memcpy(frame->data.src, src, FRAME_ADDR_LEN);
    
}

void FrameDataCreate(void *data, pFrame_t frame) {

    unsigned char *pos = (unsigned char *)data;
    
    if (NULL == data) {
        return ;
    }
    *pos++ = frame->head;
    *pos++ = frame->type;
    *pos++ = frame->len;
    memcpy(pos, frame->data.dst, FRAME_ADDR_LEN);
    pos += FRAME_ADDR_LEN;
    memcpy(pos, frame->data.src, FRAME_ADDR_LEN);
    pos += FRAME_ADDR_LEN;
    *pos++ = frame->data.cmd;
    memcpy(pos, frame->data.data, FRAME_DATA_DATA_LEN(frame));
    pos += FRAME_DATA_DATA_LEN(frame);
    *pos++ = frame->data.chk;
    *pos++ = frame->tail;
    
}

static unsigned char FrameCmdIsValid(FrameCmd_t cmd) {

    switch (cmd) {
        case FRAME_CMD_LOCK_OPEN :
        case FRAME_CMD_LOCK_CLOSE :
        case FRAME_CMD_GET_OPEN_RECORD : 
        case FRAME_CMD_RSP_SYSTEM_TIME : 
        case FRAME_CMD_FIRMWARE_UPDATE :
        case FRAME_CMD_RESPON_UPDATE : {
        } return 0;
    }

    return 1;
}

unsigned char FrameDataParse(void *data, pFrame_t frame) {

    unsigned char *pos = (unsigned char *)data;
    
    if ((NULL == data) || (NULL == frame)) {
        return 1;
    }
    frame->head = *pos++;
    if (FRAME_HEAD != frame->head) {
        return 1;
    }
    frame->type = *pos++;
    if (FRAME_JWLOCK_WG_TYPE != frame->type) {
        return 1;
    }
    frame->len = *pos++;
    memcpy(frame->data.dst, pos, FRAME_ADDR_LEN);
    pos += FRAME_ADDR_LEN;
    memcpy(frame->data.src, pos, FRAME_ADDR_LEN);
    pos += FRAME_ADDR_LEN;
    frame->data.cmd = (FrameCmd_t)*pos++;
    if (1 == FrameCmdIsValid(frame->data.cmd)) {
        return 1;
    }
    frame->data.data = pos;
    pos += FRAME_DATA_DATA_LEN(frame);
    frame->data.chk = *pos++;
    frame->tail = *pos;
    if (FRAME_TAIL != frame->tail) {
        return 1;
    }

    return 0;
}

void FrameSend(FrameCmd_t cmd, void *arg, unsigned char len) {

    Frame_t frame;
    unsigned char SendData[FRAME_MAX_LEN] = {0x00};
    
    FrameInit(&frame);
    frame.data.cmd = cmd;
    if (NULL == arg) {
        frame.len = sizeof(FrameData_t) - 4;
    } else {
        frame.data.data = (unsigned char *)arg;
        frame.len = sizeof(FrameData_t) - 4 + len;
    }
    frame.data.chk = 0x00;
    FrameDataCreate(SendData, &frame);
    RS485Send(SendData, FRAME_TOTAL_LEN(frame));
#if 0
{
    unsigned char i = 0;
    LOG("Frame Send : ");
    for (i = 0; i < FRAME_TOTAL_LEN(frame); i++) {
        LOG("%02x ", SendData[i]);
    }
    LOG("\r\n");
}
#endif
    
}

#define UPDATE_TIMEOUT  10000
static void FrameDealFirmwareUpdate(Frame_t frame, unsigned char *RecvData) {

    UpdateInfo_t info;
    char *pos1 = NULL;
    char *pos2 = NULL;
    char tmp[32] = {0x00};
    int PackId = 0;
    int intTmp = 0;
    int PackSize = 0;
    unsigned int AppStoreAddr = APP_CODE_SECTOR;
    unsigned int UpdateTimeout = 0;

    /* PackSize */
    pos1 = strchr((char *)frame.data.data, ':') + 1;
    pos2 = strchr(pos1, ';');
    memcpy(tmp, pos1, pos2-pos1);
    sscanf(tmp, "%d", &PackSize);
    info.size = PackSize;
    LOG("PackSize:%d\r\n", PackSize);
    /* md5(32) */
    pos1 = strchr(pos2, ':') + 1;
    pos2 = strchr(pos1, ';');
    memcpy(info.md5, pos1, pos2-pos1);
    /* Erase Store App Flash */
    UpdateInfoAppCodeErase();
    LOG("Firmware Update Start\r\n");

    while (1) {
        DOG_FEED();
        delay_ms(1);
        /* request data */
        if (0 == (UpdateTimeout--)) {
            char arg[8] = {0X00};
            UpdateTimeout = UPDATE_TIMEOUT;
            sprintf((char *)arg, "%d", PackId);
            LOG("Request ID : %d\r\n", PackId);
            FrameSend(FRAME_CMD_REQUEST_UPDATE, arg, strlen(arg));
        }
        
        if (1 == RS485Recv(RecvData, NULL)) {
            continue ;
        }
        if (1 == FrameDataParse(RecvData, &frame)) {
            continue ;
        }
        if (FRAME_CMD_RESPON_UPDATE != frame.data.cmd) {
            UpdateTimeout = 0;
            continue;
        }
        /* PackID */
        pos1 = (char *)frame.data.data;
        pos2 = strchr(pos1, ',');
        memset(tmp, 0x00, sizeof(tmp));
        memcpy(tmp, pos1, pos2 - pos1);
        sscanf(tmp, "%d", &intTmp);
        LOG("PackId : %d\r\n", intTmp);
        if (intTmp != PackId) {
            UpdateTimeout = 0;
            continue;
        }
        /* PackSize */
        pos1 = pos2 + 1;
        pos2 = strchr(pos1, ',');
        memset(tmp, 0x00, sizeof(tmp));
        memcpy(tmp, pos1, pos2 - pos1);
        sscanf(tmp, "%d", &intTmp);
        LOG("PackSize : %d\r\n", intTmp);
        /* PackContent */
        pos1 = pos2 + 1;
        UpdateInfoAppCodeSave((void *)pos1, AppStoreAddr, intTmp);
        PackId++;
        AppStoreAddr += intTmp;
        PackSize -= intTmp;
        /* finish */
        if (0 == PackSize) {
            /* notif to server : update finish */
            FrameSend(FRAME_CMD_FINISH_UPDATE, NULL, 0);
            info.NeedFlag = UPDATE_FLAG_YES;
            UpdateInfoSet(&info);
            LOG("Update Finish\r\n");
            delay_ms(100);
            NVIC_SystemReset();
            return ;
        }
        UpdateTimeout = 0;
    }
    
}

void FrameRecv(void) {

    unsigned char RecvData[FRAME_MAX_LEN] = {0x00};
    Frame_t frame;
    
    if (1 == RS485Recv(RecvData, NULL)) {
        return ;
    }
    
    if (1 == FrameDataParse(RecvData, &frame)) {
        return ;
    }
    LOG("cmd : %02x \r\n", frame.data.cmd);
    switch (frame.data.cmd) {
        case FRAME_CMD_LOCK_OPEN : {
            JwLockOpen();
        } break;
        case FRAME_CMD_LOCK_CLOSE : {
            JwLockClose();
        } break;
        case FRAME_CMD_GET_OPEN_RECORD : {
            unsigned char RecordNum = frame.data.data[0];
            LOG("RecordNum : %d\r\n", RecordNum);
            JwLockGetOpenRecord(0, 5);
        } break;
        case FRAME_CMD_RSP_SYSTEM_TIME : {
            JwLockTime_t time = *((JwLockTime_t *)(frame.data.data));
            LOG("%02x_%02x_%02x %02x:%02x:%02x ÐÇÆÚ%d\r\n",
                time.year, time.month, time.day,
                time.hour, time.min, time.sec, time.week);
            JwLockSetTime(time);
        } break;
        case FRAME_CMD_FIRMWARE_UPDATE : {
            FrameDealFirmwareUpdate(frame, RecvData);
        } break;
        default : {
        } break;
    }
    
}

