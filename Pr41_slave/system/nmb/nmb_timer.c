#include "p33FJ128GP706.h"
#include "system\nmb\nmb_high.h"

unsigned int nmb_clock_ms; //in ms
unsigned int nmb_timer_mode;


void nmb_timer2_stop(void)
{
	T2CONbits.TON = 0;
	IEC0bits.T2IE = 0;
	IFS0bits.T2IF = 0;
}


void nmb_timer2_callback(void)
{
	if (PORTGbits.RG2 == 1) //вход Защ. БЗ  нет/сраб
	{
		PORTGbits.RG3 = 0; //сброс АЗ БЗ (0=нет сброса, 1=сброс)
		nmb_timer2_stop();
	}
}


void nmb_timer2_start(void)
{
	T2CON = 0;
	T2CONbits.TCS = 0;   //Clock Source Select bit: Internal clock (Fcy=40MHz=Fosc/2=80мгц/2
	T2CONbits.T32 = 0;   //16bit mode
	T2CONbits.TCKPS = 2; //1:64
	PR2 = 0x3D09;        //период = 25нс * 64 * 0x3D09 = 25мс
	IPC1bits.T2IP = 5;    //приоритет прерывания = 5
	IFS0bits.T2IF = 0;    //на всякий случай сбрасываем флаг прерывания 
	IEC0bits.T2IE = 1;    //разрешаем прерывания от таймера2
	T2CONbits.TON = 1;    //включаем таймер 2
}


void nmb_timer_stop(void)
{
	T1CONbits.TON = 0;
	IEC0bits.T1IE = 0;
	IFS0bits.T1IF = 0;
	nmb_clock_ms = 0;
}


void nmb_timer_callback(void)
{
	nmb_clock_ms += 25;
	
	switch (nmb_timer_mode)
	{
		case 0:
			if (nmb_mode == 1) //Зарядка БИ
			{
				//from mode0 to mode1
				if (PORTDbits.RD9 == 0)
				{
					//СВ БЗ включился
					PORTDbits.RD4 = 0; //включение питания БЗ
					nmb_timer_stop();
				}
				else
				{
					//СВ БЗ не включился
					if (nmb_clock_ms >= 1000)
					{
						//таймаут, ошибка
						nmb_error = 1;
						PORTDbits.RD4 = 0; //включение питания БЗ
						nmb_timer_stop();
					}
				}
			}
			else if (nmb_mode == 2) //ГВИ
			{
				//from mode0 to mode2
				if (PORTDbits.RD10 == 0)
				{
					//СВ ГВИ включился
					nmb_timer_stop();
				}
				else
				{
					//СВ БЗ не включился
					if (nmb_clock_ms >= 1000)
					{
						//таймаут, ошибка
						nmb_error = 1;
						nmb_timer_stop();
					}
				}
			}
			else if (nmb_mode == 3) //БТР
			{
				//from mode0 to mode3
				if (PORTDbits.RD11 == 1)
				{
					//СВ БТР включился
					nmb_timer_stop();
				}
				else
				{
					//СВ БЗ не включился
					if (nmb_clock_ms >= 25)
					{
						//таймаут, ошибка
						nmb_error = 1;
						nmb_timer_stop();
					}
				}
			}
			break;
		case 1:
			//from mode1 to mode0
			if (PORTDbits.RD9 == 1)
			{
				//СВ БЗ выключился
				PORTDbits.RD5 = 0;  //выключение питания БЗ
				nmb_timer_stop();
			}
			else
			{
				//СВ БЗ не выключился
				if (nmb_clock_ms >= 1000)
				{
					//таймаут, ошибка
					nmb_error = 1;
					PORTDbits.RD5 = 0;  //выключение питания БЗ
					nmb_timer_stop();
				}
			}
			break;
		case 2:
			//from mode2 to mode0
			if (PORTDbits.RD10 == 1)
			{
				//СВ ГВИ выключился
				nmb_timer_stop();
			}
			else
			{
				//СВ ГВИ не выключился
				if (nmb_clock_ms >= 1000)
				{
					//таймаут, ошибка
					nmb_error = 1;
					nmb_timer_stop();
				}
			}
			break;
		case 3:
			//from mode3 to mode0
			if (PORTDbits.RD11 == 0)
			{
				//СВ БТР выключился
				nmb_timer_stop();
			}
			else
			{
				//СВ БТР не выключился
				if (nmb_clock_ms >= 25)
				{
					//таймаут, ошибка
					nmb_error = 1;
					nmb_timer_stop();
				}
			}
			break;
		default:
			nmb_timer_stop();
			break;
	}
}


int nmb_timer_is_active(void)
{
	return T1CONbits.TON;
}


void nmb_timer_start(void)
{
	nmb_clock_ms = 0;
	nmb_timer_mode = nmb_mode;
	T1CON = 0;
	T1CONbits.TCS = 0;    //Timer1 Clock Source Select bit: Internal clock (Fcy=40MHz=Fosc/2=80мгц/2
	T1CONbits.TCKPS = 2;  //1:64 
	PR1 = 0x3D09;       //период = 25нс * 64 * 0x3D09 = 25мс
	IPC0bits.T1IP = 4;  //приоритет прерывания = 4
	IFS0bits.T1IF = 0;  //на всякий случай сбрасываем флаг прерывания 
	IEC0bits.T1IE = 1;  //разрешаем прерывания от таймера1
	T1CONbits.TON = 1;  //включаем таймер
}

