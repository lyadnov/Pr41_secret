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
#define EEPROM_VERSION  0x02  //��� ������ ����� �������� ���������� � EEPROM


void eeprom_write_default_values()
{
	eeprom_write_byte(ADDR_EEPROM_VERSION,EEPROM_VERSION);   //������ ������
	nmb_eeprom_write_default_values();
	return;
}


void Init(void)
{
	unsigned char eeprom_ver;

	oscillator_init();
 
	//���1
	AD1CON1=0;
	AD1CON2=0;
	AD1CON3=0;
	AD1CON4=0;
	AD1CSSL=0;
	AD1CSSH=0;
	AD1PCFGL=0xFFFF;
	AD1PCFGH=0xFFFF;    //��� ����� �������� �����\������
	//���2
	AD2CON1=0;
	AD2CON2=0;
	AD2CON3=0;
	AD2CON4=0;
	AD2CSSL=0;
	AD2PCFGL=0xFFFF;     //��� ����� �������� �����\������
	
	//CAN
	C1CTRL1bits.REQOP=1; //disable mode
	C2CTRL1bits.REQOP=1; //disable mode

	//�������������� ������, ��� ����� - �� ���� ��-���������, ����� �� ���� �������� �� �������.
	TRISB=0xFFFF;  
	TRISC=0xFFFF;
	TRISD=0xFFFF;  
	TRISG=0xFFFF;  
	TRISF=0xFFFF;  

	//������� RB0 �� ����� ��� �������� �������
	TRISBbits.TRISB0=0;
	PORTBbits.RB0=0;

	//��������������� ����������
	INTCON1=0;
	INTCON1bits.NSTDIS=1;   //��������� ��������� ���������� (interrupt nesting)
	INTCON2=0;
	INTCON2bits.ALTIVT=0;   //Use standard (default) vector table
		
	//-------------�������������� ���������� � EEPROM-------------- 
	eeprom_ver=eeprom_read(ADDR_EEPROM_VERSION);  //�������� EEPROM �� ������� ���������� ���������� � ���� ������  
	if(eeprom_ver!=EEPROM_VERSION) eeprom_write_default_values();

	//-------------�������������� ���������-----------------------  
		
	//----USART-------  + Timer8 ��� ������ ���� �� �����
	UsartInit();
	//----���----
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

