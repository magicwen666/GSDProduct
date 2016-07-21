#ifndef __FRAME_H__
#define __FRAME_H__

#define FRAME_HEAD  0xFD
#define FRAME_TAIL  0xFE
#define FRAME_JWLOCK_WG_TYPE    0x05

#define FRAME_ADDR_LEN  4
#define FRAME_TOTAL_LEN(frame)      (frame.len + 4)
#define FRAME_DATA_DATA_LEN(frame)  (frame->len - 10)
#define FRAME_MAX_LEN   256

#pragma pack(1)

typedef enum {
    FRAME_CMD_NULL,
    FRAME_CMD_LOCK_OPEN,
    FRAME_CMD_LOCK_CLOSE,
    FRAME_CMD_GET_OPEN_RECORD,
    FRAME_CMD_RSP_OPEN_RECORD,
    FRAME_CMD_GET_SYSTEM_TIME,
    FRAME_CMD_RSP_SYSTEM_TIME,
    FRAME_CMD_LOW_VOLTAGE,
    FRAME_CMD_LOCK_OPEN_EVENT,
    FRAME_CMD_FIRMWARE_UPDATE,
    FRAME_CMD_REQUEST_UPDATE,
    FRAME_CMD_RESPON_UPDATE,
    FRAME_CMD_FINISH_UPDATE,
} FrameCmd_t;

typedef struct {
    unsigned char dst[4];
    unsigned char src[4];
    FrameCmd_t cmd;
    unsigned char *data;
    unsigned char chk;
} FrameData_t;

typedef struct {
    unsigned char head;
    unsigned char type;
    unsigned int len;
    FrameData_t data;
    unsigned char tail;
} Frame_t, *pFrame_t;
#pragma pack()

void FrameDataCreate(void *data, pFrame_t frame);
unsigned char FrameDataParse(void *data, pFrame_t frame);
void FrameSend(FrameCmd_t cmd, void *arg, unsigned char len);
void FrameRecv(void);

#endif



