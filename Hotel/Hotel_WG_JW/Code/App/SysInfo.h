#ifndef __SYSINFO_H__
#define __SYSINFO_H__

#define SYS_INFO_SECTOR 1 * 4096
#define SYS_INFO_SIZE   sizeof(sys_info_t)
#define SYS_INFO_USED   0x01

#define SYS_VEN_ID      "GAKATO"           /* 厂商ID */
#define SYS_DEV_MODEL   "GFM-WG-JW"        /* 设备型号 */
#define SYS_DEV_ID      "c3000000"         /* 设备编号 6位 */
#define SYS_DEV_ID_ADDR 0xF000

#pragma pack(1)

typedef struct {
    unsigned char floor;
    unsigned char num;
} sys_room_t;

typedef struct {
    unsigned char id[4];
} sys_id_t;

typedef struct {
    unsigned char used;         /* 0xFF : not used */
    unsigned char BleMac[16];
    unsigned char BleName[16];
    sys_id_t id;
    sys_room_t room;            /* room number */
} sys_info_t;

#pragma pack()

void SysInfoInit(void);
void SysInfoBleInfoSet(unsigned char *mac, unsigned char *name);
void SysInfoBleInfoGet(unsigned char *mac, unsigned char *name);
unsigned char SysInfoBleIsNull(void *mac);
void SysInfoRoomNumSet(sys_room_t room);
void SysInfoRoomNumGet(sys_room_t *room);
void SysInfoIDSet(sys_id_t id);
sys_id_t SysInfoIDGet(void);
void SysInfoReset(void);

#endif



