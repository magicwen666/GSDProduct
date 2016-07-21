#include "RfTx_OPEN.h"
#include "RfRx_OPEN.h"
#include "Memory.h"
#include "Crc.h"
#include "rf_time.h"
#include "chip.h"


#define BIT_ERROR   2

#define BIT_TIME 			((u16)(100*OS_M))
#define MIN_BIT_TIME 		((u16)(30*OS_M))
#define THRESHOLD_TIME 		((u16)(150*OS_M))
#define MAX_2BIT_TIME 		((u16)(280*OS_M))


#define ReadRfRxPin()	gpio_get_bitval(LPC_GPIO2, 6)


//**********************************************
//name: RxBit
//function: receive the bits
//in:the buff to save the bits
//out:the flg,
//**********************************************
u8 RxBit(u8 *Buff)
{
	u8 Rec;
	u16 TMR;
	u8 i;
	u8 j;
	u8 BitMark;
	u8 state;
	
	RF_TimerOn(MAX_2BIT_TIME);
	//throw away one bit
	Rec = ReadRfRxPin();
	while(ReadRfRxPin()==Rec)//wait for the level to change
	{
		if(RF_GetTimerFlag())//if time out,return;
		{
			state=RX_EX_MAX;
			goto OUT;
		}		
	}
	RF_TimerOn(MAX_2BIT_TIME);
	i=0;
	j=0;
	Buff[i]=0;
	while(1)
	{				
		Rec=ReadRfRxPin();
		while(ReadRfRxPin()==Rec)//wait for the level to change
		{
			if(RF_GetTimerFlag())//if time out,return;
			{
				state=RX_EX_MAX;
				goto OUT;
			}			
		}
		//TMR = RF_GetTimerCntValue();
        TMR = (u16)(LPC_CT16B0->TC);
		RF_TimerOn(MAX_2BIT_TIME);
		
		if(TMR<MIN_BIT_TIME)//the level is too short
		{
			state=RX_EX_MIN;
			goto OUT;
		}
		else if(TMR>THRESHOLD_TIME)//have received two bits
		{
			BitMark=j;
			if(Rec)//receive 1
			{
				Buff[i]|=(0X80>>j);
			}
			j++;
			if(j==8)
			{
				i++;
				Buff[i]=0;
				j=0;
			}
			if(Rec)
			{
				Buff[i]|=(0X80>>j);
			}
		}
		else//receive a bit
		{
			if(Rec)
			{
				Buff[i]|=(0X80>>j);
			}
		}
		
		j++;
		if(j==8)
		{
			i++;
			Buff[i]=0;
			j=0;
		}
		if(i==MAX_DATA_LEN)
		{
			if(BitMark&1)
			{
				state=RX_OK_ODD;
				goto OUT;
			}
			else
			{
				state=RX_OK_EVEN;
				goto OUT;
			}
		}
	}
OUT:
	RF_TimerOff();
	return state;
}

//***************************************
//name:
//function:

//in:
//out:
//***************************************
u8 RfRx(u8 *ptr,u8 n)
{
	u8 i;
	u8 j;
	u16 FrameStart;
	u8 NBit;
	u8 NByte;
	u8 Bit1s;
	u8 Buff[MAX_DATA_LEN+1];
	u8 Temp[MAX_RF_DATA_LEN+3];
	u8 state;
	u8 CrcBuff[2];
	u8 Xor;
	
	state=RxBit(Buff);
		
	if((state==RX_OK_EVEN)||(state==RX_OK_ODD))
	{
		FrameStart=0;
		NBit=0;
		NByte=0;
		Bit1s=0;
			
		for(i=0;i<MAX_DATA_LEN;i++)
		{
			for(j=state&1;j<8;j+=2)
			{
				
				if(FrameStart==((u16)(FRAME_HEAD_1<<8)+(u16)FRAME_HEAD_2))//hand found the head
				{
					if(Buff[i]&(0x80>>j))//1
					{
						Temp[NByte]<<=1;//save
						Temp[NByte]++;
						NBit++;//the number of received bits add 1
						Bit1s++;//the number of received continue 1 add 1
					}
					else
					{
                        //Bit1s=0;//clear the number of received continue 1
						if(Bit1s!=(N_FRAME_HEAD_BIT-1))
						{
							Temp[NByte]<<=1;//save
							NBit++;//the number of received bits add 1
						}
						else
						{
							continue;
						}
                        Bit1s=0;//clear the number of received continue 1
					}
					if(NBit==8)//enough one byte
					{
						NBit=0;
						NByte++;//the number of received bytes add 1

						if(NByte==1)//the frame number
						{
							continue;
						}
						
						if(Temp[RX_LEN_ADDR]>
						(n+FRAME_NUM_LEN+FRAME_LEN_LEN+RF_REC_LEN+RF_CRC_LEN))
						{
							return EX_MAX_DATA_LEN;
						}
							
						if(NByte==Temp[RX_LEN_ADDR]+FRAME_NUM_LEN+FRAME_LEN_LEN+RF_REC_LEN+RF_CRC_LEN)//enough bytes
						{
							Xor=Temp[RX_CRC_ADDR]^Temp[RX_CRC_ADDR+1];
							
							for(i=RX_TYPE_ADDR;i<RX_CRC_ADDR;i++)
							{
								Temp[i]^=Xor;
								Xor+=65;
							}
								
							CRC16(Temp+RX_LEN_ADDR, CrcBuff, 
							Temp[RX_LEN_ADDR]+FRAME_LEN_LEN+RF_REC_LEN, 'A');
							if((CrcBuff[0]!=Temp[RX_CRC_ADDR])
							||(CrcBuff[1]!=Temp[RX_CRC_ADDR+1]))//check crc
							{
								return RX_CRC_ERROR;
							}
							else
							{
								GetRecCode(CrcBuff,Temp+RX_LEN_ADDR,Temp[RX_LEN_ADDR]+FRAME_LEN_LEN);
								MemCopy(ptr, Temp+RX_NUM_ADDR, Temp[RX_LEN_ADDR]+FRAME_NUM_LEN+FRAME_LEN_LEN);
								if(MemCmp(CrcBuff, Temp+RX_REC_ADDR, RF_REC_LEN)!=OK)
								{
									return RX_REC_ERROR;
								}
								else
								{
									return OK;//
								}
							}
						}
					}
				}
				else
				{
					FrameStart<<=1;
					if(Buff[i]&(0x80>>j))
					{
						FrameStart++;
					}												
				}
			}
		}
		return RX_NO_FRAME_START;
	}
	return  state;
}

