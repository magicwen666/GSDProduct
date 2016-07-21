#include "shell.h"
#include "chip.h"
#include "wifi.h"
#include "network.h"
#include "W25qFlash.h"
#include "wt588d.h"
#include "rf433.h"
#include "Rf433Mgr.h"
#include "SysInfo.h"
#include "delay.h"
#include <string.h>
#include <stdio.h>

static void ShellShowHelp(void) {

    SYSLOG("\r\n------------------------Shell CMD V1.0------------------------ \r\n");
    SYSLOG(
        "-------系统命令-------\r\n"
        "001 : 系统重启\r\n"
        "002 : 系列号配置6位<etc.c1000001>,要保证唯一性\r\n"
        "003 : 恢复出厂设置\r\n");

    SYSLOG("-------音频命令-------\r\n"
        "100 : 播放\r\n"
        "101 : 停止\r\n");
    SYSLOG("-------遥控器命令-------\r\n"
        "200 : 插座关<取消报警>\r\n"
        "201 : 插座开\r\n"
        "202 : 音量调节\r\n"
        "203 : 报警\r\n");

    SYSLOG("\r\nhzw@GAKATO ~# ");

}

static void SheelDealID(unsigned char *id) {

    unsigned char len = strlen((const char *)id);
    unsigned char i = 0;

    if (8 != len) {
        SYSLOG("\r\nID Length Error, Must be 8 bit\r\n");
        return ;
    }

    if (0 != memcmp(id, "c2", 2)) {
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
            SysInfoReset();
            SYSLOG("恢复出厂设置完成\r\n");
            delay_ms(1000);
            NVIC_SystemReset();
        } break;
        case 100 : {
            if (' ' != buf[3]) {
                SYSLOG("\r\nUnkonw cmd : %d\r\n", cmd);
                break;
            }
            WT588DStartPlay(buf[4] - 0x30);
        } break;
        case 101 : {
            //RF433Send(0x00231048, 20);
        } break;
        case 200 : {
            RF433Send(RF433MgrCreateCode(0), 40);
        } break;
        case 201 : {
            RF433Send(RF433MgrCreateCode(3), 40);
        } break;
        case 202 : {
            RF433Send(0x004923A1, 5);
        } break;
        case 203 : {
            RF433Send(RF433MgrCreateCode(1), 40);
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


