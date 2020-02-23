#include "p33FJ128GP706.h"

#include "system\eeprom\eeprom.h"
#include "system\lcd_graph\lcd_graph_low.h"
#include "system\menu\menu_proc.h"
#include "system\menu\menu00_main.h"
#include "system\menu\menu02_light.h"
#include "system\menu\menu15_about.h"

//---------переменные--------------
unsigned char contrast_temp;
unsigned char flag_security_menu2;

//----------функции----------------
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

//--------------------------основная часть-------------------------------------------

///////////////////////////////////
/*rom near*/ const unsigned char menu_3[]=

//	'--------------------'
//  '                    '
//  '^  ЖКИ  КОНТРАСТ    '
//  '                    '
//  'М  156    ИЗМЕНИТЬ J'

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//  
 {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
// ^        Ж  К  И        К  О  Н  Т  Р  А  С  Т    
  zu,__,__,ZH,_K,RI,__,__,_K,_O,_H,_T,xP,_A,xC,_T,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//  
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
// М        1  5  6              И  З  М  Е  Н  И  Т  Ь     J                                                                            
  _M,__,__,__,__,__,__,__,__,__,RI,RZ,_M,_E,_H,RI,_T,_b,__,zj}; 

void menu3_out(void)
{
	//инициаллизация параметров меню
	menu_body_out(menu_3);
	active_menu=m3;
	flags_menu.f_refresh=0;
    flags_menu.f_autochange=0;
    flag_security_menu2=0;
	//инициаллизация переменных menu0
	contrast_temp=contrast_level;
    //выводим переменные
	menu3_refresh();
	return;
}

///////////////////////////////////
void menu3_refresh(void)
{
	//выводим текущее значение ЖК контраста
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
		case (0b00001000):	//левая верхняя кнопка
			menu2_out();	
			break;	
		case (0b00000100):	//левая нижняя кнопка
			if(flag_security_menu2==0) flag_security_menu2=1;
			break;	
		case (0b00000010):	//правая верхняя кнопка
			break;	
		case (0b00000001):	//правая нижняя кнопка
			menu3_2_out();
			break;	
		
		//для скрытых менюшек
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
//  'E     ИЗМЕНИТЬ     J'
//  '                    '
//  '-       156        +'

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//  
 {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// E                 И  З  М  Е  Н  И  Т  Ь                 J  
  _E,__,__,__,__,__,RI,RZ,_M,_E,_H,RI,_T,_b,__,__,__,__,__,zj,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//  
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
// -                       1  5  6                          +                                                                            
  zm,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,zp}; 

void menu3_2_out(void)
{
	//инициаллизация параметров меню
	menu_body_out(menu_3_2);
	active_menu=m3_2;
	flags_menu.f_refresh=0;
    flags_menu.f_autochange=1;
	autochange_rate=AUTOCHANGE_RATE_FAST;
	//инициаллизация переменных menu
 	//выводим переменные
	menu3_2_refresh();
    return;
}

///////////////////////////////////
void menu3_2_refresh(void)
{
	//выводим текущее значение ЖК контраста
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
		case (0b00001000):	//левая верхняя кнопка
			menu3_out();
			break;	
		case (0b00000100):	//левая нижняя кнопка
			menu3_2_minus();
			break;	
		case (0b00000010):	//правая верхняя кнопка
			menu3_3_out();
			break;	
		case (0b00000001):	//правая нижняя кнопка
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
//  '      СОХРАНИТЬ     ' 
//  '      ИЗМЕНЕНИЯ?    ' 
//	'                    '   
//  'НЕТ               ДА'

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//  
 {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
//                   С  О  Х  Р  А  Н  И  Т  Ь 
  __,__,__,__,__,__,xC,_O,_X,xP,_A,_H,RI,_T,_b,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//                   И  З  М  Е  Н  Е  Н  И  Я  ?                                                                                         
  __,__,__,__,__,__,RI,RZ,_M,_E,_H,_E,_H,RI,JA,zw,__,__,__,__, 
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
// Н  Е  Т                                               Д  А                                                                           
  _H,_E,_T,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,RD,_A}; 

void menu3_3_out(void)
{
	//инициаллизация параметров меню
	menu_body_out(menu_3_3);
	active_menu=m3_3;
	flags_menu.f_refresh=0;
    flags_menu.f_autochange=0;
	//инициаллизация переменных menu
 	//выводим переменные
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
		case (0b00001000):	//левая верхняя кнопка
			break;	
		case (0b00000100):	//левая нижняя кнопка
			menu3_2_out();
			break;	
		case (0b00000010):	//правая верхняя кнопка
			break;	
		case (0b00000001):	//правая нижняя кнопка
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

