#ifndef __UPDATEINFO_H__
#define __UPDATEINFO_H__

/* 用户程序起始地址 */
#define APP_CODE_START_ADDR     0x1000

#define UPDATE_FLAG_NO  0
#define UPDATE_FLAG_YES 1

/* 存放升级信息 */
#define UPDATE_INFO_SECTOR      0 * 4096
/* 存放系统参数信息 */
#define SYS_INFO_SECTOR         1 * 4096
/* 存放用户程序 */
#define APP_CODE_SECTOR         10 * 4096
#define APP_CODE_SECTOR_SIZE    100
#define UPDATE_PACK_SIZE        64

#pragma pack(1)
typedef struct {
    char NeedFlag;
    char VersionName[32];   /* 0.0.1 */
    char VersionCode[64];   /* 2016051901 */
    int  size;
    char md5[32];
} UpdateInfo_t;
#pragma pack()

extern void (*AppCodeRun)(void);
void UpdateInfoInit(void);
char UpdateInfoIsUpdate(void);
void UpdateInfoSetVersion(const char *VersionName, const char *VersionCode);
void UpdateInfoSet(UpdateInfo_t *info);
void UpdateInfoGet(UpdateInfo_t *info);
void UpdateInfoAppCodeErase(void);
void UpdateInfoAppCodeSave(void * buf,unsigned long addr,unsigned int len);
void UpdateInfoAppCodeUpdate(void);

#endif



