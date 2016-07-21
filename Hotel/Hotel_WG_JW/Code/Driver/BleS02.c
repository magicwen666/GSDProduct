#include "BleS02.h"
#include "chip.h"
#include "delay.h"
#include <stdlib.h>
#include <string.h>

#define BleS02Interrupt    UART1_IRQHandler

#define BLES02_MAX_LEN      256
static unsigned char BleS02RBuf[BLES02_MAX_LEN] = {0x00};
static volatile unsigned short BleS02RLen = 0x00;
#define BLES02_WAIT_IDLE    0
#define BLES02_WAIT_GS      1
#define BLES02_WAIT_LINK    2
#define BLES02_WAIT_STANDBY 3
static volatile unsigned char BleS02WaitFlag = BLES02_WAIT_IDLE;
#define BLES02_WAIT_TIMEOUT 20000
static volatile unsigned char BleS02IsTTM = 0;
#define BLES02_RECV_TTM_TIMEOUT 100
static volatile unsigned int  BleS02RecvTTMTimeout = 0;

static void BleS02IntDealGS(unsigned char r) {

    if (0x8000 != (BleS02RLen & 0x8000)) {
        BleS02RBuf[BleS02RLen] = r;
    }
    if (('F' == BleS02RBuf[BleS02RLen]) && 
        ('F' == BleS02RBuf[BleS02RLen-1]) &&
        ('O' == BleS02RBuf[BleS02RLen-2])) {
        /* finish */
        BleS02RLen++;
        BleS02RLen |= 0x8000;
        BleS02WaitFlag = BLES02_WAIT_IDLE;
        return ;
    }
    if (0x8000 != (BleS02RLen & 0x8000)) {
        BleS02RLen++;
    }
}

static void BleS02IntDealLink(unsigned char r) {

    if (0x8000 != (BleS02RLen & 0x8000)) {
        BleS02RBuf[BleS02RLen] = r;
    }

    if (('M' == BleS02RBuf[BleS02RLen]) && 
        ('T' == BleS02RBuf[BleS02RLen-1]) &&
        ('T' == BleS02RBuf[BleS02RLen-2])) {
        /* finish */
        BleS02RLen++;
        BleS02RLen |= 0x8000;
        BleS02WaitFlag = BLES02_WAIT_IDLE;
        return ;
    }

    if (0x8000 != (BleS02RLen & 0x8000)) {
        BleS02RLen++;
    }
}

static void BleS02IntDealStandby(unsigned char r) {

    if (0x8000 != (BleS02RLen & 0x8000)) {
        BleS02RBuf[BleS02RLen] = r;
    }

    if (('M' == BleS02RBuf[BleS02RLen]) && 
        ('B' == BleS02RBuf[BleS02RLen-1]) &&
        ('S' == BleS02RBuf[BleS02RLen-2])) {
        /* finish */
        BleS02RLen++;
        BleS02RLen |= 0x8000;
        BleS02WaitFlag = BLES02_WAIT_IDLE;
        return ;
    }

    if (0x8000 != (BleS02RLen & 0x8000)) {
        BleS02RLen++;
    }
}

void BleS02Interrupt(void) {

    volatile unsigned char r = 0;

    r = LPC_UART1->RBR;
    if ((1 == BleS02IsTTM) && (BLES02_WAIT_STANDBY != BleS02WaitFlag)) {
        //LOG("%02x ", r);
        BleS02RBuf[BleS02RLen++] = r;
        BleS02RecvTTMTimeout = BLES02_RECV_TTM_TIMEOUT;
        return ;
    }
    switch (BleS02WaitFlag) {
        case BLES02_WAIT_IDLE : {
        } break;
        case BLES02_WAIT_GS : {
            BleS02IntDealGS(r);
        } break;
        case BLES02_WAIT_LINK : {
            BleS02IntDealLink(r);
        } break;
        case BLES02_WAIT_STANDBY : {
            BleS02IntDealStandby(r);
        } break;
        default : {
        } break;
    }
    
}

void BleS02Handler(void) {
    
    if (0 != BleS02RecvTTMTimeout) {
        BleS02RecvTTMTimeout--;
    }
}

void BleS02Init(void) {

    syscon_ahb_enable(SYSCON_CLK_IOCON);
    /* U1_TX P0_6 : 22 */
    LPC_IOCON->PIO0_6 |= IOCON_FUNC3 | IOCON_MODE_PULLUP;
    /* U1_RX P0_7 : 23 */
    LPC_IOCON->PIO0_7 |= IOCON_FUNC3 | IOCON_MODE_PULLUP;

    uart1_init(115200);
}

void BleS02Send(void *data, unsigned int len) {

    unsigned char *p = (unsigned char *)data;

    while (len--) {
        uart1_send_byte(*p++);
        if (0 == BleS02IsTTM) {
            continue;
        }
        delay_ms(1);
    }
    delay_ms(10);
}

unsigned char BleS02Recv(void *data, unsigned int *len) {

    if ((0 == BleS02IsTTM) ||
        (0 == BleS02RLen) || 
        (0 != BleS02RecvTTMTimeout)) {
        return 1;
    }

    if (NULL != data) {
        memcpy(data, BleS02RBuf, BleS02RLen);
    }

    if (NULL != len) {
        *len = BleS02RLen;
    }
#if 0
    {
        unsigned char i = 0;
        LOG("\r\nSlave->Master:");
        for (i = 0; i < BleS02RLen; i++) {
            LOG("%02x ", BleS02RBuf[i]);
        }
        LOG("\r\n");
    }
#endif
    
    memset(BleS02RBuf, 0x00, sizeof(BleS02RBuf));
    BleS02RLen = 0;

    return 0;
}

unsigned char BleS02WaitResp(void *data, unsigned int *len) {

    unsigned int timeout = BLES02_WAIT_TIMEOUT;

    while (timeout--) {
        DOG_FEED();
        delay_ms(1);
        if (0x8000 != (BleS02RLen & 0x8000)) {
            continue;
        }
        LOG("Resp : \r\n%s\r\n", BleS02RBuf);
        if (NULL != data) {
            memcpy(data, BleS02RBuf, BleS02RLen & 0x7FFF);
        }
        if (NULL != len) {
            *len = BleS02RLen & 0x7FFF;
        }
        BleS02RLen = 0;
        memset(BleS02RBuf, 0x00, sizeof(BleS02RBuf));
        return 0;
    }

    BleS02RLen = 0;
    memset(BleS02RBuf, 0x00, sizeof(BleS02RBuf));
    return 1;
}

unsigned char BleS02GlobalScan(BleS02Info_t *info, unsigned char *cnt) {

    char cmd[BLES02_MAX_LEN*2] = {0x00};
    char *pos = NULL;
    char index = 0xFF;
    //char tmp[16] = {0X00};

    sprintf(cmd, "%s", BLES02_CMD_GS);
    BleS02WaitFlag = BLES02_WAIT_GS;
    BleS02Send(cmd, strlen(cmd));
    memset(cmd, 0x00, BLES02_MAX_LEN);
    if (1 == BleS02WaitResp(cmd, NULL)) {
        return 1;
    }

    /* get mac and name */
    /*
    AT:GS
    AT: OK
    STAT: DIS-ON
    STAT: LIST-#0-0x987BF35605F8
    STAT: NAME-#0-DZF_0210
    STAT: LIST-#1-0x987BF3560708
    STAT: NAME-#1-DZF_0210
    STAT: LIST-#2-0xA0E6F85BC99D
    STAT: NAME-#2-DH-ASL81XXX-B0
    STAT: DIS-OFF
    */
    pos = strstr(cmd, "DIS-ON");
    if (NULL == pos) {
        return 1;
    }
    while (1) {
        DOG_FEED();
        pos = strstr(pos, "\r\n") + 2;
        if (0 == memcmp(pos, "STAT: DIS-OFF", strlen("STAT: DIS-OFF"))) {
            break;
        }
        /* index */
        pos = strstr(pos, "LIST-#") + strlen("LIST-#");
        if (NULL == pos) {
            return 1;
        }
        index = *pos - 0x30;
        /* mac */
        pos += 4;
        if (NULL != info) {
            memcpy(info[index].mac, pos, 12);
        }
        
        pos = strstr(pos, "NAME-#") + strlen("NAME-#");
        if (NULL == pos) {
            continue;
        }
        /* name */
        pos += 2;
        if (NULL != info) {
            memcpy(info[index].name, pos, strstr(pos, "\r\n") - pos);
        }
    }
    
    if (NULL != cnt) {
        if (0xFF == index) {
            *cnt = 0;
        } else {
            *cnt = index + 1;

        }
    }

    return 0;
}

unsigned char BleS02LinkWithIndex(unsigned char index) {

    char cmd[BLES02_MAX_LEN] = {0x00};

    sprintf(cmd, "%s%c", BLES02_CMD_LINK_INDEX, index);
    BleS02Send(cmd, strlen(cmd));
    BleS02WaitFlag = BLES02_WAIT_LINK;
    memset(cmd, 0x00, BLES02_MAX_LEN);
    if (1 == BleS02WaitResp(cmd, NULL)) {
        return 1;
    }
    BleS02IsTTM = 1;
    
    return 0;
}

unsigned char BleS02LinkWithMac(unsigned char *mac) {

    char cmd[BLES02_MAX_LEN] = {0x00};

    if (NULL == mac) {
        return 1;
    }
    sprintf(cmd, "%s%s", BLES02_CMD_LINK_MAC, mac);
    BleS02WaitFlag = BLES02_WAIT_LINK;
    BleS02Send(cmd, strlen(cmd));
    memset(cmd, 0x00, BLES02_MAX_LEN);
    if (1 == BleS02WaitResp(cmd, NULL)) {
        return 1;
    }
    BleS02IsTTM = 1;
    
    return 0;
}

unsigned char BleS02Standby(void) {
    
    char cmd[BLES02_MAX_LEN] = {0x00};

    sprintf(cmd, "%s", BLES02_CMD_STANDBY);
    BleS02WaitFlag = BLES02_WAIT_STANDBY;
    BleS02Send(cmd, strlen(cmd));
    memset(cmd, 0x00, BLES02_MAX_LEN);
    if (1 == BleS02WaitResp(cmd, NULL)) {
        return 1;
    }
    BleS02IsTTM = 0;
    
    return 0;
}



