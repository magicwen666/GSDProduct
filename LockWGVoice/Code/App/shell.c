#include "shell.h"
#include "chip.h"
#include "lock315.h"
#include "wifi.h"
#include "network.h"
#include "SysInfo.h"
#include "W25qFlash.h"
#include "DoorMagnet.h"
#include "wt588d.h"
#include <string.h>
#include <stdio.h>

static void ShellShowHelp(void) {

    char LogBuf[1024] = {0x00};
    
    strcpy(LogBuf, "\r\n------------------------Shell CMD V1.0------------------------ \r\n");
    strcat(LogBuf, 
        "-------系统命令-------\r\n"
        "001 : 系统重启\r\n"
        "002 : 系列号配置6位<etc.c1000001>,要保证唯一性\r\n"
        "003 : 恢复出厂设置\r\n"
        "-------锁指令-------\r\n"
        "100 : 开锁\r\n"
        "101 : 关锁\r\n"
        "------语音指令-------\r\n"
        "200 : 播放\r\n"
        "201 : 停止\r\n");

    strcat(LogBuf, "\r\nhzw@GAKATO ~# ");
    SYSLOG(LogBuf);

}

static void SheelDealID(unsigned char *id) {

    unsigned char len = strlen((const char *)id);
    unsigned char i = 0;

    if (8 != len) {
        SYSLOG("\r\nID Length Error, Must be 8 bit\r\n");
        return ;
    }

    if (0 != memcmp(id, "c1", 2)) {
        SYSLOG("\r\nID Error, Must begin whth \"c1\"\r\n");
        return ;
    }
    
    for (i = 2; i < len; i++) {
        if ((id[i] < '0') || (id[i] > '9')) {
            SYSLOG("\r\nID Error, Must be 0-9\r\n");
            return ;
        }
    }
    
    SysInfoIDSave(id);
    SYSLOG("序列号配置成功\r\n");
    NVIC_SystemReset();
}

/*
static void SheelDealPwd(unsigned char *pwd) {

    unsigned char len = strlen((const char *)pwd);
    unsigned char i = 0;

    if (8 != len) {
        SYSLOG("\r\nPassword Length Error, Must be 8 bit\r\n");
        return ;
    }

    for (i = 0; i < len; i++) {
        if ((pwd[i] < '0') || (pwd[i] > '9')) {
            SYSLOG("\r\nPassword Error, Must be 0-9\r\n");
            return ;
        }
    }
    SysInfoPwdSave(pwd);

}*/

void ShellCmdParse(unsigned char *buf, unsigned char len) {
    
    unsigned int cmd = 0;
    unsigned char i = 0;
    
    if ((NULL == buf) || (len < 3)) {
        ShellShowHelp();
        return ;
    }
    for (i = 0; i < 3; i++) {
        if ((buf[i] < '0') || (buf[i] > '9')) {
            ShellShowHelp();
            return ;
        }
    }

    cmd  = (buf[0] - 0x30) * 100;
    cmd += (buf[1] - 0x30) * 10;
    cmd += (buf[2] - 0x30);
    SYSLOG("\r\n");
    switch (cmd) {
        case 1 : {
            NVIC_SystemReset();
        } break;
        case 2 : {
            if (' ' != buf[3]) {
                SYSLOG("\r\nUnkonw cmd : %d\r\n", cmd);
                return ;
            }
            SheelDealID(&buf[4]);
        } break;
        case 3 : {
            SysInfoReset(0);
            SYSLOG("恢复出厂设置完成\r\n");
            NVIC_SystemReset();
        } break;
        /* LOCK cmd */
        case 100 : {
            Lock315Open(); 
        } break;
        case 101 : {
            Lock315Close(); 
        } break;
        case 200 : {
            if (' ' != buf[3]) {
                SYSLOG("\r\nUnkonw cmd : %d\r\n", cmd);
                return ;
            }
            WT588DStartPlay(buf[4]-0x30);
        } break;
        case 201 : {
            WT588DStopPlay();
        } break;
        default : {
            SYSLOG("\r\nUnkonw cmd : %d\r\n", cmd);
        } break;
    } 
    
    SYSLOG("\r\nhzw@GAKATO ~# ");
    
}

static unsigned char ShellBuf[64] = {0x00};
static volatile unsigned short ShellLen = 0;

void ShellCmdRecv(void) {

    if (0x8000 != (ShellLen & 0x8000)) {
        return ;    
    }
    ShellCmdParse(ShellBuf, strlen((const char *)ShellBuf));
    ShellLen = 0;
    memset(ShellBuf, 0x00, sizeof(ShellBuf));
    
}

static unsigned char u0rbuf[64];
static volatile unsigned char u0len = 0;

void UART0_IRQHandler(void) {

    unsigned char r = 0;

    if (0x01 == (LPC_UART0->IIR & 0x01)) {
        return ;
    }

    r = LPC_UART0->RBR;
    if (('\r' == r) || ('\n' == r)) {
        ShellLen = u0len | 0x8000;
        memcpy(ShellBuf, u0rbuf, u0len);
        memset(u0rbuf, 0x00, sizeof(u0rbuf));
        u0len = 0;
        return ;
    }
    u0rbuf[u0len++] = r;
    
}


