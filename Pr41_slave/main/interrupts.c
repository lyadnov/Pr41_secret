#include "p33FJ128GP706.h"



//---------����������--------------
volatile unsigned int mks_clock;

//----------�������----------------

//--------------------------�������� �����-------------------------------------------

void __attribute__((interrupt,auto_psv)) _T1Interrupt(void)
{
	//������� ������ RB14 
	//���� RB15 ��� ������� �� ���������� �.� �� ��������� TX\RX �� ����� USART
	if(PORTBbits.RB14==0)
		PORTBbits.RB14=1;
	else
		PORTBbits.RB14=0;
	
	//if(LATBbits.LATB0==0) LATBbits.LATB0=1;
	//else LATBbits.LATB0=0;
	
	IFS0bits.T1IF=0;
	
	return;
}


void __attribute__((interrupt,auto_psv)) _T2Interrupt(void)
{
	mks_clock += 2;
	IFS0bits.T2IF=0;
	return;
}



