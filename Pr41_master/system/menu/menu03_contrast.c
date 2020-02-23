#include "p33FJ128GP706.h"

#include "system\eeprom\eeprom.h"
#include "system\lcd_graph\lcd_graph_low.h"
#include "system\menu\menu_proc.h"
#include "system\menu\menu00_main.h"
#include "system\menu\menu02_light.h"
#include "system\menu\menu15_about.h"

//---------����������--------------
unsigned char contrast_temp;
unsigned char flag_security_menu2;

//----------�������----------------
void menu3_out(void);
void menu3_refresh(void);
void menu3_key_pressed(void);

void menu3_2_out(void);
void menu3_2_refresh(void);
void menu3_2_key_pressed(void);
	void menu3_2_plus(void);
	void menu3_2_minus(void);

void menu3_3_out(void);
void menu3_3_refresh(void);
void menu3_3_key_pressed(void);
	void menu3_3_yes(void);

//--------------------------�������� �����-------------------------------------------

///////////////////////////////////
/*rom near*/ const unsigned char menu_3[]=

//	'--------------------'
//  '                    '
//  '^  ���  ��������    '
//  '                    '
//  '�  156    �������� J'

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//  
 {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
// ^        �  �  �        �  �  �  �  �  �  �  �    
  zu,__,__,ZH,_K,RI,__,__,_K,_O,_H,_T,xP,_A,xC,_T,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//  
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
// �        1  5  6              �  �  �  �  �  �  �  �     J                                                                            
  _M,__,__,__,__,__,__,__,__,__,RI,RZ,_M,_E,_H,RI,_T,_b,__,zj}; 

void menu3_out(void)
{
	//�������������� ���������� ����
	menu_body_out(menu_3);
	active_menu=m3;
	flags_menu.f_refresh=0;
    flags_menu.f_autochange=0;
    flag_security_menu2=0;
	//�������������� ���������� menu0
	contrast_temp=contrast_level;
    //������� ����������
	menu3_refresh();
	return;
}

///////////////////////////////////
void menu3_refresh(void)
{
	//������� ������� �������� �� ���������
	menu_variable_out(3,3,contrast_level,3);
	OC7RS=contrast_level;
	//CCPR5L=127-contrast_level; 
	return;
}

///////////////////////////////////
void menu3_key_pressed(void)
{
	switch(button_status)
    {
		case (0b00001000):	//����� ������� ������
			menu2_out();	
			break;	
		case (0b00000100):	//����� ������ ������
			if(flag_security_menu2==0) flag_security_menu2=1;
			break;	
		case (0b00000010):	//������ ������� ������
			break;	
		case (0b00000001):	//������ ������ ������
			menu3_2_out();
			break;	
		
		//��� ������� �������
		case (0b00001100):	
			if(flag_security_menu2==1) flag_security_menu2=2;
			else flag_security_menu2=5;		 
			break;
		case (0b00000110):	
			if(flag_security_menu2==2) flag_security_menu2=3; 
			else flag_security_menu2=5;
			break;
		case (0b00000101):	
			if(flag_security_menu2==3) flag_security_menu2=4; 
			else flag_security_menu2=5;
			break;
		case (0b00000000):	
			if(flag_security_menu2==1) menu0_out();
			if(flag_security_menu2==4) menu15_out();
			flag_security_menu2=0;
			break;		
		
		default:
			break;
	}

	return;
}

///////////////////////////////////////////////////////////////

///////////////////////////////////
/*rom near*/ const unsigned char menu_3_2[]=
//	'--------------------'
//  '                    '
//  'E     ��������     J'
//  '                    '
//  '-       156        +'

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//  
 {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// E                 �  �  �  �  �  �  �  �                 J  
  _E,__,__,__,__,__,RI,RZ,_M,_E,_H,RI,_T,_b,__,__,__,__,__,zj,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//  
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
// -                       1  5  6                          +                                                                            
  zm,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,zp}; 

void menu3_2_out(void)
{
	//�������������� ���������� ����
	menu_body_out(menu_3_2);
	active_menu=m3_2;
	flags_menu.f_refresh=0;
    flags_menu.f_autochange=1;
	autochange_rate=AUTOCHANGE_RATE_FAST;
	//�������������� ���������� menu
 	//������� ����������
	menu3_2_refresh();
    return;
}

///////////////////////////////////
void menu3_2_refresh(void)
{
	//������� ������� �������� �� ���������
	menu_variable_out(3,8,contrast_temp,3);
	OC7RS=contrast_temp;
	//CCPR5L=127-contrast_temp; 
	return;
}

///////////////////////////////////
void menu3_2_key_pressed(void)
{
	switch(button_status)
    {
		case (0b00001000):	//����� ������� ������
			menu3_out();
			break;	
		case (0b00000100):	//����� ������ ������
			menu3_2_minus();
			break;	
		case (0b00000010):	//������ ������� ������
			menu3_3_out();
			break;	
		case (0b00000001):	//������ ������ ������
			menu3_2_plus();
			break;	
		default:
			break;
	}
	return;
}

///////////////////////////////////
void menu3_2_plus(void)
{
	contrast_temp++;	
	if(contrast_temp==128) contrast_temp=0;
	menu3_2_refresh();
	return;
}

///////////////////////////////////
void menu3_2_minus(void)
{
	contrast_temp--;	
	if(contrast_temp==255) contrast_temp=127;
	menu3_2_refresh();	
	return;
}

///////////////////////////////////////////////////////////////

///////////////////////////////////
/*rom near*/ const unsigned char menu_3_3[]=
//	'--------------------'
//  '      ���������     ' 
//  '      ���������?    ' 
//	'                    '   
//  '���               ��'

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//  
 {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
//                   �  �  �  �  �  �  �  �  � 
  __,__,__,__,__,__,xC,_O,_X,xP,_A,_H,RI,_T,_b,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//                   �  �  �  �  �  �  �  �  �  ?                                                                                         
  __,__,__,__,__,__,RI,RZ,_M,_E,_H,_E,_H,RI,JA,zw,__,__,__,__, 
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
// �  �  �                                               �  �                                                                           
  _H,_E,_T,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,RD,_A}; 

void menu3_3_out(void)
{
	//�������������� ���������� ����
	menu_body_out(menu_3_3);
	active_menu=m3_3;
	flags_menu.f_refresh=0;
    flags_menu.f_autochange=0;
	//�������������� ���������� menu
 	//������� ����������
	menu3_3_refresh();
    return;
}

///////////////////////////////////
void menu3_3_refresh(void)
{
	return;
}

///////////////////////////////////
void menu3_3_key_pressed(void)
{
	switch(button_status)
    {
		case (0b00001000):	//����� ������� ������
			break;	
		case (0b00000100):	//����� ������ ������
			menu3_2_out();
			break;	
		case (0b00000010):	//������ ������� ������
			break;	
		case (0b00000001):	//������ ������ ������
			menu3_3_yes();
			break;	
		default:
			break;
	}
	return;
}

///////////////////////////////////
void menu3_3_yes(void)
{
	contrast_level=contrast_temp;
	OC7RS=contrast_level;
	//CCPR5L=127-contrast_level;
	//eeprom_write_low_int(4,contrast_level);
	eeprom_write_byte(ADDR_EEPROM_CONTRAST,contrast_level);
	menu3_out();
	return;
}

