#ifndef __PIR_H__
#define __PIR_H__

#define PIR_READY_TIME  15000
#define PIR_SPACE_TIME  1000
#define PIR_CLEAR_TIME  30000

void PirHandler(void);
void PirInit(void);
void PirCheck(void);

#endif


