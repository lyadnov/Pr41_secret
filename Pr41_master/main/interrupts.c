#include "p33FJ128GP706.h"

#include "system\menu\menu_proc.h"


//---------����������--------------
volatile unsigned long global_clock=0;

//----------�������----------------

//--------------------------�������� �����-------------------------------------------
void __attribute__((interrupt,auto_psv)) _T1Interrupt(void)
{
	menu_button_processing_TMR1();  //����������� ���������� ������ (������ �� ��������)
	menu_refresh();                 //�������������� �������
	menu_autochange();              //��������� ����������� ������� ������
	
	global_clock++;                 //������� ������������� ������ 13����
	IFS0bits.T1IF=0;
	return;
}

void __attribute__((interrupt,auto_psv)) _CNInterrupt(void)
{
	menu_button_processing_CN();   //����������� ������� ������
	IFS1bits.CNIF=0; 
	return;
}

void __attribute__((interrupt,auto_psv)) _INT1Interrupt(void)
{
	IFS1bits.INT1IF=0;
	return;
}




