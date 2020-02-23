#include "p33FJ128GP706.h"
#include <stdio.h>

#include "main\errors.h"  

//#include "system\lcd\lcd.h"
#include "system\lcd_graph\lcd_graph_low.h"
#include "system\lcd_graph\lcd_graph_high.h"
#include "system\menu\menu_proc.h"

#include "system\menu\menu00_main.h"
#include "system\menu\menu02_light.h"
#include "system\menu\menu03_contrast.h"
#include "system\menu\menu15_about.h"
#include "system\menu\menu16_current.h"
#include "system\menu\menu17_record.h"
#include "system\menu\menu18_calibration.h"
#include "system\menu\menu19_start.h"
#include "system\menu\menu20_addr.h"
#include "system\menu\menu21_interval.h"
#include "system\menu\menu22_pause.h"

//---------переменные--------------
//глобальные
unsigned char active_menu;
unsigned char autochange_rate;
unsigned char button_status=0;
flags_menu_struct flags_menu;
//внутренние
typedef union 
{
	struct 
	{
		unsigned char f_autochange_start      :1; //разрешено автообновление менюшек или нет
	}; 
	unsigned char byte;
} flags_menu_proc_struct;
flags_menu_proc_struct flags_menu_proc;
unsigned char new_button_status;
unsigned char button1_count;
unsigned char button2_count;
unsigned char button3_count;
unsigned char button4_count;
unsigned char refresh_count=0;
unsigned char autochange_count;

//----------функции----------------
void menu_button_processing_CN(void);
void menu_button_processing_TMR1(void);

void menu_refresh(void);
void menu_key_pressed(void);

void menu_autochange(void);
void menu_autochange_start(void);
void menu_autochange_end(void);

void menu_body_out(/*rom near*/ const unsigned char* menu);
void menu_variable_out(unsigned char line,unsigned char column, unsigned int x,unsigned char len);
void menu_string_out(unsigned char line,unsigned char column, unsigned char * str,unsigned char len);
//void menu_string_out_rom(unsigned char line,unsigned char column, rom near unsigned char * str);


//--------------------------основная часть-------------------------------------------
void menu_init(void)
{
	unsigned short count=0;
	//ножки
	TRISGbits.TRISG6=1; //4 входа для кнопок
	TRISGbits.TRISG7=1;
	TRISGbits.TRISG8=1;
	TRISGbits.TRISG9=1;
	
	
	//прерывания от кнопок
	CNEN1=0;    
	CNEN2=0;
	CNEN1bits.CN8IE=1;  //RG6 вкл. прерывание на изменение сигнала на входе
	CNEN1bits.CN9IE=1;  //RG7 вкл. прерывание на изменение сигнала на входе
	CNEN1bits.CN10IE=1; //RG8 вкл. прерывание на изменение сигнала на входе
	CNEN1bits.CN11IE=1; //RG9 вкл. прерывание на изменение сигнала на входе
	CNPU1bits.CN8PUE=1;   //подтягивающие резисторы включены
	CNPU1bits.CN9PUE=1;
	CNPU1bits.CN10PUE=1;
	CNPU1bits.CN11PUE=1;
		
	//костыль №1 - на случай если пик перезагрузили от кнопки ЖК, и не успели ее отпустить - чтобы он не зашел при отпукании кнопки в какоенибудь меню
	while(count<10000)
	{
		//проверяем что все кнопки отпущены
		if( (~PORTG&0x03C0) !=0)  
		{
			//есть нажатые
			count=0;
		}
		else
		{
			//все кнопки отпущены. ждем некоторое время
			count++;
		}
	}
			
	IPC4bits.CNIP=3; //dml пробую понизить приоритет прерываний от кнопок//4;    //приоритет прерывания =4
	IFS1bits.CNIF=0;    //на всякий случай сбрасываем флаг прерывания 
	IEC1bits.CNIE=1;    //разрешаем прерывания по изменению сигнала на ножке
	
	//переменные
	flags_menu.byte=0;
	flags_menu_proc.byte=0;
	return;
}



///////////////////////////////////
void menu_button_processing_CN()
{    
	new_button_status=(~PORTG&0x03C0)>>6;
	
	//button1
	if((new_button_status&0b00000001)!=(button_status&0b00000001))
	{
		if((button_status&0b00000001)==0)   //кнопку нажали
		{
			button_status=button_status|0b00000001;
			button1_count=0;
			menu_autochange_start();
			menu_key_pressed(); 
		}
		else                                //кнопку отпустили
		{
			button1_count=BUTTON_ANTINOISE_PAUSE;   //ждем время=BUTTON_ANTINOISE_PAUSE, чтобы убедиться что кнопка точно отпущена
		}
	}
	else
	{
		button1_count=0;
	}

	//button2
	if((new_button_status&0b00000010)!=(button_status&0b00000010))
	{
		if((button_status&0b00000010)==0)
		{
			button_status=button_status|0b00000010;
			button2_count=0;
			menu_autochange_start();
			menu_key_pressed();
			
		}
		else
		{
			button2_count=BUTTON_ANTINOISE_PAUSE;
		}
	}
	else
	{
		button2_count=0;
	}

	//button3
	if((new_button_status&0b00000100)!=(button_status&0b00000100))
	{
		if((button_status&0b00000100)==0)
		{
			button_status=button_status|0b00000100;
			button3_count=0;
			menu_autochange_start();
			menu_key_pressed();
		}
		else
		{
			button3_count=BUTTON_ANTINOISE_PAUSE;
		}
	}
	else
	{
		button3_count=0;
	}

	//button4
	if((new_button_status&0b00001000)!=(button_status&0b00001000))
	{
		if((button_status&0b00001000)==0)
		{
			button_status=button_status|0b00001000;
			button4_count=0;
			menu_autochange_start();
			menu_key_pressed();
		}
		else
		{
			button4_count=BUTTON_ANTINOISE_PAUSE;
		}
	}
	else
	{
		button4_count=0;
	}
	return;
}



///////////////////////////////////
void menu_button_processing_TMR1()
{ 
	//button1
	if(button1_count!=0)
	{
		button1_count--;
		if(button1_count==0)
		{
			button_status=button_status & 0b11111110;
			menu_key_pressed();
			menu_autochange_end();  
		}
	} 
	//button2
	if(button2_count!=0)
	{
		button2_count--;
		if(button2_count==0)
		{
			button_status=button_status & 0b11111101;
			menu_key_pressed();
			menu_autochange_end();  
		}
	}
	//button3
	if(button3_count!=0)
	{
		button3_count--;
		if(button3_count==0)
		{
			button_status=button_status & 0b11111011;
			menu_key_pressed();
			menu_autochange_end();  
		}
	}
	//button4
	if(button4_count!=0)
	{
		button4_count--;
		if(button4_count==0)
		{
			button_status=button_status & 0b11110111;
			menu_key_pressed();
			menu_autochange_end();    
		}
	}
	return;
}



///////////////////////////////////
void menu_refresh()
//функция вызывается по прерыванию таймера
{
	if(flags_menu.f_refresh==0) return;
	refresh_count++;  
	if(refresh_count < REFRASH_RATE) return;
	refresh_count=0; 
	switch(active_menu)
	{
		case (m16_2):
			menu16_2_refresh();
			break;  
		case (m16_3):
			menu16_3_refresh();
			break;  
		case (m17_2):
			menu17_2_refresh();
			break;  
		case (m18_2):
			menu18_2_refresh();
			break;  
		case (m20_2):
			menu20_2_refresh();
			break;  
		case (m21_2):
			menu21_2_refresh();
			break;  
		case (m22_2):
			menu22_2_refresh();
			break;  

		default:
			error_num=err_UnknownActiveMenu;
			error_out();
			break;  
	}
	return;
}

///////////////////////////////////
void menu_key_pressed(void)
{
	switch(active_menu)
	{
		case (m0):
			menu0_key_pressed();
			break;
		case (m3):
			menu3_key_pressed();
			break;
		case (m3_2):
			menu3_2_key_pressed();
			break;
		case (m3_3):
			menu3_3_key_pressed();
			break;  
		case (m2):
			menu2_key_pressed();
			break;
		case (m2_2):
			menu2_2_key_pressed();
			break;
		case (m2_3):
			menu2_3_key_pressed();
			break;
		case (m15):
			menu15_key_pressed();
			break;
		case (m16):
			menu16_key_pressed();
			break;  
		case (m16_2):
			menu16_2_key_pressed();
			break;
		case (m16_3):
			menu16_3_key_pressed();
			break;
		case (m17):
			menu17_key_pressed();
			break;  
		case (m17_2):
			menu17_2_key_pressed();
			break;  
		case (m18):
			menu18_key_pressed();
			break;  
		case (m18_2):
			menu18_2_key_pressed();
			break;  
		case (m18_3):
			menu18_3_key_pressed();
			break;  
		case (m19):
			menu19_key_pressed();
			break;  
		case (m20):
			menu20_key_pressed();
			break;  
		case (m20_2):
			menu20_2_key_pressed();
			break;  
		case (m20_3):
			menu20_3_key_pressed();
			break;  
		case (m21):
			menu21_key_pressed();
			break;  
		case (m21_2):
			menu21_2_key_pressed();
			break;  
		case (m21_3):
			menu21_3_key_pressed();
			break;  
		case (m22):
			menu22_key_pressed();
			break;  
		case (m22_2):
			menu22_2_key_pressed();
			break;  
		case (m22_3):
			menu22_3_key_pressed();
			break;  

		default:
			error_num=err_UnknownActiveMenu;
			error_out();
			break;  
	}

	return;
}


///////////////////////////////////
void menu_autochange(void)
//функция вызывается по прерыванию таймера
{
	if(flags_menu_proc.f_autochange_start==0) return;
	if(flags_menu.f_autochange==0) return;  //для предотвращения ситуации когда с нажатой кнопкой перешли в новую менюшку, а там автонаростание запрещено
	autochange_count--;
	if(autochange_count != 0) return;
	autochange_count = autochange_rate;

	menu_key_pressed();
	return;
}

///////////////////////////////////
void menu_autochange_start(void)
//функция вызывается по прерыванию таймера
{
	if(flags_menu.f_autochange==0) return;
	flags_menu_proc.f_autochange_start=1;
	autochange_count=AUTOCHANGE_START_PAUSE;
	return;
}

///////////////////////////////////
void menu_autochange_end(void)
//функция вызывается по прерыванию таймера
{
	flags_menu_proc.f_autochange_start=0;
	return;
}

///////////////////////////////////
void menu_body_out(/*rom near*/ const unsigned char* menu)
{   //вывод 20*4 символов на экран  
	unsigned char i;
	
	graph_set_position(0,0);
	for(i=0;i<20;i++) graph_symbol_out(menu[i]);
	graph_set_position(1,0);
	for(i=0;i<20;i++) graph_symbol_out(menu[20+i]);
	graph_set_position(2,0);
	for(i=0;i<20;i++) graph_symbol_out(menu[40+i]);
	graph_set_position(3,0);
	for(i=0;i<20;i++) graph_symbol_out(menu[60+i]);
	return;
}


 
//itoa:  конвертируем n в символы в s - на выходе строка задом наперед
 void my_itoa(int n, char s[], char* len) //https://ru.wikipedia.org/wiki/Itoa_%28%D0%A1%D0%B8%29
 {
		 int i;
 
		 i = 0;
		 do
		 {       /* генерируем цифры в обратном порядке */
				 s[i++] = n % 10 + '0';   /* берем следующую цифру */
		 } while ((n /= 10) > 0);     /* удаляем */
		
		*len=i;
 }

///////////////////////////////////
void menu_variable_out(unsigned char line,unsigned char column,unsigned int x,unsigned char len)
{    
	char i;
	char str[8+1]="00000000";  
	char len_out;
	
// i=sprintf(str,"%.8u",x);
	my_itoa(x, str,&len_out);
	if( (len>8)||(len_out>8)||(len_out>len) )
	{
		error_num=err_VariableTooLong;
		error_out();
	}

	graph_set_position(line,column);
	for(i=0;i<len;i++) graph_symbol_out(str[len-1-i]);  
	return;
}


///////////////////////////////////
void menu_float_variable_out(unsigned char line,unsigned char column, unsigned int x,unsigned char len_before_point,unsigned char len_after_point)
{    
	char i;
	char str[8+1]="00000000";
	char len_out;
	char len=len_before_point+len_after_point;
//  i=sprintf(str,"%.8u",x);
	my_itoa(x, str,&len_out);
	if( (len>8) || (len_out>len) )
	{
		error_num=err_VariableTooLong;
		error_out();
	}
	
	graph_set_position(line,column);
	for(i=0;i<len_before_point;i++) graph_symbol_out(str[len-1-i]);  
	graph_symbol_out(zc);
	for(i=0;i<len_after_point;i++) graph_symbol_out(str[len-1-len_before_point-i]);  
	return;
}

///////////////////////////////////
void menu_string_out(unsigned char line,unsigned char column, unsigned char * str,unsigned char len)
{    
	unsigned char i;

	graph_set_position(line,column);
	for(i=0;i<len;i++) graph_symbol_out(str[i]);  
	return;
}

///////////////////////////////////
void menu_picture_out(unsigned char * buf,unsigned char x,unsigned char y,unsigned char xlen,unsigned char ylen)
//buf - массив с картинкой.
//x(0..121),y(=0,8,16,24) - координаты куда выводить изображение. (0,0 - левый верхний угол, 121,31 - правый нижний)
//xlen=1..122 - размеры изображения
//ylen=8,16,24,32
{
	unsigned char active_half,i,j;
		
	for(i=0;i<(ylen>>3);i++)
	{
		//устанавливаем позицию
		if(x<=60)
		{
			active_half=l;
			graph_cmd_out(active_half,x+19);    //столбец       
		}
		else 
		{
			active_half=r;
			graph_cmd_out(active_half,x-61);    //столбец       
		}
		graph_cmd_out(active_half,0xB8+i+(y>>3)); //строка
		
		for(j=0;j<xlen;j++)
		{
			//проверяем позицию
			if ( ((x+j)>=61) && (active_half!=r) )
			{
				active_half=r;
				graph_cmd_out(active_half,x+j-61);    //столбец       
				graph_cmd_out(active_half,0xB8+i+(y>>3));  //строка
			}
			//выводим данные
			graph_data_out(active_half,buf[xlen*i + j]);
		}
	}

	return;
}  
