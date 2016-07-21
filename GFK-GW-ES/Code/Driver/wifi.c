#include "wifi.h"
#include "chip.h"
#include "delay.h"
#include "shell.h"
#include <stdlib.h>
#include <string.h>

#define WifiInterrupt   UART2_IRQHandler
#define WIFI_CMD_LEN    256
#define WIFI_MAX_LEN    256
static unsigned char RBuf[WIFI_MAX_LEN] = {0x00};
static volatile unsigned short RLen = 0;
static volatile unsigned char waitEndCnt = 0;
enum {
    WIFI_CMD_STA_IDEL,
    WIFI_CMD_STA_WAIT_AT_RSP,
    WIFI_CMD_STA_NO_DEAL,
    WIFI_CMD_STA_WAIT_SMART_LINK,
};
static volatile unsigned char WifiCmdSta = WIFI_CMD_STA_IDEL;
static volatile unsigned short WifiTimeout = WIFI_WAIT_SHORT_TIMEOUT;

#define WIFI_RECV_FLAG_IDEL      0
#define WIFI_RECV_FLAG_START     1
#define WIFI_RECV_FALG_FINISH    2
static volatile unsigned char WifiRecvFlag = WIFI_RECV_FLAG_IDEL;

static unsigned char WifiBuf[WIFI_MAX_LEN] = {0x00};
static volatile unsigned short WifiLen = 0;




static void WifiIntDealNetData(unsigned char r) {

    static volatile unsigned char recvLenFlag = 0;
    static volatile unsigned short recvLen = 0;
    static volatile unsigned char calcCnt = 0;
    
    if (WIFI_RECV_FLAG_START == WifiRecvFlag) {
        RBuf[RLen] = r;
        if ((0 == recvLenFlag) && (':' == r)) {
            unsigned char *pos = &RBuf[4];
            recvLenFlag = 1;
            /*
            IPD,xxx: or IPD,0,xxx:
            calc net data length 
            */
            while (':' != *pos) {
                if (',' == *pos) {
                    recvLen = 0;
                    calcCnt++;
                    pos++;
                    continue;
                }
                recvLen *= 10;
                recvLen += (*pos - 0x30);
                pos++;
                calcCnt++;
            }
        }
        RLen++;
    }
    
    if (('+' == r) && (WIFI_RECV_FLAG_IDEL == WifiRecvFlag)) {
        WifiRecvFlag = WIFI_RECV_FLAG_START;
        return ;
    } else if ((RLen == 5+recvLen+calcCnt) && (1 == recvLenFlag)) {
        WifiRecvFlag = WIFI_RECV_FLAG_IDEL;
        WifiLen = RLen | 0x8000;
        memcpy(WifiBuf, RBuf, RLen);
        RLen = 0;
        memset(RBuf, 0x00, WIFI_MAX_LEN);
        recvLen = 0;
        recvLenFlag = 0;
        calcCnt = 0;
        return ;
    } 
    
}

static void WifiIntDealAtRspData(unsigned char r) {

    if ((' ' == r) || (0x00 == r) || 
        ('\r' == r) || ('\n' == r) || ('+' == r)) {
        return ;
    }
    RBuf[RLen++] = r;
    if ((('K' == RBuf[RLen-1]) && ('O' == RBuf[RLen-2])) ||
        (('R' == RBuf[RLen-1]) && ('O' == RBuf[RLen-2]) && ('R' == RBuf[RLen-3]))) {
        WifiLen = RLen | 0x8000;
        memcpy(WifiBuf, RBuf, RLen);
        RLen = 0;
        memset(RBuf, 0x00, WIFI_MAX_LEN);
    }
    
}

static void WifiIntDealSmartLink(unsigned char r) {

    RBuf[RLen++] = r;
    if (('K' == RBuf[RLen-1]) && ('O' == RBuf[RLen-2])) {
        WifiLen = RLen | 0x8000;
        memcpy(WifiBuf, RBuf, RLen);
        RLen = 0;
        memset(RBuf, 0x00, WIFI_MAX_LEN);
    }
    
}

void WifiInterrupt(void) {

    volatile unsigned char r = 0;

    r = LPC_UART2->RBR;
    switch (WifiCmdSta) {
        case WIFI_CMD_STA_NO_DEAL : {
            //LOG("%c", r);
        } break;
        case WIFI_CMD_STA_WAIT_SMART_LINK : {
            WifiIntDealSmartLink(r);
        } break;
        case WIFI_CMD_STA_IDEL : {
            WifiIntDealNetData(r);
        } break;
        case WIFI_CMD_STA_WAIT_AT_RSP : {
            WifiIntDealAtRspData(r);
        } break;
    }
    
}

void WifiHandler(void) {

    if (0 != WifiTimeout) {
        WifiTimeout--;
    }
    
}

wifi_ret_t WifiInit(void) {

    syscon_ahb_enable(SYSCON_CLK_IOCON);
    /* rxd p0_3 */
    LPC_IOCON->PIO0_3 |= (IOCON_FUNC3 | IOCON_MODE_PULLUP);
    /* txd p1_8 */
    LPC_IOCON->PIO1_8 |= (IOCON_FUNC3 | IOCON_MODE_PULLUP);
    /* rst p2_7 */
    LPC_IOCON->PIO2_7 |= (IOCON_FUNC0 | IOCON_MODE_PULLUP);
    /* pd  p0_2 */
    LPC_IOCON->PIO0_2 |= (IOCON_FUNC0 | IOCON_MODE_PULLUP);

    syscon_ahb_enable(SYSCON_CLK_GPIO);
    gpio_set_dir(LPC_GPIO2, 7, GPIO_OUT);
    gpio_set_dir(LPC_GPIO0, 2, GPIO_OUT);
    
    uart2_init(WIFI_BAUD_DEFAULT);

    /* wait wifi module restart complete */
    delay_ms(3000);
    if (WIFI_RET_SUCCESS != WifiSetMode(WIFI_MODE_STATION)) {
        return WIFI_RET_FAILURE;
    }
    
    return WIFI_RET_SUCCESS;
}


void WifiSend(unsigned char *data, unsigned int len) {

    WifiCmdSta = WIFI_CMD_STA_WAIT_AT_RSP;
    WifiTimeout = WIFI_WAIT_SHORT_TIMEOUT;
    uart2_send(data, len);
    
#if 0
    {
        unsigned char i = 0;
        LOG("\r\nWifi Send : ");
        for (i = 0; i < len; i++) {
            LOG("%02x ", data[i]);
        }
        LOG("\r\n");
    }
#endif
}

wifi_ret_t WifiRecv(unsigned char *data, unsigned int *len) {

    if ((NULL == data) || (NULL == len)) {
        return WIFI_RET_ARGERR;
    }
    
    if ((0x8000 != (WifiLen & 0x8000)) || (WIFI_CMD_STA_WAIT_AT_RSP != WifiCmdSta)) {
        return WIFI_RET_FAILURE;
    }
    WifiLen = WifiLen & 0x7FFF;
    *len = WifiLen;
    if (0 == memcmp(WifiBuf, "IPD", 3)) {
        WifiLen = 0;
        memset(WifiBuf, 0x00, WIFI_MAX_LEN);
        WifiCmdSta = WIFI_CMD_STA_IDEL;
        return WIFI_RET_FAILURE;
    }
    memcpy(data, WifiBuf, *len);
    WifiLen = 0;
    memset(WifiBuf, 0x00, WIFI_MAX_LEN);
    WifiCmdSta = WIFI_CMD_STA_IDEL;

    return WIFI_RET_SUCCESS;
    
}

wifi_ret_t WifiRectNetData(void *data, unsigned int *len) {

    unsigned char *pos = NULL;
    unsigned int vaildLen = 0;

    if ((0x8000 != (WifiLen & 0x8000)) || (WIFI_CMD_STA_IDEL != WifiCmdSta)) {
        return WIFI_RET_FAILURE;
    }
    WifiLen = WifiLen & 0x7FFF;
    if (0 == memcmp(WifiBuf, "IPD", 3)) { /* 网络数据以+IPD开头 */
        vaildLen = strchr((const char *)WifiBuf, ':') + 1 - (char *)WifiBuf;
        vaildLen = WifiLen - vaildLen;
        pos = &WifiBuf[4];
        if (NULL != len) {
            while (':' != *pos) {
                if (',' == *pos) {
                    *len = 0;
                    pos++;
                    continue;
                }
                *len *= 10;
                *len += (*pos - 0x30);
                pos++;
            }
        }
        pos = (unsigned char *)strchr((const char *)WifiBuf, ':') + 1;
        if (NULL != data) {
            memcpy(data, pos, vaildLen);
        }
#if 0  
        {
            unsigned char i = 0;
            LOG("NetData : %d >>> ", vaildLen);
            for (i = 0; i < vaildLen; i++) {
                LOG("%02x ", *pos++);
            }
            LOG("\r\n");
        }
#endif
    }
    WifiLen = 0;
    memset(WifiBuf, 0x00, sizeof(WifiBuf));
    
    return WIFI_RET_SUCCESS;
}

wifi_ret_t WifiWaitResp(unsigned char *data) {

    unsigned int len = 0;

    if (NULL == data) {
        return WIFI_RET_ARGERR;
    }
    
    while (1) {
        DOG_FEED();
        if (0 == WifiTimeout) {
            LOG("wifi timeout\r\n");
            break;
        }
        if (WIFI_RET_SUCCESS == WifiRecv(data, &len)) {
            //LOG("Resp >>> %s\r\n", data);
            if ('K' == data[len-1]) {
                return WIFI_RET_SUCCESS;
            } else {
                return WIFI_RET_FAILURE;
            }
        }
    }
    WifiCmdSta = WIFI_CMD_STA_IDEL;

    return WIFI_RET_TIMEOUT;
}

wifi_ret_t WifiTestAT(void) {

    unsigned char cmd[WIFI_CMD_LEN] = {0x00};

    sprintf((char *)cmd, "%s\r\n", "AT");
    WifiSend(cmd, strlen((const char *)cmd));

    return WifiWaitResp(cmd);
    
}

wifi_ret_t WifiGetVersion(unsigned char *info) {

    unsigned char cmd[WIFI_CMD_LEN] = {0x00};
    
    sprintf((char *)cmd, "%s\r\n", "AT+GMR");
    WifiSend(cmd, strlen((const char *)cmd));
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }

    if (NULL != info) {
    }
    
    return WIFI_RET_SUCCESS;
}

/* '1' : open, '0' : close */
wifi_ret_t WifiSetEcho(unsigned char openclose) {

    unsigned char cmd[WIFI_CMD_LEN] = {0x00};

    if (('1' != openclose) && ('0' != openclose)) {
        return WIFI_RET_ARGERR;
    }

    sprintf((char *)cmd, "AT+ATE%c\r\n", openclose);
    WifiSend(cmd, strlen((const char *)cmd));
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }
    
    return WIFI_RET_SUCCESS;
}


wifi_ret_t WifiGetMode(unsigned char *mode) {

    unsigned char cmd[WIFI_CMD_LEN] = {0x00};

    if (NULL == mode) {
        return WIFI_RET_ARGERR;
    }

    sprintf((char *)cmd, "%s\r\n", "AT+CWMODE?");
    WifiSend(cmd, strlen((const char *)cmd));
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }

    memcpy(mode, strchr((const char *)cmd, ':') + 1, 1);
    
    return WIFI_RET_SUCCESS;
}

wifi_ret_t WifiSetMode(unsigned char mode) {

    unsigned char cmd[WIFI_CMD_LEN] = {0x00};
    unsigned char rsp[WIFI_CMD_LEN] = {0x00};
    unsigned char trytime = 3;
    
    sprintf((char *)cmd, "AT+CWMODE=%c\r\n", mode);
    while (trytime--) {
        WifiSend(cmd, strlen((const char *)cmd));
        if (WifiWaitResp(rsp)) {
            continue;
        } else {
            return WIFI_RET_SUCCESS;
        }
    }
    
    return WIFI_RET_FAILURE;
}

wifi_ret_t WifiGetApList(void) {

    unsigned char cmd[WIFI_CMD_LEN] = {0x00};

    sprintf((char *)cmd, "%s\r\n", "AT+CWLAP");
    WifiSend(cmd, strlen((const char *)cmd));
    
    return WIFI_RET_SUCCESS;
}

wifi_ret_t WifiSetWifiArg(unsigned char *WifiName, unsigned char *WifiPwd) {
    
    unsigned char cmd[WIFI_CMD_LEN] = {0x00};

    if ((NULL == WifiName) || (NULL == WifiPwd)) {
        return WIFI_RET_ARGERR;
    }
    sprintf((char *)cmd, "AT+CWSAP=\"%s\",\"%s\",3,4\r\n", WifiName, WifiPwd);
    WifiSend(cmd, strlen((const char *)cmd));

    return WifiWaitResp(cmd);  
}

wifi_ret_t WifiGetWifiMac(unsigned char *mac) {

    unsigned char cmd[WIFI_CMD_LEN] = {0x00};

    if (NULL == mac) {
        return WIFI_RET_ARGERR;
    }

    sprintf((char *)cmd, "%s\r\n", "AT+CIPSTAMAC?");
    WifiSend(cmd, strlen((const char *)cmd));
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }
    
    memcpy(mac, strchr((const char *)cmd, '\"'), 19);
    
    return WIFI_RET_SUCCESS;  
}

wifi_ret_t WifiGetRssiAndMac(unsigned char *name, unsigned char *rssi, unsigned char *mac) {

    unsigned char cmd[WIFI_CMD_LEN] = {0x00};
    char *curr = NULL;
    char *next = NULL;
    
    if ((NULL == name) || (NULL == rssi) || (NULL == mac)) {
        return WIFI_RET_ARGERR;
    }
    sprintf((char *)cmd, "AT+CWLAP=\"%s\",\"\"\r\n", name);
    WifiSend(cmd, strlen((const char *)cmd));
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }
    /* AT+CWLAP="kaifa",""+CWLAP:(2,"kaifa",-40,"c8:3a:35:35:cd:20",6,1)OK */
    curr = strchr((const char *)cmd, ':');
    curr = strchr(curr, ',') + 1;
    curr = strchr(curr, ',') + 1;
    next = strchr(curr, ',');
    memcpy(rssi, curr, next-curr);
    curr = next + 2;
    next = strchr(curr, ',') - 1;
    memcpy(mac, curr, next-curr);

    return WIFI_RET_SUCCESS;
}

wifi_ret_t WifiResetModule(void) {

    unsigned char cmd[WIFI_CMD_LEN] = {0x00};
    unsigned int  delay = 300;

    sprintf((char *)cmd, "%s\r\n", "AT+RST");
    WifiSend(cmd, strlen((const char *)cmd));
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }
    WifiCmdSta = WIFI_CMD_STA_NO_DEAL;
    LOG("Wait ");
    while (delay--) {
        LOG("...");
        delay_ms(10);
    }
    LOG(" WIFI Reset Success\r\n");
    WifiCmdSta = WIFI_CMD_STA_IDEL;
    
    return WIFI_RET_SUCCESS;
}

/* 1:open, 0:close */
wifi_ret_t WifiSetTcpServer(unsigned char openclose) {

    unsigned char cmd[WIFI_CMD_LEN] = {0x00};

    sprintf((char *)cmd, "%s\r\n", "AT+CIPMUX=1");
    WifiSend(cmd, strlen((const char *)cmd));
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }
    
    memset(cmd, 0x00, sizeof(cmd));
    /* port:80 web server */
    sprintf((char *)cmd, "AT+CIPSERVER=%d,8000\r\n", openclose);
    WifiSend(cmd, strlen((const char *)cmd)); 
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }

    if (0 == openclose) {
        WifiResetModule();
        return WIFI_RET_SUCCESS;
    } 
    /* set timeout */
    memset(cmd, 0x00, sizeof(cmd));
    sprintf((char *)cmd, "AT+CIPSTO=%d\r\n", WIFI_SERVER_TIMEOUT);
    WifiSend(cmd, strlen((const char *)cmd)); 
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }
    
    return WIFI_RET_SUCCESS;
}

wifi_ret_t WifiJoinAp(unsigned char *name, unsigned char *pwd) {

    unsigned char cmd[WIFI_CMD_LEN] = {0X00};
    unsigned char tmp[32] = {0x00};

    if ((NULL == name) || (NULL == pwd)) {
        return WIFI_RET_ARGERR;
    }

    if (WIFI_RET_SUCCESS == WifiGetAp(tmp)) {
        if (0 == strcmp((const char *)tmp, (const char *)name)) {
            return WIFI_RET_JOIN_AP;
        }
    }

    sprintf((char *)cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", name, pwd);
    WifiSend(cmd, strlen((const char *)cmd)); 
    
    return WIFI_RET_SUCCESS;
}

wifi_ret_t WifiGetAp(unsigned char *name) {

    unsigned char cmd[WIFI_CMD_LEN] = {0x00};
    char *pos = NULL;

    if (NULL == name) {
        return WIFI_RET_ARGERR;
    }

    sprintf((char *)cmd, "%s\r\n", "AT+CWJAP?");
    WifiSend(cmd, strlen((const char *)cmd)); 
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }
    
    /* AT+CWJAP?+CWJAP:"kaifa","c8:3a:35:35:cd:20",6,-30OK or 
      AT+CWJAP?NOAPOK */
    pos = strchr((const char *)cmd, '?') + 1;
    if (0 == memcmp(pos, "NoAP", 4)) {
        return WIFI_RET_FAILURE;
    }
    pos = strchr((const char *)cmd, ':') + 2;
    memcpy(name, pos, (strchr(pos, ',') - 1 - pos));

    return WIFI_RET_SUCCESS;
}

wifi_ret_t WifiQuitAp(void) {

    unsigned char cmd[WIFI_CMD_LEN] = {0x00};

    sprintf((char *)cmd, "%s\r\n", "AT+CWQAP");
    WifiSend(cmd, strlen((const char *)cmd)); 
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }
    
    return WIFI_RET_SUCCESS;
}

wifi_ret_t WifiGetIP(unsigned char *ip) {

    unsigned char cmd[WIFI_CMD_LEN] = {0x00};
    char *pos1 = NULL;
    char *pos2 = NULL;

    sprintf((char *)cmd, "%s\r\n", "AT+CIFSR");
    WifiSend(cmd, strlen((const char *)cmd)); 
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }
    /* first " postion */
    pos1 = strchr((const char *)cmd, '\"') + 1;
    pos2 = strchr((const char *)pos1, '\"');
    if (NULL != ip) {
        memcpy(ip, pos1, pos2-pos1);
    }

    return WIFI_RET_SUCCESS;
}

wifi_ret_t WifiSetBaud(unsigned long baud) {

    unsigned char cmd[WIFI_CMD_LEN] = {0x00};
    
    sprintf((char *)cmd, "AT+UART=%ld,8,1,0,0\r\n", baud);    
    WifiSend(cmd, strlen((const char *)cmd)); 
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }

    return WIFI_RET_SUCCESS;
}

wifi_ret_t WifiGetBaud(unsigned long *baud) {

    unsigned char cmd[WIFI_CMD_LEN] = {0x00};

    if (NULL == baud) {
        return WIFI_RET_ARGERR;
    }

    sprintf((char *)cmd, "%s\r\n", "AT+UART?");
    WifiSend(cmd, strlen((const char *)cmd)); 
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }

    return WIFI_RET_SUCCESS;
}

wifi_ret_t WifiSetMutexLink(void) {

    unsigned char cmd[WIFI_CMD_LEN] = {0x00};

    sprintf((char *)cmd, "%s\r\n", "AT+CIPMUX=1");
    WifiSend(cmd, strlen((const char *)cmd));
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }
    
    return WIFI_RET_SUCCESS;
}

wifi_ret_t WifiSetSingleLink(void) {

    unsigned char cmd[WIFI_CMD_LEN] = {0x00};

    sprintf((char *)cmd, "%s\r\n", "AT+CIPMUX=0");
    WifiSend(cmd, strlen((const char *)cmd));
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }
    
    return WIFI_RET_SUCCESS;
    
}


/*
** mutex = 0 : single link
** mutex = 1 : mutex link 
*/
wifi_ret_t WifiIsMutexLink(unsigned char *mutex) {

    unsigned char cmd[WIFI_CMD_LEN] = {0x00};

    if (NULL == mutex) {
        return WIFI_RET_ARGERR;
    }

    sprintf((char *)cmd, "%s\r\n", "AT+CIPMUX?");
    WifiSend(cmd, strlen((const char *)cmd)); 
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }
    
    *mutex = (*(strchr((const char *)cmd, ':') + 1)) - 0x30;
    
    return WIFI_RET_SUCCESS;
}

wifi_ret_t WifiCreateTCPLink(unsigned char *ip, unsigned int port) {

    unsigned char mutex = 0;
    unsigned char cmd[WIFI_CMD_LEN] = {0x00};

    if (NULL == ip) {
        return WIFI_RET_ARGERR;
    }

    if (WIFI_RET_SUCCESS != WifiIsMutexLink(&mutex)) {
        return WIFI_RET_FAILURE;
    }

    if (1 == mutex) {
        sprintf((char *)cmd, "AT+CIPSTART=1,\"TCP\",\"%s\",%d\r\n", ip, port);
    } else {
        sprintf((char *)cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", ip, port);
    }
    WifiSend(cmd, strlen((const char *)cmd)); 
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }
    
    return WIFI_RET_SUCCESS;
}

wifi_ret_t WifiCloseTCPLink(unsigned int id) {
    
    unsigned char cmd[WIFI_CMD_LEN] = {0x00};

    sprintf((char *)cmd, "AT+CIPCLOSE=%d\r\n", id);
    WifiSend(cmd, strlen((const char *)cmd)); 
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }
    
    return WIFI_RET_SUCCESS;
}

wifi_ret_t WifiSendTCPData(unsigned char mutex, unsigned int id, void *data, unsigned int len) {
    
    unsigned char cmd[WIFI_CMD_LEN] = {0x00};
    
    if (1 == mutex) {
        sprintf((char *)cmd, "AT+CIPSEND=%d,%d\r\n", id, len);
    } else {
        sprintf((char *)cmd, "AT+CIPSEND=%d\r\n", len);
    }
    WifiSend(cmd, strlen((const char *)cmd)); 
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }
    
    WifiSend(data, len); 
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }
    
    return WIFI_RET_SUCCESS;
}

wifi_ret_t WifiGetTCPLinkStatus(unsigned char *state) {

    unsigned char cmd[WIFI_CMD_LEN] = {0x00};

    sprintf((char *)cmd, "%s\r\n", "AT+CIPSTATUS");
    WifiSend(cmd, strlen((const char *)cmd)); 
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }

    if (NULL != state) {
        char *p1 = strchr((const char *)cmd, ':') + 1;
        char *p2 = strstr((const char *)cmd, "OK");
        memcpy(state, p1, p2-p1);
    }
    
    return WIFI_RET_SUCCESS;
}

wifi_ret_t WifiNetFirmwareUpdate(void) {

    unsigned char cmd[WIFI_CMD_LEN] = {0x00};

    sprintf((char *)cmd, "%s\r\n", "AT+CIUPDATE");
    /* first : found server */
    WifiSend(cmd, strlen((const char *)cmd)); 
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }
    
    return WIFI_RET_SUCCESS;
}

wifi_ret_t WifiGetRAMSize(unsigned int *size) {

    unsigned char cmd[WIFI_CMD_LEN] = {0x00};

    sprintf((char *)cmd, "%s\r\n", "AT+CSYSHEAP");
    WifiSend(cmd, strlen((const char *)cmd)); 
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }

    return WIFI_RET_SUCCESS;
    
}

wifi_ret_t WifiOpenSmartLink(void) {

    unsigned char cmd[WIFI_CMD_LEN] = {0x00};

    sprintf((char *)cmd, "%s\r\n", "AT+CWSMARTSTART=1");
    WifiSend(cmd, strlen((const char *)cmd)); 
    if (WIFI_RET_SUCCESS != WifiWaitResp(cmd)) {
        return WIFI_RET_TIMEOUT;
    }
    WifiCmdSta = WIFI_CMD_STA_WAIT_SMART_LINK;
    
    return WIFI_RET_SUCCESS;
}

wifi_ret_t WifiGetSmartLink(unsigned char *WifiInfo) {

    char *pos1 = NULL;
    char *pos2 = NULL;
    char tmp[32] = {','};

    if ((0x8000 != (WifiLen & 0x8000)) || (WIFI_CMD_STA_WAIT_SMART_LINK != WifiCmdSta)) {
        return WIFI_RET_FAILURE;
    }
    WifiLen = WifiLen & 0x7FFF;
    if ((0 == WifiLen) || (NULL == WifiInfo)) {
        return WIFI_RET_FAILURE;
    }
    
    pos1 = strstr((const char *)WifiBuf, "SSID:") + 5;
    pos2 = strstr((const char *)WifiBuf, "PASSWORD:") - 2;
    if ((NULL == pos1) || (NULL == pos2)) {
        WifiCmdSta = WIFI_CMD_STA_IDEL;
        return WIFI_RET_FAILURE;
    }
    memcpy(WifiInfo, pos1, pos2 - pos1);
    pos1 = pos2 + 11;
    pos2 = strchr(pos1, '\r');
    memcpy(&tmp[1], pos1, pos2 - pos1);
    strcat((char *)WifiInfo, tmp);
    WifiLen = 0;
    memset(WifiBuf, 0x00, WIFI_MAX_LEN);
    WifiCmdSta = WIFI_CMD_STA_IDEL;

    return WIFI_RET_SUCCESS;
}



