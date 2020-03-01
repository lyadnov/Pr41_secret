#include "p33FJ128GP706.h"

#include "main\errors.h" 
#include "main\oscillator.h"
#include "system\flash\flash.h"
#include "system\eeprom\eeprom.h"
#include "system\lcd_graph\lcd_graph_low.h"
#include "system\menu\menu00_main.h"
#include "system\menu\menu_proc.h"
#include "system\usart\usart.h"
#include "system\modbus\modbus.h"
#include "system\modbus\modbus_master.h"

//---------константы---------------
#define DEBUG 0
#define EEPROM_VERSION  0x02  //для записи новых значений параметров в EEPROM

//---------переменные--------------

//----------функции----------------
void Init(void);
void eeprom_write_default_values(void);


//--------------------------основная часть-------------------------------------------

///////////////////////////////////
int main(void)
{
	unsigned short test;
	int res;

	Init();

	do
	{
		res = modbus_master_read_reg(1 ,REG_MAGIC, 1, &test); //dml!!! для отладки удалить.
		Nop();
		Nop();
		Nop();
	}	
	while(1);  //висим в пустом цикле

	return 0; //never reach this place	
}

///////////////////////////////////
void Init(void)
{
	unsigned char eeprom_ver;

	oscillator_init();
 
	//АЦП1
	AD1CON1=0;
	AD1CON2=0;
	AD1CON3=0;
	AD1CON4=0;
	AD1CSSL=0;
	AD1CSSH=0;
	AD1PCFGL=0xFFFF;
	AD1PCFGH=0xFFFF;    //все ножки цифровые входы\выходы   
	//АЦП2
	AD2CON1=0;
	AD2CON2=0;
	AD2CON3=0;
	AD2CON4=0;
	AD2CSSL=0;
	AD2PCFGL=0xFFFF;     //все ножки цифровые входы\выходы  

	//CAN
	C1CTRL1bits.REQOP=1; //disable mode
	C2CTRL1bits.REQOP=1; //disable mode

	//инициаллизация портов, все ножки - на вход по-умолчанию, чтобы не было просадки по питанию.
	TRISB=0xFFFF;  
	TRISC=0xFFFF;
	TRISD=0xFFFF;  
	TRISG=0xFFFF;  
	TRISF=0xFFFF;  
	
	//инициаллизируем прерывания
	INTCON1=0;
	INTCON1bits.NSTDIS=1;   //запрещаем вложенные прерывания
	INTCON2=0;
	INTCON2bits.ALTIVT=0;   //Use standard (default) vector table
		
		///-------------инициаллизация переменных в EEPROM-------------- 
	eeprom_ver=eeprom_read(ADDR_EEPROM_VERSION);  //проверка EEPROM на наличие правильных записанных в него данных  
	if(eeprom_ver!=EEPROM_VERSION) eeprom_write_default_values();

	
	//-------------инициаллизация периферии-----------------------  
		
		
	
	//-----ЖКИ-графический-------
	graph_init_pic();
	graph_init_external_controller();
	menu_init();
	menu0_out();

		
		//----USART-------  + Timer8 для поиска пауз на линии
	UsartInit();
		

	//-----таймер TMR1 (защита от дребезга ЖК, и др функции) 
	T1CON=0;
	PR1=0xFFFF;
	T1CONbits.TCS=0;    //Timer1 Clock Source Select bit: Internal clock (Fcy=40MHz=Fosc/2=80мгц/2
	T1CONbits.TCKPS=1;  //1:8 13мс период 
	T1CONbits.TON=1;    //включаем таймер
	IPC0bits.T1IP=4;    //приоритет прерывания =4
	IFS0bits.T1IF=0;    //на всякий случай сбрасываем флаг прерывания 
	IEC0bits.T1IE=1;    //разрешаем прерывания от таймера1


	//----modbus----
	modbus_init();

	//отладка с помощью 32-х битного таймера TMR6 (TMR7 используется для страших байтов)
//  T6CON=0;
//  T6CONbits.TCS=0;            //Timer6 Clock Source Select bit: Internal clock (Fcy=40MHz=Fosc/2=80мгц/2
//  T6CONbits.TCKPS=0/*3*/;          //1:256  1такт=25*256нс=6.4мкс
//  T6CONbits.T32=1;
//  
//  PR7=0xFFFF;
//  PR6=0xFFFF;
//  TMR6=0;
//  T6CONbits.TON=1; //включаем таймер 6
	


	
	//-------------инициаллизация прерываний----------------
	return;
}

///////////////////////////////////
void eeprom_write_default_values()
{
	eeprom_write_byte(ADDR_EEPROM_VERSION,EEPROM_VERSION);   //версия еепром
	graph_eeprom_write_default_values();                //жки

	modbus_eeprom_write_default_values();
	return;
}


