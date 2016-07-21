#ifndef __KEY_H__
#define __KEY_H__

#define KEY_NONE    0
#define KEY_HAVE    0x01

typedef enum {
	KEY_STA_UP,
    KEY_STA_DOWN,
	KEY_STA_HOLD,
	KEY_STA_LHOLD,
	KEY_STA_LLHOLD,
	KEY_STA_CLICK,
	KEY_STA_LCLIC,
} key_sta_t;

void KeyHandler(void);
void KeyInit(void);
unsigned char KeyScan(void);
key_sta_t KeyStaSwitch(unsigned char keys);
void KeyListened(void);

#endif


