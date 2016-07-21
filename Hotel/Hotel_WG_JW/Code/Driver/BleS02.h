#ifndef __BLES02_H__
#define __BLES02_H__

#define BLES02_CMD_GS           "AT:GS"
#define BLES02_CMD_LINK_INDEX   "AT:GI-#"
#define BLES02_CMD_STANDBY      "AT:MODE-SBM"
#define BLES02_CMD_LINK_MAC     "AT:DS-"

#define BLES02_SCAN_MAX_NUM     8

#pragma pack(1)
typedef struct {
    unsigned char mac[20];
    unsigned char name[32];
} BleS02Info_t;
#pragma pack()

void BleS02Handler(void);
void BleS02Init(void);
void BleS02Send(void *data, unsigned int len);
unsigned char BleS02Recv(void *data, unsigned int *len);
unsigned char BleS02WaitResp(void *data, unsigned int *len);
unsigned char BleS02GlobalScan(BleS02Info_t *info, unsigned char *cnt);
unsigned char BleS02LinkWithIndex(unsigned char index);
unsigned char BleS02LinkWithMac(unsigned char *mac);
unsigned char BleS02Standby(void);


#endif



