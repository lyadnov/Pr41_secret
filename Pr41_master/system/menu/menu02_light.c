#include "p33FJ128GP706.h"

#include "system\eeprom\eeprom.h"
#include "system\lcd_graph\lcd_graph_low.h"
#include "system\menu\menu_proc.h"
#include "system\menu\menu00_main.h"
#include "system\menu\menu03_contrast.h"

//---------����������--------------
unsigned char light_temp;
//----------�������----------------
void menu2_out(void);
void menu2_refresh(void);
void menu2_key_pressed(void);

void menu2_2_out(void);
void menu2_2_refresh(void);
void menu2_2_key_pressed(void);
	void menu2_2_plus(void);
	void menu2_2_minus(void);

void menu2_3_out(void);
void menu2_3_refresh(void);
void menu2_3_key_pressed(void);
	void menu2_3_yes(void);

//--------------------------�������� �����-------------------------------------------

///////////////////////////////////
/*rom near*/const unsigned char menu_2[]=

//	'--------------------'
//  '                    '
//  '   ��� ���������   v'
//  '                    '
//  '�  156    �������� J'

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//  
 {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//          �  �  �     �  �  �  �  �  �  �  �  �           v
  __,__,__,ZH,_K,RI,__,RP,_O,RD,xC,_B,_E,_T,_K,_A,__,__,__,zd,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//  
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
// �        1  5  6              �  �  �  �  �  �  �  �     J                                                                            
  _M,__,__,__,__,__,__,__,__,__,RI,RZ,_M,_E,_H,RI,_T,_b,__,zj}; 

void menu2_out(void)
{
	//�������������� ���������� ����
	menu_body_out(menu_2);
	active_menu=m2;
	flags_menu.f_refresh=0;
    flags_menu.f_autochange=0;
	//�������������� ���������� menu0
	light_temp=light_level;
    //������� ����������
	menu2_refresh();
	return;
}

///////////////////////////////////
void menu2_refresh(void)
{
	//������� ������� �������� �� ���������
	menu_variable_out(3,3,light_level,3);
	OC8RS=light_level;
	return;
}

///////////////////////////////////
void menu2_key_pressed(void)
{
	switch(button_status)
    {
		case (0b00001000):	//����� ������� ������
			//menu6_out();	
			break;	
		case (0b00000100):	//����� ������ ������
			menu0_out();
			break;	
		case (0b00000010):	//������ ������� ������
			menu3_out();	
			break;	
		case (0b00000001):	//������ ������ ������
			menu2_2_out();
			break;	
		
		default:
			break;
	}

	return;
}

///////////////////////////////////////////////////////////////

///////////////////////////////////
/*rom near*/ const unsigned char menu_2_2[]=

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

void menu2_2_out(void)
{
	//�������������� ���������� ����
	menu_body_out(menu_2_2);
	active_menu=m2_2;
	flags_menu.f_refresh=0;
    flags_menu.f_autochange=1;
	autochange_rate=AUTOCHANGE_RATE_FAST;
	//�������������� ���������� menu
 	//������� ����������
	menu2_2_refresh();
    return;
}

///////////////////////////////////
void menu2_2_refresh(void)
{
	//������� ������� �������� �� ���������
	menu_variable_out(3,8,light_temp,3);
	OC8RS=light_temp; 
	return;
}

///////////////////////////////////
void menu2_2_key_pressed(void)
{
	switch(button_status)
    {
		case (0b00001000):	//����� ������� ������
			menu2_out();
			break;	
		case (0b00000100):	//����� ������ ������
			menu2_2_minus();
			break;	
		case (0b00000010):	//������ ������� ������
			menu2_3_out();
			break;	
		case (0b00000001):	//������ ������ ������
			menu2_2_plus();
			break;	
		default:
			break;
	}
	return;

}

///////////////////////////////////
void menu2_2_plus(void)
{
	light_temp++;	
	if(light_temp==128) light_temp=0;
	menu2_2_refresh();
	return;
}

///////////////////////////////////
void menu2_2_minus(void)
{
	light_temp--;	
	if(light_temp==255) light_temp=127;
	menu2_2_refresh();	
	return;
}

///////////////////////////////////////////////////////////////

///////////////////////////////////
/*rom near*/ const unsigned char menu_2_3[]=

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

void menu2_3_out(void)
{
	//�������������� ���������� ����
	menu_body_out(menu_2_3);
	active_menu=m2_3;
	flags_menu.f_refresh=0;
    flags_menu.f_autochange=0;
	//�������������� ���������� menu
 	//������� ����������
	menu2_3_refresh();
    return;
}

///////////////////////////////////
void menu2_3_refresh(void)
{
	return;
}

///////////////////////////////////
void menu2_3_key_pressed(void)
{
	switch(button_status)
    {
		case (0b00001000):	//����� ������� ������
			break;	
		case (0b00000100):	//����� ������ ������
			menu2_2_out();
			break;	
		case (0b00000010):	//������ ������� ������
			break;	
		case (0b00000001):	//������ ������ ������
			menu2_3_yes();
			break;	
		default:
			break;
	}
	return;
}

///////////////////////////////////
void menu2_3_yes(void)
{
	light_level=light_temp;
	OC8RS=light_level;
	//eeprom_write_low_int(3,light_level);
	eeprom_write_byte(ADDR_EEPROM_LIGHT,light_level);
	menu2_out();
	return;
}

