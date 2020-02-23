#include "p33FJ128GP706.h"

#include "system\menu\menu_proc.h"


//---------переменные--------------
volatile unsigned long global_clock=0;

//----------функции----------------

//--------------------------основная часть-------------------------------------------
void __attribute__((interrupt,auto_psv)) _T1Interrupt(void)
{
	menu_button_processing_TMR1();  //определение отпущенных кнопок (защита от дребезга)
	menu_refresh();                 //автообновление менюшек
	menu_autochange();              //обработка автоповтора нажатых кнопок
	
	global_clock++;                 //счетчик увеличивается каждые 13мсек
	IFS0bits.T1IF=0;
	return;
}

void __attribute__((interrupt,auto_psv)) _CNInterrupt(void)
{
	menu_button_processing_CN();   //определение нажатых кнопок
	IFS1bits.CNIF=0; 
	return;
}

void __attribute__((interrupt,auto_psv)) _INT1Interrupt(void)
{
	IFS1bits.INT1IF=0;
	return;
}




