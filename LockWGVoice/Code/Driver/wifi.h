#ifndef __WIFI_H__
#define __WIFI_H__

#define WIFI_BAUD_DEFAULT   115200
#define WIFI_MODULE_RESET   (LPC_GPIO2->DATA &= ~(1 << 7))

enum {
    WIFI_MODE_STATION = '1',
    WIFI_MODE_AP = '2',
    WIFI_MODE_AP_STATION = '3',
};

typedef enum {
    WIFI_RET_SUCCESS,
    WIFI_RET_ARGERR,
    WIFI_RET_TIMEOUT,
    WIFI_RET_FAILURE,
    WIFI_RET_JOIN_AP,
} wifi_ret_t;

#define WIFI_WAIT_SHORT_TIMEOUT     5000
#define WIFI_WAIT_LONG_TIMEOUT      20000    /* 20s */
#define WIFI_NAME       "magicwen"
#define WIFI_PWD        "1234567890"
#define WIFI_SERVER_TIMEOUT 200     /* 200s */
#define WIFI_MAX_NET_BUF_SIZE   256

extern volatile unsigned char g_flag;

void WifiHandler(void);
wifi_ret_t WifiInit(void);
void WifiSend(unsigned char *data, unsigned int len);
wifi_ret_t WifiRecv(unsigned char *data, unsigned int *len);
wifi_ret_t WifiRectNetData(void *data, unsigned int *len);
wifi_ret_t WifiWaitResp(unsigned char *data);
wifi_ret_t WifiTestAT(void);
wifi_ret_t WifiGetVersion(unsigned char *info);
wifi_ret_t WifiSetEcho(unsigned char openclose);
wifi_ret_t WifiGetMode(unsigned char *mode);
wifi_ret_t WifiSetMode(unsigned char mode);
wifi_ret_t WifiGetApList(void);
wifi_ret_t WifiSetWifiArg(unsigned char *WifiName, unsigned char *WifiPwd);
wifi_ret_t WifiGetWifiMac(unsigned char *mac);
wifi_ret_t WifiGetRssiAndMac(unsigned char *name, unsigned char *rssi, unsigned char *mac);
wifi_ret_t WifiResetModule(void);
wifi_ret_t WifiSetTcpServer(unsigned char openclose);
wifi_ret_t WifiJoinAp(unsigned char *name, unsigned char *pwd);
wifi_ret_t WifiGetAp(unsigned char *name);
wifi_ret_t WifiQuitAp(void);
wifi_ret_t WifiGetIP(unsigned char *ip);
wifi_ret_t WifiSetBaud(unsigned long baud);
wifi_ret_t WifiGetBaud(unsigned long *baud);
wifi_ret_t WifiSetMutexLink(void);
wifi_ret_t WifiSetSingleLink(void);
wifi_ret_t WifiIsMutexLink(unsigned char *mutex);
wifi_ret_t WifiCloseTCPLink(unsigned int id);
wifi_ret_t WifiSendTCPData(unsigned char mutex, unsigned int id, void *data, unsigned int len);
wifi_ret_t WifiCreateTCPLink(unsigned char *ip, unsigned int port);
wifi_ret_t WifiGetTCPLinkStatus(unsigned char *state);
wifi_ret_t WifiNetFirmwareUpdate(void);
wifi_ret_t WifiGetRAMSize(unsigned int *size);
wifi_ret_t WifiPing(unsigned char *domain);
wifi_ret_t WifiOpenSmartLink(void);
wifi_ret_t WifiGetSmartLink(unsigned char *WifiInfo);


#endif


