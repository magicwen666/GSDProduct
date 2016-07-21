#ifndef __WT588D_H__
#define __WT588D_H__

typedef enum {
    WT588D_VOLUME_1 = 0xE0,
    WT588D_VOLUME_2 = 0xE1,
    WT588D_VOLUME_3 = 0xE2,
    WT588D_VOLUME_4 = 0xE3,
    WT588D_VOLUME_5 = 0xE4,
    WT588D_VOLUME_6 = 0xE5,
    WT588D_VOLUME_7 = 0xE6,
    WT588D_VOLUME_8 = 0xE7,
} WT588DVolume_t;

#define WT588D_LOOP_PLAY    0xF2
#define WT588D_STOP_PLAY    0xFE

typedef enum {
    WT588D_CONTEX_NET_CFG,
    WT588D_CONTEX_LINK_WAIT,
    WT588D_CONTEX_LINK_SUCC,
    WT588D_CONTEX_LINK_FAIL,
    WT588D_CONTEX_LOGIN_SUCC,
    WT588D_CONTEX_RESET_SUCC,
    WT588D_CONTEX_LOCK_ASSOC_SUCC,
    WT588D_CONTEX_LOCK_ASSOC_NONE,
    WT588D_CONTEX_FLASH_INIT_ERR,
} WT588DContex_t;

void WT588DInit(void);
void WT588DSendByte(unsigned char byte);
void WT588DStartPlay(unsigned char addr);
void WT588DStopPlay(void);
void WT588DLoopPlay(void);
void WT588DAdjustVolume(WT588DVolume_t volume);

#endif



