#include "shell.h"
#include "chip.h"
#include "JwLock.h"
#include "BleS02.h"
#include "SysInfo.h"
#include "rs485.h"
#include <string.h>
#include <stdio.h>

static void ShellShowHelp(void) {

    LOG("\r\n------------------------Shell CMD V1.0------------------------ \r\n");
    LOG("-------系统命令-------\r\n"
        "001 : 系统重启\r\n"
        "002 : 配置序列号\r\n"
        "003 : 配置房间号\r\n"
        "-------S02指令-------\r\n"
        "200 : 扫描\r\n"
        "201 : 连接\r\n"
        "202 : 待机\r\n"
        "203 : 开锁\r\n"
        "204 : 关锁\r\n"
        "205 : 锁信息\r\n"
        "206 : 设置时间\r\n"
        "207 : 获取开门记录\r\n"
        );

}

static void SheelDealID(unsigned char *data) {

    unsigned int tmp = 0;
    sys_id_t id;
    unsigned char i = 0;
    
    sscanf((char *)data, "%08x", &tmp);
    for (i = 0; i < 4; i++) {
        id.id[i] = (tmp & 0xFF000000) >> 24;
        tmp <<= 8;
    }
    SysInfoIDSet(id);

}

static void SheelDealRoomNum(unsigned char *data) {

    //unsigned char len = strlen((char *)data);
    unsigned int tmp = 0;
    sys_room_t room;
    
    //if (4 != (len) {
        //LOG("Room Number Length Err, Must be 4bit : %d\r\n", len);
        //return ;
    //}
    
    sscanf((char *)data, "%04x", &tmp);
    room.floor = (tmp & 0xFF00) >> 8;
    room.num = tmp & 0x00FF;
    SysInfoRoomNumSet(room);
    
}

void ShellCmdParse(unsigned char *buf, unsigned char len) {
    
    unsigned int cmd = 0;
    unsigned char i = 0;
    
    if ((NULL == buf) || (len < 3)) {
        ShellShowHelp();
        LOG("\r\nhzw@GAKATO ~# ");
        return ;
    }
    for (i = 0; i < 3; i++) {
        if ((buf[i] < '0') || (buf[i] > '9')) {
            ShellShowHelp();
            LOG("\r\nhzw@GAKATO ~# ");
            return ;
        }
    }

    cmd  = (buf[0] - 0x30) * 100;
    cmd += (buf[1] - 0x30) * 10;
    cmd += (buf[2] - 0x30);

    switch (cmd) {
        case 1 : {
            NVIC_SystemReset();
        } break;
        case 2 : {
            if (' ' != buf[3]) {
                ShellShowHelp();
                break;
            }
            SheelDealID(&buf[4]);
        } break;
        case 3 : {
            if (' ' != buf[3]) {
                ShellShowHelp();
                break;
            }
            SheelDealRoomNum(&buf[4]);
        } break;
        case 200 : {
            BleS02GlobalScan(NULL, NULL);
        } break;
        case 201 : {
            BleS02LinkWithIndex(buf[4]);
        } break;
        case 202 : {
            BleS02Standby();
        } break;
        case 203 : {
            JwLockOpen();
        } break;
        case 204 : {
            JwLockClose();
        } break;
        case 205 : {
            JwLockGetSta();
        } break;
        case 206 : {
            JwLockTime_t time = {0x20, 0x05, 0x10, 0x13, 0x07, 0x03, 0x16};
            JwLockSetTime(time);
        } break;
        case 207 : {
            JwLockGetOpenRecord(0, 5);
        } break;
        default : {
            LOG("\r\nUnkonw cmd : %d\r\n", cmd);
        } break;
    }
    
    
    LOG("\r\nhzw@GAKATO ~# ");
    
}


void ShellCmdRecv(void) {

    if ((0x8000 != (UsartLen & 0x8000)) ||
        (USART_RECV_MODE_SHELL != UsartRecvMode)) {
        return ;    
    }
    ShellCmdParse(UsartBuf, UsartLen & 0x7FFF);
    UsartLen = 0;
    memset(UsartBuf, 0x00, sizeof(UsartBuf));
    UsartRecvMode = 0;

}



