#ifndef __DOOR_H__
#define __DOOR_H__

typedef enum {
    DOOR_STA_NONE,
    DOOR_STA_CLOSE,
    DOOR_STA_OPEN,
} door_sta_t;

typedef struct {
    door_sta_t sta;
    unsigned int tick;  /* ¶à¾Ã¼ì²âÃÅµÄ×´Ì¬ */
} door_t;

void DoorHander(void);
void DoorInit(void);
door_sta_t DoorGetSta(void);
void DoorOpenFeedback(void);
void DoorCloseFeedback(void);
unsigned char DoorIsCanCheck(void);

#endif


