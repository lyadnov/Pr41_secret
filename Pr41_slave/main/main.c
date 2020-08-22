#include "p33FJ128GP706.h"

#include "main\errors.h" 
#include "main\oscillator.h"
#include "system\flash\flash.h"
#include "system\eeprom\eeprom.h"

#include "system\usart\usart.h"
#include "system\adc\adc.h"
#include "system\nmb\nmb.h"
#include "system\nmb\nmb_ext.h"


#define DEBUG 0
#define EEPROM_VERSION  0x02  //для записи новых значений параметров в EEPROM


void eeprom_write_default_values()
{
	eeprom_write_byte(ADDR_EEPROM_VERSION,EEPROM_VERSION);   //версия еепром
	nmb_eeprom_write_default_values();
	return;
}


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

	//отладка RB0 на выход для моргания диодами
	TRISBbits.TRISB0=0;
	PORTBbits.RB0=0;

	//инициаллизируем прерывания
	INTCON1=0;
	INTCON1bits.NSTDIS=1;   //запрещаем вложенные прерывания (interrupt nesting)
	INTCON2=0;
	INTCON2bits.ALTIVT=0;   //Use standard (default) vector table
		
	//-------------инициаллизация переменных в EEPROM-------------- 
	eeprom_ver=eeprom_read(ADDR_EEPROM_VERSION);  //проверка EEPROM на наличие правильных записанных в него данных  
	if(eeprom_ver!=EEPROM_VERSION) eeprom_write_default_values();

	//-------------инициаллизация периферии-----------------------  
		
	//----USART-------  + Timer8 для поиска пауз на линии
	UsartInit();
	//----АЦП----
	ADC_Init();

	nmb_init();

	return;
}


int main(void)
{
	Init();

	nmb_slave_loop();

	return 0; //never reach this place	
}

