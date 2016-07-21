#ifndef __RS485_H__
#define __RS485_H__

#define USART_RECV_MODE_IDLE    0
#define USART_RECV_MODE_SHELL   1
#define USART_RECV_MODE_RS485   2
extern volatile unsigned char UsartRecvMode;
#define USART_RECV_MAX_LEN  256
extern unsigned char UsartBuf[USART_RECV_MAX_LEN];
extern volatile unsigned short UsartLen;

void RS485Init(void);
void RS485Send(void *data, unsigned int len);
unsigned char RS485Recv(void *data, unsigned int *len);

#endif



