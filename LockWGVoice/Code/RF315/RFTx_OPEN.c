#include "RfTx_OPEN.h"
#include "RfRx_OPEN.h"
#include "Memory.h"
#include "Crc.h"
#include "rf_time.h"
#include "SysInfo.h"

//∫Í∂®“Â£∫
#define HALF_BIT ((u16)(100*OS_M))	  

//
u8 Buff[FRAME_HEAD_LEN+FRAME_NUM_LEN+FRAME_LEN_LEN+
		RF_SNR_LEN+RF_TYPE_LEN+RF_ID_LEN+
		RF_CMD_LEN+RF_MAX_DATA_LEN+
		RF_REC_LEN+RF_CRC_LEN+RF_ADD_BYTE_LEN];//8 is the length of data
u16 nBit;
u16 TxBit;

//*****************************************
//name: GetRecCode
//function:according the given data and the key,generate the rec code.
//in:the key and the data,include:
//SNR,ID,CMD,DATA,DataLen:the length of the DATA.
//out:the rec code
//******************************************
void GetRecCode(u8 *RecCode, u8 *data, u8 len)
{
	u8 i, j;
	u32 crc;
	u32 poly = 0x84089604; //CRC polynom
	SysInfoPwdGet(&crc);
	
	for (i=0;i<len;i++)
	{
		crc ^= data[i];
		for (j=0; j<8; j++)
		{
			if (crc&1) 
			{
				crc = (crc>>1) ^ poly;
			}
			else 
			{
				crc = (crc>>1);
			}
		}
	}
	RecCode[0]=(u8)crc;
	RecCode[1]=(u8)(crc>>8);
	return;
}

//*****************************************
//name: RfTxDataIncode
//function:data incode before tx
//in:the key and the data which want to send,include:
//SNR,ID,CMD,DATA,DataLen:the length of the DATA.
//nTX:the times of Tx 
//out:none
//******************************************
void RfTxDataIncode(u8 *Data, u8 Datalen,u8 nTime)
{
	u8 Xor;
	u8 i,j,k,l;
	u8 Record1s;
	TxBit=0;
	nBit=(u16)(FRAME_HEAD_LEN+FRAME_NUM_LEN)*8;
	Buff[FRAME_NUM_ADDR]=nTime;

	Buff[RF_ADD_BYTE_LEN+FRAME_LEN_ADDR]=RF_SNR_LEN+RF_TYPE_LEN+
	RF_ID_LEN+RF_CMD_LEN+Datalen;
	MemCopy(Buff+RF_ADD_BYTE_LEN+RF_TYPE_ADDR, Data, 
	RF_SNR_LEN+RF_TYPE_LEN+RF_ID_LEN+RF_CMD_LEN+Datalen);
	GetRecCode(Buff+RF_ADD_BYTE_LEN+RF_REC_ADDR, Buff+RF_ADD_BYTE_LEN+FRAME_LEN_ADDR, 
	FRAME_LEN_LEN+RF_SNR_LEN+RF_TYPE_LEN+RF_ID_LEN+RF_CMD_LEN+Datalen);
	CRC16(Buff+RF_ADD_BYTE_LEN+FRAME_LEN_ADDR, Buff+RF_ADD_BYTE_LEN+RF_CRC_ADDR, 
	FRAME_LEN_LEN+RF_SNR_LEN+RF_TYPE_LEN+RF_ID_LEN+RF_CMD_LEN+RF_REC_LEN+Datalen, 'A');

	Xor=Buff[RF_ADD_BYTE_LEN+RF_CRC_ADDR]^Buff[RF_ADD_BYTE_LEN+RF_CRC_ADDR+1];
	for(i=RF_TYPE_ADDR;i<RF_CRC_ADDR;i++)
	{
		Buff[RF_ADD_BYTE_LEN+i]^=Xor;
		Xor+=65;
	}

	Buff[FRAME_HEAD_ADDR]=FRAME_HEAD_1;
	Buff[FRAME_HEAD_ADDR+1]=FRAME_HEAD_2;
	
	k=FRAME_LEN_ADDR;
	l=0;
	
	for(i=FRAME_LEN_ADDR;i<RF_CRC_ADDR+RF_CRC_LEN;i++)
	{
		for(j=0;j<8;j++)
		{
			nBit++;
			if(Buff[RF_ADD_BYTE_LEN+i]&(0X80>>j))//1		
			{
				Buff[k]|=(0X80>>l);
				Record1s++;
			}
			else//0
			{
				Buff[k]&=(~(0X80>>l));
				Record1s=0;//clear the Record1s
			}
			l++;
			if(l==8)
			{
				l=0;
				k++;
			}
			if(Record1s==(N_FRAME_HEAD_BIT-1))//if send six 1,set a 0,
			{
				Buff[k]&=(~(0X80>>l));
				nBit++;
				Record1s=0; 
				l++;
				if(l==8)
				{
					l=0;
					k++;
				}
			}
		}		
	}
	nBit<<=1;
}

//*******************************************
//name: GetTxBit
//function:get a bit to send
//in: none
//out: the bit to send,if no bit to send,return 0XFF
//*******************************************
u8 GetTxBit(void)
{
	u8 Bit;

	if(TxBit>(nBit-1))
	{
		Buff[FRAME_NUM_ADDR]--;
		if(!Buff[FRAME_NUM_ADDR])
		{
			return 0XFF;
		}
		TxBit=0;
	}
	
	if(Buff[(u8)(TxBit>>4)]&(0X80>>(((u8)(TxBit>>1))&7)))
	{
		Bit=0;
	}
	else
	{
		Bit=1;
	}

	if(TxBit&1)
	{
		Bit^=1;
	}

	TxBit++;
	return Bit;
}

//******************************************
//name£∫RfTx
//function£∫send data by RF
//in:Key:the key to encode the data,
//data:the data to send,include the SNR,the ID,the cmd and the data,
//nTx:how many times to send.
//out£∫none°£
//*******************************************
void RfTx (u8 *Data, u8 Datalen, u8 nTime)
{
	u8 Temp;
    
	RfTxDataIncode(Data, Datalen, nTime);
	RF_TimerOn(HALF_BIT);
	while(1)
	{
		Temp=GetTxBit();
		if(Temp==1)
		{
			while(!RF_GetTimerFlag());
			RfTxON();
			RF_ClrTimerFlag();
		}
		else if(Temp==0)
		{
			while(!RF_GetTimerFlag());
			RfTxOFF();
			RF_ClrTimerFlag();
		}
		else
		{
			RfTxOFF();
			RF_TimerOff();
			RF_ClrTimerFlag();
			return;
		}
	}
}

