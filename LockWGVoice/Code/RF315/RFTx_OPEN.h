#ifndef __RFTX_OPEN_H__
#define __RFTX_OPEN_H__

#include "typedef.h"

#define RfTxON()    gpio_set_bitval(LPC_GPIO2, 0, 1)
#define RfTxOFF()   gpio_set_bitval(LPC_GPIO2, 0, 0)

void RfTx (u8 *Data, u8 Datalen, u8 nTime);
void GetRecCode(u8 *RecCode, u8 *data, u8 len);
void RfTxDataIncode(u8 *Data, u8 Datalen,u8 nTime);
u8 GetTxBit(void);

#endif

