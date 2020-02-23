#include "system\modbus\modbus.h"
#include "p33FJ128GP706.h"

//
/************************************************************************************
		Mb_test_crc : check the crc of a packet
*************************************************************************************
input :
-------
frame  : packet with is crc
n      : lenght of the packet without tht crc
                              ^^^^^^^
answer :
--------
1 = crc fealure
0 = crc ok
************************************************************************************/
int Mb_test_crc(unsigned char frame[],int n)
{
	unsigned int crc,i,j,carry_flag,a;
	crc=0xffff;
	for (i=0;i<n;i++)
	{
		crc=crc^frame[i];
		for (j=0;j<8;j++)
		{
			a=crc;
			carry_flag=a&0x0001;
			crc=crc>>1;
			if (carry_flag==1)
				crc=crc^0xa001;
		}
	}
   //if (Mb_verbose)
   //   printf("test crc %0x %0x\n",(crc&255),(crc>>8));
	if ((frame[n+1]!=(crc>>8)) || (frame[n]!=(crc&255)))
      return 1;
   else
      return 0;
}

/************************************************************************************
		Mb_calcul_crc : compute the crc of a packet and put it at the end
*************************************************************************************
input :
-------
frame  : packet with is crc
n      : lenght of the packet without tht crc
                              ^^^^^^^
answer :
--------
crc
************************************************************************************/
int Mb_calcul_crc(unsigned char frame[],int n)
{
	unsigned int crc,i,j,carry_flag,a;
	crc=0xffff;
	for (i=0;i<n;i++)
	{
		crc=crc^frame[i];
		for (j=0;j<8;j++)
		{
			a=crc;
			carry_flag=a&0x0001;
			crc=crc>>1;
			if (carry_flag==1)
				crc=crc^0xa001;
		}
	}
	frame[n+1]=crc>>8;
	frame[n]=crc&255;
	return crc;
}


