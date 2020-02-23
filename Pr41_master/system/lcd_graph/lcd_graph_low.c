#include "main\p33FJ128GP706.h"

#include "system\lcd_graph\lcd_graph_low.h"
#include "system\eeprom\eeprom.h"

//----------���������--------------
#define E   PORTDbits.RD5
#define RES PORTDbits.RD8 
#define CS  PORTFbits.RF6  
#define RW  PORTDbits.RD11
#define A0  PORTDbits.RD4

//---------����������--------------
unsigned char contrast_level;
unsigned char light_level;

//----------�������----------------
void pause_ms(unsigned int ms);
#define pause_250_ns   Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
//--------------------------�������� �����-------------------------------------------

///////////////////////////////////
void graph_init_pic(void)
{
	//�����
	TRISFbits.TRISF0=0; //DB0 �� �����   
	TRISFbits.TRISF1=0; //DB1
	TRISGbits.TRISG2=0; //DB2
	TRISGbits.TRISG3=0; //DB3
	TRISDbits.TRISD0=0; //DB4     
	TRISDbits.TRISD1=0; //DB5
	TRISDbits.TRISD2=0; //DB6
	TRISDbits.TRISD3=0; //DB7
	TRISDbits.TRISD4=0; //A0
	TRISDbits.TRISD5=0; //E
	TRISDbits.TRISD8=0; //Res
	TRISFbits.TRISF6=0; //CS
	TRISDbits.TRISD11=0;//RW
	TRISDbits.TRISD6=0; //�������� ���
	TRISDbits.TRISD7=0; //��������� ���

	//��������
	contrast_level=eeprom_read(ADDR_EEPROM_CONTRAST);
	//���������
	light_level=eeprom_read(ADDR_EEPROM_LIGHT);

	//������������� ���
	PR2=128;            //������ ������ �������2
	//������������� ���7 - ��������
	OC7RS=contrast_level;
	OC7R=contrast_level;
	OC7CONbits.OCTSEL=0;//Timer2 is the clock source for Compare x  
	OC7CONbits.OCM=6;   //PWM mode on OCx, Fault pin disabled
	//������������� ���8 - ���������
	OC8RS=light_level;
	OC8R=light_level;
	OC8CONbits.OCTSEL=0;//Timer2 is the clock source for Compare x  
	OC8CONbits.OCM=6;   //PWM mode on OCx, Fault pin disabled
	//������������� TMR2
	T2CONbits.T32=0;    //16-�� ��������� �����
	T2CONbits.TCS=0;    //Timerx Clock Source Select bit: Internal clock (FCY)
	T2CONbits.TCKPS=0;  //Prescale=0
	T2CONbits.TON=1;    //�������� TMR2

	return;
}


///////////////////////////////////
void graph_init_external_controller(void)
{
	unsigned char i,j;
	
	E=1;    //E=1
	RES=0;    //RES=0
	//�������� 10���
	pause_ms(1);
	RES=1;    //Res=1
	//����� 1��
	pause_ms(1);
	
	graph_cmd_out(l,0b11100010); //RESET
	graph_cmd_out(r,0b11100010); //RESET
	graph_cmd_out(l,0b11101110); //������� ������ ����� RMW(END);
	graph_cmd_out(r,0b11101110); //������� ������ ����� RMW(END);
	graph_cmd_out(l,0b10100100); //������� ���������� = Static Drive OFF
	graph_cmd_out(r,0b10100100); //������� ���������� = Static Drive OFF
	graph_cmd_out(l,0b10101001); //����� ������������ - ��� MT-12232A ������ ����������
	graph_cmd_out(r,0b10101001); //����� ������������ - ��� MT-12232A ������ ����������
	graph_cmd_out(l,0b11000000); //��������� ������ �� 0
	graph_cmd_out(r,0b11000000); //��������� ������ �� 0
	graph_cmd_out(l,0b10100001); //�������� ������������
	graph_cmd_out(r,0b10100000); //������ ������������
	graph_cmd_out(l,0b10101111); //�������� �������
	graph_cmd_out(r,0b10101111); //�������� �������
   
//    x=graph_data_read(l);
//    Nop();
//    Nop();
//    Nop();
	
	//������� ����� ��
	for(i=0;i<4;i++)
	{
		graph_cmd_out(l,0xB8+i);     //page(���������� ������) = 0
		graph_cmd_out(l,19);         //������� = 0
		graph_data_out(l,0b00000000); //0-��� ������� ������ ������    
		for(j=0;j<12;j++)
		{
			graph_data_out(l,0);
			graph_data_out(l,0);
			graph_data_out(l,0);  
			graph_data_out(l,0);
			graph_data_out(l,0);
		}
	}
	
	 //������� ������ ��
	for(i=0;i<4;i++)
	{
		graph_cmd_out(r,0xB8+i);     //page(���������� ������) = 0
		graph_cmd_out(r,0);         //������� = 0
		
		for(j=0;j<12;j++)
		{
			graph_data_out(r,0);
			graph_data_out(r,0);
			graph_data_out(r,0);  
			graph_data_out(r,0);
			graph_data_out(r,0);
		}
		graph_data_out(r,0b00000000); //61-��� ������� ������ ������    
	}

	return;
}

///////////////////////////////////
void graph_eeprom_write_default_values(void)
{
	eeprom_write_byte(ADDR_EEPROM_LIGHT,107);              //�������
  	eeprom_write_byte(ADDR_EEPROM_CONTRAST,40);              //��������
	return;
}


///////////////////////////////////
void graph_cmd_out(unsigned char left_or_right,unsigned char cmd)
{
	CS=left_or_right;   //CS �������� ����� �������� (��������)
	RW=0;               //R/W = Write
	A0=0;               //A0 = �������
	PORTF=(PORTF&0xFFFC)|(cmd & ~0xFC);
	PORTG=(PORTG&0xFFF3)|(cmd & ~0xF3);
	PORTD= (PORTD&0xFFF0) | ( (cmd&0b11110000)>>4 );
	//����� ��������� ������ = ����� >40��
	Nop(); //25��
	Nop();
	E=0;    //E=0
	//����� ��������� ������ = ����� 160��
	pause_ms(1);    //!!!
	Nop(); //25��
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	E=1;    //E=1
	//����� 1800��
	pause_250_ns
	pause_250_ns
	pause_250_ns
	pause_250_ns
	pause_250_ns
	pause_250_ns
	pause_250_ns
	pause_250_ns   
	return;
}    
	
///////////////////////////////////
void graph_data_out(unsigned char left_or_right,unsigned char cmd)
{
	CS=left_or_right;   //CS �������� ����� �������� (��������)
	RW=0;               //R/W = Write
	A0=1;               //A0 = ������
	  
	PORTF=(PORTF&0xFFFC)|(cmd & ~0xFC);
	PORTG=(PORTG&0xFFF3)|(cmd & ~0xF3);
	PORTD= (PORTD&0xFFF0) | ( (cmd&0b11110000)>>4 );
	//����� ��������� ������ = ����� >40��
	Nop(); //25��
	Nop();
	E=0;    //E=0
	//����� ��������� ������ = ����� 160��
	Nop(); //25��
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	E=1;    //E=1
	//����� 1800��
	pause_250_ns
	pause_250_ns
	pause_250_ns
	pause_250_ns
	pause_250_ns
	pause_250_ns
	pause_250_ns
	pause_250_ns   
	return;
}


///////////////////////////////////
unsigned char graph_data_read(unsigned char left_or_right)
{
	unsigned char x=0;

	TRISFbits.TRISF0=1;
	TRISFbits.TRISF1=1;
	TRISGbits.TRISG2=1;
	TRISGbits.TRISG3=1;
	TRISD |= 0x000F;

	CS=left_or_right;   //CS �������� ����� �������� (��������)
	RW=1;               //R/W = Read
	A0=0;               //A0 = �������
	//����� ������������� ������ >40��
	Nop(); 
	Nop();
	E=0;    //E=0
	//���� 300��
	Nop();Nop();Nop();Nop();
	Nop();Nop();Nop();Nop();
	Nop();Nop();Nop();Nop();
	
	x=(PORTGbits.RG3<<3)|(PORTGbits.RG2<<2)|(PORTFbits.RF1<<1)|(PORTFbits.RF0) | ((PORTD&0x000F)<<4);
	E=1;    //E=1
	//����� 1800��
	pause_250_ns
	pause_250_ns
	pause_250_ns
	pause_250_ns
	pause_250_ns
	pause_250_ns
	pause_250_ns
	pause_250_ns   
	
	TRISFbits.TRISF0=0;
	TRISFbits.TRISF1=0;
	TRISGbits.TRISG2=0;
	TRISGbits.TRISG3=0;
	
	TRISD &= 0xFFF0;
	
	return(x);
}


///////////////////////////////////
void pause_ms(unsigned int ms)
{
	unsigned int i;
	
	while(ms>0)
	{
		for(i=0;i<6000;i++)
		{
			i++;
			i--;
		}
		ms--;	
	}
	return;
}



