#ifndef __SYS_H__
#define __SYS_H__

#include "chip.h"
#include "network.h"

//#define DEBUG
#ifdef  DEBUG
#define LOGInit()    uart0_init(115200)
#define LOG(...)     printf(__VA_ARGS__)
#else
#define LOGInit()
#define LOG(...)
#endif

#define FEED_DOG    (board_wdt_feed())

#define MAX_SCR_KEY_SIZE    6       // 最大屏幕按键数
#define MAX_COLOR_SIZE      3       // 最大颜色数

#pragma pack(1)

typedef struct {
    unsigned char r_val;
    unsigned char g_val;
    unsigned char b_val;
} rgb_t;

typedef struct {
    net_dev_type_t type;
    unsigned char id;
} id_t;

typedef unsigned char main_key_t;

typedef enum {
    BLUE,
    WHITE,
    ORANGE,
} color_t;

typedef struct {
    unsigned char used; /* 0x01 : used */
    id_t id;
    struct {
        unsigned char wait;
        unsigned char other;
    } scr_num;
    unsigned char master;   /* 主机 */
    color_t color;
} sys_info_t;
extern sys_info_t g_sys_info;

typedef enum {
    QUIT_STATE_NONE,
    QUIT_STATE_NET_SAVE,
    QUIT_STATE_NET_DELETE,
    QUIT_STATE_NET_CANCEL,
    QUIT_STATE_LOCAL_SAVE,
    QUIT_STATE_LOCAL_DELETE,
    QUIT_STATE_LOCAL_CANCEL
} quit_state_t;

#pragma pack()


#define SYS_BACKLIGHT_LEVEL 0       /* 0-3 */
#define SYS_KEY_COLOR       BLUE

#define SYS_INFO_ADDR   0xF000
#define SYS_INFO_SIZE   sizeof(sys_info_t)
//#define ASSOC_KEY_ADDR  (SYS_INFO_ADDR + SYS_INFO_SIZE)
#define ASSOC_KEY_ADDR  0xE000
#define ASSOC_KEY_SZIE  (sizeof(main_key_t) * MAX_SCR_KEY_SIZE)

#define MAX_IR_TIMEOUT  60000
#define WAIT_DELAY_TICK 120000  /* 120s */
//#define WAIT_DELAY_TICK 5000

void SysInfoInit(void);
void SysInfoSetMaster(unsigned char master);
unsigned char SysInfoIsMaster(void);
void SysInfoScrBitSave(sys_info_t info);
void SysInfoColorSave(color_t color);
color_t SysInfoColorGet(void);
void KeyTask(void);
void IrTask(void);
void PsTask(void);
void NetWorkAssocRun(net_cmd_t cmd, main_key_t mainkey);
void NetWorkTask(void);
void WaitTask(void);
void DoorTask(void);

#endif


