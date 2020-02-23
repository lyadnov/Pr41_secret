#include "p33FJ128GP706.h"

#include "system\eeprom\eeprom.h"
#include "system\pmode\pmode.h"
#include "system\modbus\modbus.h"

//---------константы---------------
#define TIMER2_PERIOD              80    //=2мкс = 80*25нс
#define DEFAULT_P2_INTERVAL_MKS    50

//---------переменные--------------
extern volatile unsigned int mks_clock;
unsigned char  cfg_pmode;
unsigned short cfg_p2_intervals[NUMBER_OF_P2_INTERVALS];
unsigned int *cfg_p2_intervals_current;

//----------функции----------------


//--------------------------основная часть-------------------------------------------
static void pmode_p2_start(void)
{
	int i;
	unsigned int next_timer_value = 0;

	//включаю TMR2
	TMR2=0;
	mks_clock = 0;
	T2CONbits.TON = 1;    //включаем таймер 8
	IPC1bits.T2IP = 4;    //приоритет прерывания = 4
	IFS0bits.T2IF = 0;    //на всякий случай сбрасываем флаг прерывания 
	IEC0bits.T2IE = 1;    //разрешаем прерывания от таймера1

	for (i = 0; i < NUMBER_OF_P2_INTERVALS; i++)
	{
		if (cfg_p2_intervals_current[i] != 0)
		{
			next_timer_value += cfg_p2_intervals_current[i];
			PORTDbits.RD1 = 1;
			while (mks_clock < next_timer_value);
		}

		PORTDbits.RD1 = 0;
		next_timer_value += (P2_INTERVAL_PERIOD_MKS - cfg_p2_intervals_current[i]);
		while (mks_clock < next_timer_value);
	}

	PORTDbits.RD1 = 1;
	
	//выключаю TMR2
	IEC0bits.T2IE = 0;
	IFS0bits.T2IF = 0;
	T2CONbits.TON = 0;
}


void pmode_change(unsigned int value)
{
	if (cfg_pmode == value)
		return;
	
	if (cfg_pmode == 0)
	{
		cfg_pmode = value;
		if(value == 1) {
			PORTDbits.RD2 = 1; //P1 mode alt
		}
		else if(value == 2)
			pmode_p2_start();
		else if(value == 3)	
			PORTFbits.RF1 = 1;
	}
	else
	{
		//cfg_pmode != 0
		if(value != 0)
			return; //ошибка, только через 0 можно менять значение pmode
		cfg_pmode = value;
		PORTDbits.RD2 = 0; //P1 mode alt
		PORTFbits.RF1 = 0; //P3 mode
		PORTDbits.RD1 = 0; //P2 mode
	}
}

void pmode_eeprom_write_default_values(void)
{
	int i;
	
	for (i = 0; i < NUMBER_OF_P2_INTERVALS; i++)
		eeprom_write_byte(ADDR_EEPROM_P2_INTERVAL1 + i, DEFAULT_P2_INTERVAL_MKS);
}


void pmode_init(void)
{
	int i;

	//ножки
	//C1CTRL1bits.REQOP = 1; //Set Disable mode for CAN module (на всякий случай)

	TRISDbits.TRISD2 = 0; //P1 mode alt
	TRISFbits.TRISF1 = 0; //P3 mode
	TRISDbits.TRISD1 = 0; //P2 mode
	

	//переменные EEPROM
	cfg_pmode = 0;
	for (i = 0; i < NUMBER_OF_P2_INTERVALS; i++)
		cfg_p2_intervals[i] = eeprom_read(ADDR_EEPROM_P2_INTERVAL1 + i);
	cfg_p2_intervals_current = &slave_regs[REG_CONFIG_P2_INTERVAL1];

	//прерывания
	T2CON = 0;
	T2CONbits.TCS = 0;            //Clock Source Select bit: Internal clock (Fcy=40MHz=Fosc/2=80мгц/2
	T2CONbits.T32 = 0;            //16bit mode
	T2CONbits.TCKPS = 0;          //1:1  1такт=25нс
	PR2 = TIMER2_PERIOD;          //2мкс
	
}

