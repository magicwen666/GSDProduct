#include "network.h"
#include "chip.h"
#include "wifi.h"
#include "shell.h"
#include "lock315.h"
#include "delay.h"
#include "SysInfo.h"
#include "key.h"
#include "led.h"
#include "json.h"
#include "wt588d.h"
#include "UpdateInfo.h"
#include <string.h>
#include <stdlib.h>

static unsigned char NetBuffer[NET_FRAME_MAX_LEN] = {0X00};
static net_frame_t NetFrame;
static network_t network;

void NetWorkHandler(void) {

    if (0 != network.LoginTick) {
        network.LoginTick--;
    }
    
    if (0 != network.HeartTick) {
        network.HeartTick--;
    }

    if (0 != network.HeartRspTick) {
        network.HeartRspTick--;
    }
    
}

unsigned char NetWorkDataParse(net_frame_t *frame) {
    
    unsigned int len = 0;
    
    if (NULL == frame) {
        LOG("Arg Error %s %d\r\n", __FILE__, __LINE__);
        return 1;
    }
 
    memset(NetBuffer, 0x00, NET_FRAME_MAX_LEN);
    if (WIFI_RET_SUCCESS != WifiRectNetData(NetBuffer, &len)) {
        return 1;
    }
    /* frame head */
    frame->sof = &NetBuffer[0];
    if (NET_FRAME_SOF != NetBuffer[0]) {
        return 1;
    }
    frame->cmd = &NetBuffer[1];
    frame->len = (unsigned short *)&NetBuffer[2];
    frame->data = &NetBuffer[4];
    frame->lrc = frame->data + *(frame->len);
    frame->eof = frame->lrc + 1;
    if (NET_FRAME_EOF != *(frame->eof)) {
        return 1;
    }

    return 0;  
}

unsigned char NetWorkCtlTypeParse(net_frame_t *frame, unsigned int *type, unsigned char *data) {

    char *pos = (char *)(frame->data);
    char tmp[32] = {0x00};

    if ((NULL == frame) || (NULL == type) || (NULL == data) || (NULL == pos)) {
        return 1;
    }
    
    pos = strchr(pos, ':') + 1;
    if (NULL == pos) {
        return 1;
    }
    /* control type */
    memcpy(tmp, pos, strchr(pos, ';')-pos);
    sscanf(tmp, "%d", type);
    pos = strchr(pos, ';') + 1;
    if (NULL == pos) {
        return 1;
    }
    if (0 != memcmp(pos, "ctlData", 7)) {
        return 0;
    }
    pos = strchr(pos, ':') + 1;
    if (NULL == pos) {
        return 1;
    }
    /* control data */
    memcpy(data, pos, (strchr(pos, ';') - pos));
    
    return 0;
}

static void NetWorkDealMcData(unsigned char *data) {

    mc_t mc;

    SysInfoMcGet(&mc);
    mc.flag = *data;
    if ('1' == mc.flag) {
        data += 2;
        mc.time = atoi((const char *)data) * 1000;
    }
    SysInfoMcSave(mc);

}

static void NetWorkUpdateDataCreate(unsigned char *dst, unsigned int type, unsigned char *arg) {

    char tmp[128] = {0x00};

    if (NULL == dst) {
        return ;
    }
    
    sprintf((char *)dst, "devModel:%s;", SYS_DEV_MODEL);
    sprintf(tmp, "updateCmd:%d;", type);
    strcat((char *)dst, tmp);
    if (NULL == arg) {
        return ;
    }
    memset(tmp, 0x00, sizeof(tmp));
    sprintf(tmp, "updateData:%s;", arg);
    strcat((char *)dst, tmp);
    
}

static unsigned char NetWorkSendUpdateCmd(unsigned char type, unsigned char *arg) {

    net_link_info_t info;
    unsigned char dst[128] = {0x00};
    
    if (0 == network.JoinFlag) {
        return 1;
    }
    while (1 == NetWorkCreateLink(&info)) {
        DOG_FEED();
        delay_ms(100);
    }
    NetWorkUpdateDataCreate(dst, type, arg);
    NetWorkDataCreate(NET_CMD_APP_UPDATE, dst);
    if (1 == NetWorkSend(info.id)) {
        return 1;
    }
    
    return 0;
}

#define UPDATE_TIMEOUT  10000
void NetWorkDealAppUpdate(unsigned char *data) {

    net_frame_t frame;

    UpdateInfo_t info;
    char *pos1 = NULL;
    char *pos2 = NULL;
    char tmp[32] = {0x00};
    int PackId = 0;
    int intTmp = 0;
    int PackSize = 0;
    char UpdateTimeout = 0;
    unsigned long AppStoreAddr = APP_CODE_SECTOR;
    
    /* VersionName */
    /* VersionCode */
    /* PackSize */
    pos1 = strchr((char *)data, ':') + 1;
    pos2 = strchr(pos1, ',');
    memcpy(tmp, pos1, pos2-pos1);
    sscanf(tmp, "%d", &PackSize);
    info.size = PackSize;
    LOG("PackSize:%d\r\n", PackSize);
    /* md5(32) */
    pos1 = strchr(pos2, ':') + 1;
    pos2 = strchr(pos1, ',');
    memcpy(info.md5, pos1, pos2-pos1);
    /* UpdateTime */

    /* send ready update cmd to server */
    UpdateInfoAppCodeErase();
    /* recv update app code */
    NetWorkCloseLink();
    LOG("Update Start");
    while (1) {
        DOG_FEED();
        if (0 == network.HeartTick) {
            unsigned char arg[8] = {0X00};
            network.HeartTick = 10000;
            sprintf((char *)arg, "%d", PackId);
            LOG("Request ID : %d\r\n", PackId);
            NetWorkSendUpdateCmd(NET_UPDATE_REQUEST, arg);
            if (++UpdateTimeout >= 20) {
                LOG("update timeout\r\n");
                return ;
            }
        }
        if (1 == NetWorkDataParse(&frame)) {
            continue;
        }
        if (NET_CMD_APP_UPDATE != *(frame.cmd)) {
            network.HeartTick = 0;
            continue;
        }
        /* get update cmd */
        pos1 = strchr((const char *)(frame.data), ':') + 1;
        if (NULL == pos1) {
            network.HeartTick = 0;
            continue;
        }
        if (NET_UPDATE_TRANSMIT != (*pos1 - 0x30)) {
            network.HeartTick = 0;
            continue;
        }
        /* get update data */
        pos2 = strstr((const char *)(frame.data), "updateData:");
        if (NULL == pos2) {
            network.HeartTick = 0;
            continue;
        }
        pos2 = strchr(pos2, ':') + 1;
        pos1 = strchr(pos2, ',');
        if (NULL == pos1) {
            network.HeartTick = 0;
            continue;
        }
        memset(tmp, 0x00, sizeof(tmp));
        memcpy(tmp, pos2, pos1 - pos2);
        sscanf(tmp, "%d", &intTmp);
        LOG("PackId : %d\r\n", intTmp);
        if (intTmp != PackId) {
            network.HeartTick = 0;
            continue;
        }
        pos1 += 1;
        pos2 = strchr(pos1, ',');
        if (NULL == pos2) {
            network.HeartTick = 0;
            continue;
        }
        memset(tmp, 0x00, sizeof(tmp));
        memcpy(tmp, pos1, pos2 - pos1);
        sscanf(tmp, "%d", &intTmp);
        LOG("PackSize : %d\r\n", intTmp);
        SYSLOG(".");
        pos2 += 1;
        UpdateInfoAppCodeSave((void *)pos2, AppStoreAddr, intTmp);
        PackId++;
        AppStoreAddr += intTmp;
        PackSize -= intTmp;
        if (0 == PackSize) {
            /* notif to server : update finish */
            NetWorkSendWarn(NET_WARN_TYPE_UPDATE_FINISH, NULL);
            info.NeedFlag = UPDATE_FLAG_YES;
            NetWorkCloseLink();
            UpdateInfoSet(&info);
            LOG("Update Finish\r\n");
            delay_ms(100);
            NVIC_SystemReset();
            return ;
        }
        UpdateTimeout = 0;
        network.HeartTick = 0;
    }
    

}

static void NetWorkDealHeartRspData(net_frame_t *frame)  {

    unsigned int type = 0;
    unsigned char data[32] = {0x00};

    if (NET_RSP_FAILURE == frame->data[0]) {
        return ;
    }
    if (1 == NetWorkCtlTypeParse(frame, &type, data)) {
        return ;
    }
    switch (type) {
        case NET_CTL_TYPE_APP_ONLINE : {
            network.AppLineFlag = APP_LINE_FLAG_ONLINE;
            LOG("APP在线\r\n");
        } break;
        case NET_CTL_TYPE_APP_OFFLINE : {
            network.AppLineFlag = APP_LINE_FLAG_OFFLINE;
            LOG("APP离线\r\n");
        } break;
        case NET_CTL_TYPE_WARN_REMOTE : {
            if (SysInfoLockIDIsNull()) {
                return ;
            }
            Lock315Warn();
            LOG("远程报警\r\n");
        } break;
        case NET_CTL_TYPE_WARN_CANCEL : {
            if (SysInfoLockIDIsNull()) {
                return ;
            }
            Lock315WarnCancel();
            LOG("取消报警\r\n");
        } break;
        case NET_CTL_TYPE_BF : {
            SysInfoBfSave(data[0] - 0x30);
            LOG("入侵布防 : %s\r\n", ('1' == data[0]) ? "打开" : "关闭");
        } break;
        case NET_CTL_TYPE_OLOCK_REMOTE: {
            if (SysInfoLockIDIsNull()) {
                return ;
            }
            SYSLOG("远程开锁 : %s\r\n", data);
            Lock315Open();
        } break;
        case NET_CTL_TYPE_CLOCK_REMOTE: {
            if (SysInfoLockIDIsNull()) {
                return ;
            }
            LOG("远程关锁\r\n");
            Lock315Close();
        } break;
        case NET_CTL_TYPE_MC : {
            LOG("移动侦测 : %s\r\n", data);
            NetWorkDealMcData(data);
        } break;
        case NET_CTL_TYPE_WIRELESS_PWD : {
            LOG("无线密码配置 : %s\r\n", data);
            SysInfoPwdSave(data);
        } break;
        case NET_CTL_TYPE_LOCK_ASSOC : {
            LOG("锁关联\r\n");
            Lock315Assoc();
        } break;
        case NET_CTL_TYPE_RESET : {
            LOG("恢复出厂设置\r\n");
            SysInfoReset(0);
            NVIC_SystemReset();
        } break;
        case NET_CTL_TYPE_APP_UPDATE : {
            NetWorkDealAppUpdate(data);
        } break;
        default : {
        } break;
    }

}

unsigned char NetWorkRecv(void) {

    net_frame_t frame;

    if (1 == NetWorkDataParse(&frame)) {
        return 1;
    }
    switch (*(frame.cmd)) {
        case NET_CMD_LOGIN : {
            NetWorkClrRspType(NET_WAIT_RSP_LOGIN);
            LOG("登录响应\r\n");
        } break;
        case NET_CMD_HEART : {
            LOG("心跳响应\r\n");
            NetWorkDealHeartRspData(&frame);
            NetWorkClrRspType(NET_WAIT_RSP_HEART);
        } break;
        case NET_CMD_WARN : {
            NetWorkCloseLink();
            NetWorkClrRspType(NET_WAIT_RSP_WARN);
            LOG("告警响应\r\n");
        } break;
        default : {
            LOG("Net cmd <%02x> unkonw\r\n", *(frame.cmd));
        } break;
    }
    
    return 0;
}

static void WifiInfoSplitNameAndPwd(char *name, char *pwd) {

    char *pos = strchr((const char *)name, ',');
    char tmp[64] = {0x00};

    if (NULL != pwd) {
        strcpy((char *)pwd, pos+1);
    }
    memcpy(tmp, name, pos-name);
    memset(name, 0x00, strlen(name));
    strcpy(name, tmp);

}


static void NetWorkWaitGetWifiInfo(unsigned char *WifiInfo) {

    WifiOpenSmartLink();
    SYSLOG("Wait WifiInfo\r\n");
    while (1) {
        DOG_FEED();
        ShellCmdRecv();
        KeyListened();
        if (WIFI_RET_SUCCESS == WifiGetSmartLink(WifiInfo)) {
            SysInfoWifiSave(WifiInfo);
            return ;
        }
    }
    
}

static void NetWorkReportNetInfo(void) {

    char name[64] = {0x00};
    char rssi[8] = {0x00};
    char ip[32] = {0x00};
    char mac[32] = {0x00};
    char *out = NULL;
    cJSON *root = NULL;

    root = cJSON_CreateObject();
    if (NULL == root) {
        return ;
    }
    
    /* wifi name */
    SysInfoWifiGet((unsigned char *)name);
    WifiInfoSplitNameAndPwd(name, NULL);
    WifiGetIP((unsigned char *)ip);
    WifiGetRssiAndMac((unsigned char *)name, (unsigned char *)rssi, (unsigned char *)mac);
    cJSON_AddStringToObject(root, "WiFiName", name);
    cJSON_AddStringToObject(root, "WiFiRSSI", rssi);
    cJSON_AddStringToObject(root, "IP", ip);
    cJSON_AddStringToObject(root, "MAC", mac);
    out = cJSON_PrintUnformatted(root);
    NetWorkSendWarn(NET_WARN_TYPE_NETINFO, (unsigned char *)out);
    cJSON_Delete(root);
    free(out);

}

static void NetWorkWaitWifiJoin(void) {

    unsigned char data[128] = {0x00};
    unsigned int len = 0;
    unsigned int timeout = 0;
    
    while (1) {
        DOG_FEED();
        delay_ms(1);
        ShellCmdRecv();
        KeyListened();
        if (timeout++ >= 20000) {
            LOG("Join Ap Failure\r\n");
            WT588DStartPlay(WT588D_CONTEX_LINK_FAIL);
            delay_ms(100);
            NVIC_SystemReset();
        }
        if (WIFI_RET_SUCCESS != WifiRecv(data, &len)) {
            continue;
        }
        if ('K' == data[len-1]) {
            break;
        } else {
            LOG("Join Ap Failure\r\n");
            WT588DStartPlay(WT588D_CONTEX_LINK_FAIL);
            delay_ms(100);
            NVIC_SystemReset();
        }
    }

}

#define LOGIN_FLAG_START    0
#define LOGIN_FLAG_WAIT_RSP 1

void NetWorkInit(void) {

    unsigned char WifiInfo[128] = {0x00};
    unsigned char WifiPwd[64] = {0x00};
    unsigned char LoginRet = 0;
    unsigned char LoginFlag = LOGIN_FLAG_START;
    unsigned char LoginCnt = 0;
    unsigned char VoiceFlag = 0;
    

    network.AppLineFlag = APP_LINE_FLAG_OFFLINE;
    network.LoginTick = 0;
    network.HeartTick = NET_HEART_OFFLINE_TICK;
    network.HeartRspTick = 0;
    network.HeartChkFlag = 0;
    network.WaitRsp = NET_WAIT_RSP_NULL;
    network.JoinFlag = 0;
    network.port = NET_SERVER_PORT1;
    
    SysInfoWifiGet(WifiInfo);
    if (1 == SysInfoWifiIsNull(WifiInfo)) {
        memset(WifiInfo, 0x00, sizeof(WifiInfo));
        NetWorkWaitGetWifiInfo(WifiInfo);
        VoiceFlag = 1;
    }
    
    WifiInfoSplitNameAndPwd((char *)WifiInfo, (char *)WifiPwd);
    LOG("Wait Join Ap ............\r\n");
    /* wait wifi join success */
    if (WIFI_RET_JOIN_AP != WifiJoinAp(WifiInfo, WifiPwd)) {
        NetWorkWaitWifiJoin();
    }
    SYSLOG("Join Ap Success .........\r\n");
    if (1 == VoiceFlag) {
        WT588DStartPlay(WT588D_CONTEX_LINK_SUCC);
    }
    /* get link mode :single or mutex */
    WifiSetMutexLink();
    network.MutexLink = 1;
    network.JoinFlag = 1;
    NetWorkReportResetFlag();
    SYSLOG("Wait Login Server .......\r\n");
    
    while (1) {
        DOG_FEED();
        ShellCmdRecv();
        KeyListened();
        /* join success , login server and wait server respon */
        /* space time login */
        if (0 == network.LoginTick) {
            network.LoginTick = NET_LOGIN_TIMEOUT;
            if (LOGIN_FLAG_START == LoginFlag) {
                LoginRet = NetWorkLogin();
                if (2 == LoginRet) {
                    NetWorkSwapServerPort();
                } else if (0 == LoginRet) {
                    LoginFlag = LOGIN_FLAG_WAIT_RSP;
                }
            } else {
                LoginFlag = LOGIN_FLAG_START;
            }
            if (LoginCnt++ >= 15) {
                WifiResetModule();
                NVIC_SystemReset();
            }
        }
        /* wait server login respon */
        if (0 == NetWorkRecv()) {
            if (0 == NetWorkIsRspSuccess(NET_WAIT_RSP_LOGIN)) {
                /* report network info */
                NetWorkReportNetInfo();
                LEDOpen(LED_COLOR_BLUE, 5);
                SYSLOG("Login Success ! \r\n");
                return;
            }
        }
        
    }
}

void NetWorkGetLinkInfo(net_link_info_t *info) {

    char sta[256] = {0x00};
    char *p = NULL;
    char tmp[8] = {0x00};


    if (WIFI_RET_SUCCESS != WifiGetTCPLinkStatus((unsigned char *)sta)) {
        return ;
    }

    if (NULL == info) {
        return ;
    }
    info->sta = *sta;
    if (NET_LINK_STA_LINK_YES != *sta) {
        return ;
    }
    p = strchr(sta, ':') + 1;
    info->id = *p - 0x30;
    p += 3;
    memcpy(info->type, p, 3);
    p = strchr(p, ',') + 2;
    memcpy(info->ip, p, (strchr(p, '\"') - p));
    p = strchr(p, ',') + 1;
    memcpy(tmp, p, strchr(p, ',') - p);
    info->port = atoi(tmp);
    
}


void NetWorkDataCreate(unsigned char cmd, unsigned char *data) {

    unsigned int i = 0;
    unsigned char lrc = 0x00;
    net_frame_t *p = &NetFrame;
    unsigned short len = strlen((const char *)data);
    NetFrame.sof = &NetBuffer[0];
    NetFrame.cmd = &NetBuffer[1];
    NetFrame.len = (unsigned short *)&NetBuffer[2];
    NetFrame.data = &NetBuffer[4];

    memset(NetBuffer, 0x00, NET_FRAME_MAX_LEN);
    /* head */
    *(p->sof) = NET_FRAME_SOF;
    /* cmd */
    *(p->cmd) = cmd;
    lrc ^= cmd;
    /* data len */
    *(p->len) = len;
    lrc ^= (len & 0x00FF);
    lrc ^= ((len & 0xFF00) >> 8);
    /* data */
    memcpy(p->data, data, len);
    for (i = 0; i < len; i++) {
        lrc ^= data[i];
    }
    /* lrc */
    p->lrc = p->data + len;
    *(p->lrc) = lrc;
    p->eof = p->lrc + 1;
    *(p->eof) = NET_FRAME_EOF;
    
}

static void NetWorkLoginDataCreate(unsigned char *dst) {

    char tmp[64] = {0x00};
    unsigned char id[32] = {0x00};
    UpdateInfo_t info;

    if (NULL == dst) {
        return ;
    }
    /* 厂商id */
    sprintf((char *)dst, "venId:%s;", SYS_VEN_ID);
    /* 设备型号 */
    sprintf(tmp, "devModel:%s;", SYS_DEV_MODEL);
    strcat((char *)dst, tmp);
    /* 设备编号 */
    SysInfoIDGet(id);
    memset(tmp, 0x00, sizeof(tmp));
    sprintf(tmp, "devId:%s;", id);
    strcat((char *)dst, tmp);
    memset(tmp, 0x00, sizeof(tmp));
    sprintf(tmp, "lockStatus:%d;", SysInfoLockIDIsNull() ? 0 : 1);
    strcat((char *)dst, tmp);
    memset(tmp, 0x00, sizeof(tmp));
    UpdateInfoGet(&info);
    sprintf(tmp, "versionName:%s;", info.VersionName);
    strcat((char *)dst, tmp);
	memset(tmp, 0x00, sizeof(tmp));
    sprintf(tmp, "versionCode:%s;", info.VersionCode);
    strcat((char *)dst, tmp);

}

void NetWorkSwapServerPort(void) {

    if (NET_SERVER_PORT1 == network.port) {
        network.port = NET_SERVER_PORT2;
    } else {
        network.port = NET_SERVER_PORT1;
    }
    LOG("Server Port : %d\r\n", network.port);

}

unsigned char NetWorkCreateLink(net_link_info_t *info) {


    if (NULL == info) {
        return 1;
    }

    NetWorkGetLinkInfo(info);
    if (NET_LINK_STA_LINK_YES == info->sta) {
        return 0;
    }
    if (WIFI_RET_SUCCESS != WifiCreateTCPLink(NET_SERVER_IP, network.port)) {
        LOG("Create Link Failure %d; Port : %d\r\n", __LINE__, network.port);
        return 1;
    }
    NetWorkGetLinkInfo(info);
    
    return 0;    
}

unsigned char NetWorkCloseLink(void) {

    net_link_info_t info;

    NetWorkGetLinkInfo(&info);
    /* close before link */
    if (NET_LINK_STA_LINK_YES != info.sta) {
        return 1;
    }

    if (WIFI_RET_SUCCESS != WifiCloseTCPLink(info.id)) {
        return 1;
    }
    
    return 0;
}

unsigned char NetWorkSend(unsigned int id) {

    if (WIFI_RET_SUCCESS != WifiSendTCPData(network.MutexLink, id, 
                            NetBuffer, NET_FRAME_LEN(NetFrame))) {
        LOG("NetWork Send Failure %d\r\n", __LINE__);
        memset(NetBuffer, 0x00, sizeof(NetBuffer));
        return 1;
    }
#if 0
    LOG("Send >>>>>>>>>>>>>>>>>>>\r\n");
    LOG("cmd : %02x\r\n", *(NetFrame.cmd));
    LOG("len : %d\r\n", *(NetFrame.len));
    LOG("data: %s\r\n", NetFrame.data);
#endif  
    memset(NetBuffer, 0x00, sizeof(NetBuffer));
    return 0;
}

unsigned char NetWorkLogin(void) {

    net_link_info_t info;
    unsigned char dst[128] = {0x00};

    if (0 == network.JoinFlag) {
        return 1;
    }
    
    if (1 == NetWorkCreateLink(&info)) {
        return 1;
    }
    
    NetWorkLoginDataCreate(dst);
    NetWorkDataCreate(NET_CMD_LOGIN, dst);
    if (0 != NetWorkSend(info.id)) {
        LOG("Login Failure %d\r\n", __LINE__);
        return 2;
    }
    NetWorkSetRspType(NET_WAIT_RSP_LOGIN);
 
    return 0; 
}

static void NetWorkWarnDataCreate(unsigned char *dst, unsigned int type, unsigned char *arg) {

    char tmp[128] = {0x00};
    unsigned char id[32] = {0x00};

    if (NULL == dst) {
        return ;
    }
    /* 设备编号 */
    SysInfoIDGet(id);
    sprintf((char *)dst, "devId:%s;", id);
    /* 报警类型 */
    sprintf(tmp, "alertType:%d;", type);
    strcat((char *)dst, tmp);
    if (NULL == arg) {
        return ;
    }
    memset(tmp, 0x00, sizeof(tmp));
    sprintf(tmp, "alertData:%s;", arg);
    strcat((char *)dst, tmp);
    
}

unsigned char NetWorkSendWarn(unsigned char type, unsigned char *arg) {

    net_link_info_t info;
    unsigned char senderr = 0;
    unsigned char dst[128] = {0x00};
    
    if (0 == network.JoinFlag) {
        return 1;
    }
    
    while (1 == NetWorkCreateLink(&info)) {
        DOG_FEED();
        delay_ms(100);
    }
    
    NetWorkWarnDataCreate(dst, type, arg);
    NetWorkDataCreate(NET_CMD_WARN, dst);
    while (1 == NetWorkSend(info.id)) {
        DOG_FEED();
        delay_ms(200);
        if (senderr++ >= 30) {
            return 1;
        }
    }
    
    return 0;
}

unsigned char NetWorkSendHeart(void) {

    net_link_info_t info;
    unsigned char id[16] = {0x00};
    unsigned char dst[32] = {0x00};

    if (0 == network.JoinFlag) {
        return 1;
    }
    
    if (0 != network.HeartTick) {
        return 1;
    }
    
    while (1 == NetWorkCreateLink(&info)) {
        DOG_FEED();
        delay_ms(100);
    }
    

    SysInfoIDGet(id);
    sprintf((char *)dst, "devId:%s;", id);
    NetWorkDataCreate(NET_CMD_HEART, dst);
    NetWorkSetRspType(NET_WAIT_RSP_HEART);
    network.HeartRspTick = NET_HEART_RSP_TIMEOUT;
    network.HeartChkFlag = 1;
    if (1 == NetWorkSend(info.id)) {
        network.HeartTick = NET_HEART_OFFLINE_TICK;
        return 1;
    }
    if (APP_LINE_FLAG_ONLINE == network.AppLineFlag) {
        network.HeartTick = NET_HEART_ONLINE_TICK;;
    } else {
        network.HeartTick = NET_HEART_OFFLINE_TICK;
    }
    return 0;
}

static volatile unsigned char HeartErrCnt = 0;

unsigned char NetWorkCheckHeart(void) {

    if ((0 == network.HeartChkFlag) ||
        (0 != network.HeartRspTick)) {
        return 1;
    }
    
    if (0 == NetWorkIsRspSuccess(NET_WAIT_RSP_HEART)) {
        LOG("Heart Rsp Success\r\n");
        LEDOpen(LED_COLOR_BLUE, 0);
        if (APP_LINE_FLAG_OFFLINE == network.AppLineFlag) {
            NetWorkCloseLink();
        }
        network.HeartChkFlag = 0;
        HeartErrCnt = 0;
        return 0;
    } 
    LOG("Heart Rsp Failure\r\n");
    LEDOpen(LED_COLOR_RED, 0);
    network.HeartTick = 0;
    network.HeartChkFlag = 0;
    if (HeartErrCnt++ >= 30) {
        LOG("Heart Rsp Timeout\r\n");
        WifiResetModule();
        NVIC_SystemReset();
    }
    if (0 == (HeartErrCnt % 5)) {
        NetWorkSwapServerPort();
    }
    
    return 1;
}


unsigned char NetWorkSendRsp(unsigned char cmd, unsigned char rsp) {
    
    return 0;
}

void NetWorkSetRspType(net_wait_rsp_t rsp) {
    
    network.WaitRsp |= rsp;
}

void NetWorkClrRspType(net_wait_rsp_t rsp) {

    network.WaitRsp &= ~rsp;
}

unsigned char NetWorkIsRspSuccess(net_wait_rsp_t rsp) {
    
    switch (rsp) {
        case NET_WAIT_RSP_LOGIN : {
            if (NET_WAIT_RSP_LOGIN != (network.WaitRsp & rsp)) {
                return 0;
            }
        } break;
        case NET_WAIT_RSP_HEART : {
            if (NET_WAIT_RSP_HEART != (network.WaitRsp & rsp)) {
                return 0;
            }
        } break;
        case NET_WAIT_RSP_WARN : {
            if (NET_WAIT_RSP_WARN != (network.WaitRsp & rsp)) {
                return 0;
            }
        } break;
        default : {
        } break;
    }
    
    return 1;
    
}

void NetWorkReportResetFlag(void) {
    
    if (0 == SysInfoGetResetFlag()) {
        return ;
    }
    while (1 == NetWorkSendWarn(NET_WARN_TYPE_WG_RESET, NULL)) {
        DOG_FEED();
    }
    SysInfoClrResetFlag();
}



