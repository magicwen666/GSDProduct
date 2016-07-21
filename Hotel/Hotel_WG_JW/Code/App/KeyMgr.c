#include "KeyMgr.h"
#include "key.h"
#include "SysInfo.h"

void KeyMgrInit(void) {
    
    KeyInit();
}

void KeyMgrListened(void) {

    key_sta_t sta = KeyStaSwitch(KeyScan());

    switch (sta) {
        case KEY_STA_LHOLD : {
            SysInfoReset();
        } break;
        default : {
        } break;
    }
    
}

