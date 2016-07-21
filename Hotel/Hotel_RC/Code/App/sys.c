#include "sys.h"
#include "keycolor.h"
#include "scr_key.h"
#include "ps.h"
#include "conf_key.h"
#include "board_ir.h"
#include "board_wdt.h"
#include "network.h"
#include "door.h"
#include "board_flash.h"
#include "delay.h"
#include <string.h>

sys_info_t g_sys_info;
static volatile unsigned char master = 0;

static void SysInfoDefault(void) {

    g_sys_info.used = 0x01;
    g_sys_info.id.type = NET_DEV_TYPE_RC;
    g_sys_info.id.id = 0x01;
    g_sys_info.scr_num.wait = 1;
    g_sys_info.scr_num.other = 2;
    g_sys_info.master = 0; 
    g_sys_info.color = ORANGE;
    
}

void SysInfoInit(void) {

    unsigned char ScrBit = 0;
    
    //FlashErase(SYS_INFO_ADDR);
    FlashReadData((unsigned char *)&g_sys_info, SYS_INFO_ADDR, SYS_INFO_SIZE);
    if ((0x01 != g_sys_info.used) && 
        (NET_DEV_TYPE_RC != g_sys_info.id.type)) {
        SysInfoDefault();
        ScrKeyAssocClear();
        FlashWriteData(&g_sys_info, SYS_INFO_ADDR, SYS_INFO_SIZE); 
    }
    ScrBit = g_sys_info.scr_num.wait + g_sys_info.scr_num.other;
    if (ScrBit > 4) {
        SysInfoDefault();
        ScrKeyAssocClear();
        FlashWriteData(&g_sys_info, SYS_INFO_ADDR, SYS_INFO_SIZE); 
    }

    master = g_sys_info.master;
    ScrKeyInit(g_sys_info);
    KeyColorInit(g_sys_info.color);
    board_led_close(LEDALL);
    PsInit();
    ScrKeyResum();
    
}

void SysInfoSetMaster(unsigned char master) {

    g_sys_info.master = master;
    
    FlashWriteData(&g_sys_info, SYS_INFO_ADDR, SYS_INFO_SIZE); 
    
}

unsigned char SysInfoIsMaster(void) {

    if (g_sys_info.master != master) {
        g_sys_info.master = master;
    }

    return g_sys_info.master;
}

void SysInfoScrBitSave(sys_info_t info) {


    g_sys_info.scr_num.wait = info.scr_num.wait;
    g_sys_info.scr_num.other = info.scr_num.other;
    FlashWriteData(&g_sys_info, SYS_INFO_ADDR, SYS_INFO_SIZE); 
    /* clear assoc key */
    ScrKeyAssocClear();
    
}

void SysInfoColorSave(color_t color) {

    g_sys_info.color = color;
    FlashWriteData(&g_sys_info, SYS_INFO_ADDR, SYS_INFO_SIZE); 
}

color_t SysInfoColorGet(void) {

    return g_sys_info.color;
}


void KeyTask(void) {

    unsigned char keys = 0;
    unsigned char i = 0;
    scr_key_t *p = NULL;

    keys = board_key_scan();
    if (0x00 != keys) {
        PsWake();
    }
    for (i = 0; i < MAX_SCR_KEY_SIZE; i++) {
        p = &g_scr_key[i];
        if (KEY_TYPE_NONE == p->type){
            break;
        }
        ScrKeyStateSwitch(p, keys);
        ScrKeyProcess(p);
    }

}

static void set_entry_clear(void) {

    unsigned char i = 0;
    scr_key_t *scr_key_p = NULL;
    conf_key_t *conf_key_p = NULL;

    for (i = 0; i < MAX_SCR_KEY_SIZE; i++) {
        scr_key_p = &g_scr_key[i];
        if (KEY_TYPE_NONE != scr_key_p->type) {
            board_light_close(scr_key_p->light_p);
        }
        scr_key_p->state = KEY_STATE_NONE;
        scr_key_p->okey.okey_state = OKEY_STATE_UP;
    }

    for (i = 0; i < 6; i++) {
        conf_key_p = &conf_key[i];
        conf_key_p->state = KEY_STATE_CLOSE;
        conf_key_p->okey.okey_state = OKEY_STATE_UP;
    }

    board_ir_clean();
    
}

static void set_quit_clear(void) {

    unsigned char i = 0;
    scr_key_t *scr_key_p = NULL;
    conf_key_t *conf_key_p = NULL;

    for (i = 0; i < MAX_SCR_KEY_SIZE; i++) {
        scr_key_p = &g_scr_key[i];
        if (KEY_TYPE_NONE != scr_key_p->type) {
            board_light_close(scr_key_p->light_p);
        }
        scr_key_p->state = KEY_STATE_CLOSE;
        scr_key_p->okey.okey_state = OKEY_STATE_UP;
    }

    for (i = 0; i < 6; i++) {
        conf_key_p = &conf_key[i];
        conf_key_p->state = KEY_STATE_CLOSE;
        conf_key_p->okey.okey_state = OKEY_STATE_UP;
    }

    board_ir_clean();
    board_key_free_wait(KEYALL);

}

static void AssocMainkeyCreate(unsigned char *mainkey) {

    unsigned char i = 0;
    scr_key_t *p = NULL;

    *mainkey = 0;

    for (i = 0; i < MAX_SCR_KEY_SIZE; i++) {
        p = &g_scr_key[i];
        if (KEY_TYPE_NONE == p->type) {
            continue;
        }
        if (KEY_STATE_OPEN == p->state) {
            *mainkey = p->keyval;
            return ;
        }
    }
    
}

static void WaitNetQuit(quit_state_t *sta, net_cmd_t cmd) {

    switch (cmd) {
        case NET_CMD_SAVE_SET : {
            *sta = QUIT_STATE_NET_SAVE;
        } break;
        case NET_CMD_CANCEL_SET : {
            *sta = QUIT_STATE_NET_CANCEL;
        } break;
        case NET_CMD_DEL_SET : {
            *sta = QUIT_STATE_NET_DELETE;
        } break;
    }

}

static void WaitIrQuit(quit_state_t *sta, unsigned char *mainkey) {

    key_t keys = 0x00;
    ir_t ir = 0x00;
    net_frame_rx_t rx;

    ir = board_ir_get();
    
    if ((IR_SET != ir) && (IR_OFF != ir) && (IR_DEL != ir)) {
        return ;
    }
    
    board_led_flash_del(LED1|LED2|LED3|LED4|LED5);
    board_led_close(LED1|LED2|LED3|LED4|LED5);
    board_led_flash_add(LED6);
    
    while (1) {
        board_wdt_feed(); 
        if (0 == board_time_out_wait()) {
            *sta = QUIT_STATE_LOCAL_CANCEL;
            NetworkSend(NET_CMD_CANCEL_SET, 0);
            return ;
        }
        keys = board_key_scan();
        ConfKeyStateSwitch(&conf_key[5], keys);
        if (OKEY_STATE_DOWN == conf_key[5].okey.okey_state) {
            switch (ir) {
                case IR_SET : {
                    AssocMainkeyCreate(mainkey);
                    if (0 != mainkey) {
                        *sta = QUIT_STATE_LOCAL_SAVE;
                        NetworkSend(NET_CMD_MAINKEY, *mainkey);
                        NetworkSend(NET_CMD_SAVE_SET, 0);
                    } else {
                        *sta = QUIT_STATE_LOCAL_CANCEL;
                        NetworkSend(NET_CMD_CANCEL_SET, 0);
                    }
                } break;
                case IR_OFF : {
                    *sta = QUIT_STATE_LOCAL_CANCEL;
                    NetworkSend(NET_CMD_CANCEL_SET, 0);
                } break;
                case IR_DEL : {
                    *sta = QUIT_STATE_LOCAL_DELETE;
                    NetworkSend(NET_CMD_DEL_SET, 0);
                } break;
            }
        }

        if (0 == NetworkRecv(&rx)) {
            switch (rx.cmd) {
                case NET_CMD_MAINKEY : {
                    *mainkey = rx.keyval;
                } break;
                default : {
                    WaitNetQuit(sta, rx.cmd);
                } break;
            }
        }

        if (QUIT_STATE_NONE != *sta) {
            return;
        }
        
    }
    
}

static void AssocStoreSave(main_key_t main_key) {

    unsigned char i = 0;
    scr_key_t *p = NULL;

    
    for (i = 0; i < MAX_SCR_KEY_SIZE; i++) {
        p = &g_scr_key[i];
        if (KEY_TYPE_NONE == p->type) {
            break;
        }
        if (KEY_STATE_OPEN == p->state) {
            p->assoc_key = main_key;
            ScrKeyAssocBackup(i);
        }
    }
}

static void AssocStoreDel(void) {

    unsigned char i = 0;
    scr_key_t *p = NULL;

    for (i = 0; i < MAX_SCR_KEY_SIZE; i++) {
        p = &g_scr_key[i];
        if (KEY_TYPE_NONE == p->type) {
            break;
        }
        if (KEY_STATE_OPEN == p->state) {
            p->assoc_key = 0x00;
            ScrKeyAssocBackup(i);
        }
    }
    
}

static void AssocSet(void) {

    quit_state_t quit_sta = QUIT_STATE_NONE;
    unsigned char mainkey = 0;
    net_frame_rx_t rx;
    key_t keys = 0x00;
    unsigned char i = 0;
    scr_key_t *p = NULL;

    board_led_flash_start();
    board_key_free_wait(KEYALL);
    board_time_out_set(MAX_IR_TIMEOUT);

    while (1) {
        board_wdt_feed();
        if (0 == board_time_out_wait()) {
            board_led_flash_stop();
            return ;
        }

        keys = board_key_scan();
        for (i = 0; i < MAX_SCR_KEY_SIZE; i++) {
            board_wdt_feed();
            p = &g_scr_key[i];
            if (KEY_TYPE_NONE == p->type) {
                break;
            }
            ScrKeyStateSwitch(p, keys);
            if (OKEY_STATE_DOWN == p->okey.okey_state) {
                if (KEY_STATE_NONE == p->state) {
                    if (ScrKeyHaveOpen()) { /* mutex */
                        break;
                    }
                    ScrKeyStateOpen(p);
                    NetworkSend(NET_CMD_ASSOC_OPEN, p->assoc_key);
                } else if (KEY_STATE_OPEN == p->state) {
                    ScrKeyStateNone(p);
                    NetworkSend(NET_CMD_ASSOC_CLOSE, p->assoc_key);
                }
            }
        }

        if (0 == NetworkRecv(&rx)) {
            switch (rx.cmd) {
                case NET_CMD_ASSOC_OPEN :
                case NET_CMD_ASSOC_CLOSE : {
                    NetWorkAssocRun(rx.cmd, rx.keyval);
                    if (NET_CMD_ASSOC_CLOSE == rx.cmd) {
                        for (i = 0; i < MAX_SCR_KEY_SIZE; i++) {
                            if (KEY_TYPE_NONE == g_scr_key[i].type) {
                                break;
                            }
                            if (rx.keyval == g_scr_key[i].keyval) {
                                board_led_flash_add(g_scr_key[i].leds);
                                break;
                            }
                        }
                    }
                } break;
                case NET_CMD_MAINKEY : {
                    mainkey = rx.keyval;
                } break;
                default : {
                    WaitNetQuit(&quit_sta, rx.cmd);
                } break;
            }
        }

        WaitIrQuit(&quit_sta, &mainkey);
        switch (quit_sta) {
            case QUIT_STATE_LOCAL_SAVE :
            case QUIT_STATE_NET_SAVE : {
                AssocStoreSave(mainkey);
                board_led_flash_stop();
            } return;
            case QUIT_STATE_LOCAL_DELETE :
            case QUIT_STATE_NET_DELETE : {
                AssocStoreDel();
                board_led_flash_stop();
            } return;
            case QUIT_STATE_LOCAL_CANCEL :
            case QUIT_STATE_NET_CANCEL : {
                board_led_flash_stop();
            } return;
            default : {
            } break;
        }
        
    }

}

static void MasterSet(void) {

    key_t keys = 0;
    ir_t ir = 0x00;

    board_led_flash_start();
    board_key_free_wait(KEYALL);
    board_time_out_set(MAX_IR_TIMEOUT);

    board_led_flash_del(LED2 | LED3 | LED4 | LED5 | LED6);
    board_led_close(LED2 | LED3 | LED4 | LED5 | LED6);

    if (1 == SysInfoIsMaster()) {
        board_led_flash_del(LED1);
        board_led_open(LED1);
        conf_key[0].state = KEY_STATE_OPEN;
    }

    while (1) {
        board_wdt_feed();
        if (0 == board_time_out_wait()) {
            board_led_flash_stop();
            return ;
        }
        keys = board_key_scan();
        ConfKeyStateSwitch(&conf_key[0], keys);
        if (OKEY_STATE_DOWN == conf_key[0].okey.okey_state) {
            if (KEY_STATE_CLOSE == conf_key[0].state) {
                conf_key[0].state = KEY_STATE_OPEN;
                board_led_flash_del(LED1);
                board_led_open(LED1);
            } else if (KEY_STATE_OPEN == conf_key[0].state) {
                conf_key[0].state = KEY_STATE_CLOSE;
                board_led_flash_add(LED1);
                board_led_close(LED1);
            }
        }

        ir = board_ir_get();
        switch (ir) {
            case IR_SET : {
                if (KEY_STATE_OPEN == conf_key[0].state) {
                    SysInfoSetMaster(1);
                }
                board_led_flash_stop();
            } return;
            case IR_OFF : {
                board_led_flash_stop();
            } return;
            case IR_DEL : {
                SysInfoSetMaster(0);
                board_led_flash_stop();
            } return;
            default : {
            } break;
        }
        
    }
    
}

static void ScrBitSet(void) {

    ir_t ir = 0x00;
    sys_info_t info;

    board_led_flash_start();
    board_key_free_wait(KEYALL);
    board_time_out_set(MAX_IR_TIMEOUT);

    while (1) {
        board_wdt_feed();
        if (0 == board_time_out_wait()) {
            board_led_flash_stop();
            return ;
        }
        
        ir = board_ir_get();
        switch (ir) {
            case IR_OFF : {
                board_led_flash_stop();
            } return;
            case IR_ON : {
                board_led_flash_del(LED1);
                board_led_open(LED1);
                info.scr_num.wait = 1;
            } break;
            case IR_L1 : {
                board_led_flash_del(LED2);
                board_led_open(LED2);
                info.scr_num.other = 1;
            } break;
            case IR_L2 : {
                board_led_flash_del(LED2);
                board_led_open(LED2);
                info.scr_num.other = 2;
            } break;
            case IR_L3 : {
                board_led_flash_del(LED2);
                board_led_open(LED2);
                info.scr_num.other = 3;
            } break;
            case IR_L4 : {
                if (1 != info.scr_num.wait) {
                    board_led_flash_del(LED2);
                    board_led_open(LED2);
                    info.scr_num.other = 4;
                } 
            } break;
            case IR_SET : {
                board_led_flash_stop();
                SysInfoScrBitSave(info);
                ScrKeyInit(g_sys_info);
                KeyColorInit(g_sys_info.color);
                board_led_close(LEDALL);
            } return;
            default : {
            } break;
        }
    }
}


static void KeyColorSet(void) {

    key_t keys = 0x00;
    conf_key_t *p = NULL;
    unsigned char i = 0;
    color_t colorindex = BLUE;
    rgb_t rgbtmp;
    ir_t ir = 0x00;

    board_key_free_wait(KEYALL);
    board_time_out_set(MAX_IR_TIMEOUT);

    while (1) {
        board_wdt_feed();
        if (0 == board_time_out_wait()) {
            KeyColorInit(g_sys_info.color);
            board_led_close(LEDALL);
            return ;
        }
        keys = board_key_scan();
        for (i = 0; i < 6; i++) {
            p = &conf_key[i];
            ConfKeyStateSwitch(p, keys);
            if (OKEY_STATE_DOWN != p->okey.okey_state) {
                continue;
            }
            colorindex = (color_t)((++colorindex) % MAX_COLOR_SIZE);
            rgbtmp = fore_color[colorindex];
            board_led_backlight_set(LEDALL, rgbtmp);
            board_led_forelight_set(LEDALL, rgbtmp);
            board_led_open(LEDALL);
        }

        ir = board_ir_get();
        switch (ir) {
            case IR_OFF : {
                KeyColorInit(g_sys_info.color);
                board_led_close(LEDALL);
            } return;
            case IR_SET : {
                SysInfoColorSave(colorindex);
                KeyColorInit(g_sys_info.color);
                board_led_close(LEDALL);
            } return;
            default : {
            } break;
        }
        
    }
    
}

static volatile rgb_t RedColor = {60, 0, 0};
static volatile rgb_t GreenColor = {0, 60, 0};
static volatile rgb_t BlueColor = {0, 0, 60};

static void LEDCheck(void) {

    ir_t ir = 0x00;

    while (1) {
        board_wdt_feed();
        board_led_display(LEDALL, RedColor);
        delay_ms(400);
        board_led_display(LEDALL, GreenColor);
        delay_ms(400);
        board_led_display(LEDALL, BlueColor);
        delay_ms(400);
        ir = board_ir_get();
        switch (ir) {
            case IR_OFF : {
                board_led_close(LEDALL);
            } return;
            default : {
            } break;
        }
    }
    
}

void IrTask(void) {

    ir_t ir = 0x00;
    key_t keys = 0x00;
    unsigned char i = 0;
    net_frame_rx_t rx;

    ir = board_ir_get();
    
    if (IR_SET != ir) {
        return ;
    }
    PsWake();
    
    ConfKeyInit();
    board_led_rotate_start();
    set_entry_clear();
    board_time_out_set(MAX_IR_TIMEOUT);
    
    while (1) {
        board_wdt_feed();
        if (0 == board_time_out_wait()) {
            board_led_rotate_stop();
            set_quit_clear();
            return ;
        }
        
        ir = board_ir_get();
        if (IR_OFF == ir) {
            board_led_rotate_stop();
            set_quit_clear();
            return ;
        }

        keys = board_key_scan();
        for (i = 0; i < 6; i++) {
            ConfKeyStateSwitch(&conf_key[i], keys);
        }
        /* 屏位设置 */
        if (OKEY_STATE_HOLD == conf_key[0].okey.okey_state) {
            board_led_rotate_stop();
            set_entry_clear();
            ScrBitSet();
            set_quit_clear();
            return ;
        }
        /* 关联设置 */
        if (OKEY_STATE_HOLD == conf_key[1].okey.okey_state) {
            board_led_rotate_stop();
            NetworkSend(NET_CMD_ASSOC_SET, 0x00);
            set_entry_clear();
            AssocSet();
            set_quit_clear();
            return ;
        }
        /* 主机设置 */
        if (OKEY_STATE_HOLD == conf_key[2].okey.okey_state) {
            board_led_rotate_stop();
            set_entry_clear();
            MasterSet();
            set_quit_clear();
            return ;
        }
        /* 背光颜色设置 */
        if (OKEY_STATE_HOLD == conf_key[3].okey.okey_state) {
            board_led_rotate_stop();
            set_entry_clear();
            KeyColorSet();
            set_quit_clear();
            return ;
        }
        /* LED 检测 */
        if (OKEY_STATE_HOLD == conf_key[4].okey.okey_state) {
            board_led_rotate_stop();
            set_entry_clear();
            LEDCheck();
            set_quit_clear();
            return ;
        }
        if (0 == NetworkRecv(&rx)) {
            switch (rx.cmd) {
                case NET_CMD_ASSOC_SET : {
                    board_led_rotate_stop();
                    set_entry_clear();
                    AssocSet();
                    set_quit_clear();
                } return;
                default : {
                } break;
            }
        }
        
    }
     
    
}

void PsTask(void) {

    if (IRPS_STATE_ON == board_irps_get()) {
        PsWake();
    }

    if (0 == g_ps.delay_tick) {
        PsSleep();
    }

}

void NetWorkAssocRun(net_cmd_t cmd, main_key_t assockey) {

    unsigned char i = 0;
    scr_key_t *p = NULL;

    if (0x00 == assockey) {
        return ;
    }
    
    for (i = 0; i < MAX_SCR_KEY_SIZE; i++) {
        p = &g_scr_key[i];
        if (KEY_TYPE_NONE == p->type) {
            break;
        }
        if (assockey != p->assoc_key) {
            continue;
        }
        
        switch (cmd) {
            case NET_CMD_ASSOC_OPEN : {
                ScrKeyStateOpen(p);
                if (1 == SysInfoIsMaster()) {
                    NetworkSend(NET_CMD_LOAD_OPEN, p->keyval);
                }
            } return;
            case NET_CMD_ASSOC_CLOSE : {
                ScrKeyStateClose(p);
                if (1 == SysInfoIsMaster()) {
                    NetworkSend(NET_CMD_LOAD_CLOSE, p->keyval);
                }
            } return;
        }
    }
    
}

static void NetWorkLoadRun(net_cmd_t cmd, main_key_t mainkey) {

    unsigned char i = 0;
    scr_key_t *p = NULL;

    for (i = 0; i < MAX_SCR_KEY_SIZE; i++) {
        p = &g_scr_key[i];
        if (KEY_TYPE_NONE == p->type) {
            break;
        }
        if (mainkey != p->keyval) {
            continue;
        }
        switch (cmd) {
            case NET_CMD_LOAD_OPEN : {
                ScrKeyStateOpen(p);
                if (0 != p->assoc_key) {
                    NetworkSend(NET_CMD_ASSOC_OPEN, p->assoc_key);
                }
            } break;
            case NET_CMD_LOAD_CLOSE : {
                ScrKeyStateClose(p);
                if (0 != p->assoc_key) {
                    NetworkSend(NET_CMD_ASSOC_CLOSE, p->assoc_key);
                }
            } break;
        }
        return ;
    }
    
}

void NetWorkTask(void) {

    net_frame_rx_t rx;

    if (NetworkRecv(&rx)) {
        return ;
    }
    LOG("cmd : %02x\r\n", rx.cmd);
    switch (rx.cmd) {
        case NET_CMD_LOAD_OPEN : 
        case NET_CMD_LOAD_CLOSE : {
            if (1 == SysInfoIsMaster()) {
                //PsWake();
                NetWorkLoadRun(rx.cmd, rx.keyval);
            }
        } break;
        case NET_CMD_ASSOC_CLOSE : 
        case NET_CMD_ASSOC_OPEN : {
            PsWake();
            NetWorkAssocRun(rx.cmd, rx.keyval);
        } break;
        case NET_CMD_ASSOC_SET : {
            set_entry_clear();
            AssocSet();
            set_quit_clear();
        } break;
        default : {
        } break;
    }
    
}

void WaitTask(void) {

    if (KEY_TYPE_WAIT != g_scr_key[0].type) {
        return ;
    }

    if (KEY_STATE_OPEN != g_scr_key[0].state) {
        return ;
    }

    if (0 == WaitDelayTick) {
        ScrKeyStateClose(&g_scr_key[0]);
    }
    
}

void DoorTask(void) {

    door_sta_t sta = DOOR_STA_NONE;
    
    if (DoorIsCanCheck()) {
        return ;
    }

    sta = DoorGetSta();
    switch (sta) {
        case DOOR_STA_OPEN : {
            if (0 == SysInfoIsMaster()) {
                break;
            }
            DoorOpenFeedback();
        } break;
        case DOOR_STA_CLOSE : {
            if (0 == SysInfoIsMaster()) {
                break;
            }
            DoorCloseFeedback();
        } break;
        default : {
        } break;
    }

}



