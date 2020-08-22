#include "p33FJ128GP706.h"
#include "errors.h"


unsigned char error_num=0;

void error_out(void)
{
	oscillograph_out(error_num);
	while(1);
	return;
}


void pause_ms(unsigned int ms)
{
	unsigned int i;
	
	while (ms > 0)
	{
		for (i = 0; i < 6000; i++)
		{
			i++;
			i--;
		}
		ms--;	
	}
	return;
}

void oscillograph_out(unsigned short x)
{
	int i, j;
	
	//��������� ����������
	INTCON1 = 0;
	INTCON1bits.NSTDIS = 1;   //��������� ��������� ����������
	INTCON2 = 0;
	INTCON2bits.ALTIVT = 0;   //Use standard (default) vector table
	IEC0 = 0;
	IEC1 = 0;
	IEC2 = 0;
	IEC3 = 0;
	IEC4 = 0;
	IFS0 = 0;
	IFS1 = 0;
	IFS2 = 0;
	IFS3 = 0;
	IFS4 = 0;
	 
	TRISBbits.TRISB0 = 0;
	
	j = 3;
	do
	{
		for (i = 0; i < 16; i++)
		{
			if (i == 8)
			{
				//����� ����� ������� � ������� ������
				PORTBbits.RB0 = 0;
				pause_ms(200);
			}

			PORTBbits.RB0 = 1;
			pause_ms(10);
			PORTBbits.RB0 = 0;
			pause_ms(10);
	
			PORTBbits.RB0 = ((x >> (15 - i)) & 0x01);
			pause_ms(80);
			
		}

		PORTBbits.RB0 = 0;
		pause_ms(800);
		j--;
	} while (j > 0);
}
