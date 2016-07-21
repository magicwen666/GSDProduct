#include "typedef.h"
#include "crc.h"

//---------------------------------------------------------------------------
//   CRC16
//  Results:		16 bit binary CRC
//  Side effects: 	None
//---------------------------------------------------------------------------
void CRC16(u8 *data, u8 *CRC, u8 len,u8 CrcTp)
{
	u16 crc;
	u8 i, j;
	const u16 poly = 0x8408; //CRC polynom

	if(CrcTp=='A')
	{
		crc=0X6363;
	}
	else if(CrcTp=='B') 
	{
		crc=0Xffff;
	}
	else
	{
		crc=0X5959;
	}
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
	CRC[0]=(u8)crc;
	CRC[1]=(u8)(crc>>8);
	if(CrcTp=='B')
	{
		CRC[0]^=0XFF;
		CRC[1]^=0XFF;
	}
	return;
}

unsigned char crc_8n(unsigned char *ptr,unsigned char n)
{
	unsigned char j=8;
	unsigned char cbit,out;
	unsigned char crc=0x79,crc_a=0,crc_b=0;	
	//*取移位的位*/	
	do{
		crc_a=*ptr;
		ptr++;
		j=8;
		cbit=1;
		do{
			crc_b=crc_a;
			crc_b=crc_b^crc;
			out=crc_b&cbit;
		//	crc_b=crc_b>>1;
			if(out!=0)
                        {
                                crc=crc^0X18;
                                crc=crc>>1;
                                crc|=0x80;
                        }else
                                crc=crc>>1;
			crc_a=crc_a>>1;
		//	cbit=cbit<<1;
		}while(--j);
	}while(--n);
	return crc;
}


