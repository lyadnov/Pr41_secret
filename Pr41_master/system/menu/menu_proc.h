#ifndef _MENU_PROC_INCLUDED
#define _MENU_PROC_INCLUDED

////----------структуры--------------
//typedef struct
//{
//    unsigned char xlen;     //размер рисунка в пикселях по горизонтали
//    unsigned char ylen;     //размер рисунка в пикселях по ветикали
//    unsigned char* data;    //указатель на буфер с данными      
//} picture_struct;

//----------константы--------------
#define BUTTON_ANTINOISE_PAUSE  5/*10*/ //время через которое кнока считается отпущенной

#define AUTOCHANGE_START_PAUSE  100 //начальная пауза после которой начнется автонарастание нажатой кнопки

#define AUTOCHANGE_RATE_FAST    5   //интервал автонарастания при нажатии и удержании кнопки
#define AUTOCHANGE_RATE_MEDIUM  10
#define AUTOCHANGE_RATE_SLOW    20

#define REFRASH_RATE            5 //10  //интервал обновления менюшек 5*13мс=65мс

#define CURSOR_ON_TIME          3
#define CURSOR_OFF_TIME         6


//---------переменные--------------
typedef union 
{
	struct 
	{
		unsigned char f_refresh 	    :1; //разрешено автообновление менюшек или нет
		unsigned char f_autochange      :1; //разрешено залипание кнопок в данном меню или нет
	}; 
	unsigned char byte;
} flags_menu_struct;
extern flags_menu_struct flags_menu;

extern unsigned char active_menu;
  //значения переменной active_menu
  #define m0    0
  #define m2    1
  #define m2_2  2
  #define m2_3  3
  #define m3    4
  #define m3_2  5
  #define m3_3  6
  #define m15   7
  #define m16   8
  #define m16_2 9
  #define m16_3 10
  #define m17   11
  #define m17_2 12
  #define m18   13
  #define m18_2 14
  #define m18_3 15
  #define m19   16
  #define m20   17
  #define m20_2 18
  #define m20_3 19
  #define m21   20
  #define m21_2 21
  #define m21_3 22
  #define m22   23
  #define m22_2 24
  #define m22_3 25

//для курсора
typedef union 
{
	struct                                  //управление курсором
	{
		unsigned char f_cursor_on        :1; 
	}; 
	unsigned char byte;
} flags_cursor_struct;

typedef struct  
{
	flags_cursor_struct flags;
	unsigned char position;  //текущая позиция курсора
	unsigned char count;     //счетчик, управляет морганием курсора    
} cursor_struct;


extern unsigned char button_status;
extern unsigned char autochange_rate;
//----------функции----------------
void menu_init(void);

void menu_button_processing_CN(void);
void menu_button_processing_TMR1(void);

void menu_refresh(void);
void menu_key_pressed(void);
void menu_autochange(void);

void menu_body_out(/*rom near*/const unsigned char* menu);
void menu_variable_out(unsigned char line,unsigned char column, unsigned int x,unsigned char len);
void menu_float_variable_out(unsigned char line,unsigned char column, unsigned int x,unsigned char len_before_point,unsigned char len_after_point);
void menu_string_out(unsigned char line,unsigned char column, unsigned char * str,unsigned char len);
void menu_picture_out(unsigned char * buf,unsigned char x,unsigned char y,unsigned char xlen,unsigned char ylen);

#define z0	0x00		//->
#define z1	0x01    //<-
#define z2	0x02    //[
#define z3	0x03    //]
#define z4	0x04 
#define z5	0x05 
#define z6	0x06 
#define z7	0x07 

#define __	0x20 	//' '
#define zz	0x2e 	//'.' 
#define zc	0x2c 	//',' comma
#define zw	0x3f 	//'?' question
#define zp	0x2b 	//'+'
#define zm	0x2d 	//'-'
#define ze	0x3d 	//'=' equal	
#define zu	0xd9 	//'^' up
#define zd	0xda 	//'v' down
#define zo	0x3e 	//'>' over
#define zl	0x3c 	//'<' less
#define zr	0x25 	//'%' peRcent
#define zj	0x7e 	//'J' enter
#define zs	0x2f 	//'/'
#define zt	0x3A    //:
#define zb	0xff	//'-' black 
#define za  0x5B    //'[' букв не осталось :)
#define zf  0x5D    //']'
 

#define _0	0x30  
#define _1	0x31 
#define _2	0x32 
#define _3	0x33 
#define _4	0x34 
#define _5	0x35 
#define _6	0x36 
#define _7	0x37 
#define _8	0x38 
#define _9	0x39 
#define _A	0x41 
#define _B	0x42 
#define xC	0x43 
#define _D	0x44 
#define _E	0x45 
#define _F	0x46 
#define _G	0x47 
#define _H 	0x48 
#define _I	0x49 
#define _J	0x4A 
#define _K	0x4B 
#define _L	0x4C 
#define _M	0x4D 
#define xN	0x4E 
#define _O	0x4F 
#define xP	0x50 
#define _Q	0x51 
#define _R	0x52 
#define xS	0x53 
#define _T	0x54 
#define _U	0x55 
#define _V	0x56 
#define _W	0x57 
#define _X	0x58 
#define _Y	0x59 
#define xZ	0x5A 

#define _a	0x61 
#define _b	0x62 
#define _c	0x63 
#define _d	0x64 
#define _e	0x65 
#define _f	0x66 
#define _g	0x67 
#define _h	0x68 
#define _i	0x69 
#define _j	0x6A 
#define _k	0x6B 
#define _l	0x6C 
#define _m	0x6D 
#define _n	0x6E 
#define _o	0x6F 
#define _p	0x70 
#define _q	0x71 
#define _r	0x72 
#define _s	0x73 
#define _t	0x74 
#define _u	0x75 
#define _v	0x76 
#define _w	0x77 
#define _x	0x78 
#define _y	0x79 
#define _z	0x7A 

#define RB	0xA0 	//Б
#define RG	0xA1 	//Г
//#define _	0xA2 	//'Ё'
#define ZH	0xA3 	//Ж
#define RZ	0xA4 	//З
#define RI	0xA5    //И
#define RJ	0xA6    //Й
#define RL	0xA7 	//Л
#define RP	0xA8 	//П
#define RU	0xA9 	//У 
#define RF	0xAA  	//Ф
#define CH	0xAB  	//Ч
#define SH	0xAC  	//Ш
#define TV	0xAD   	//Ъ
#define YY	0xAE   	//Ы
#define RA	0xAF   	//Э
#define JU	0xB0   	//Ю
#define JA	0xB1   	//Я
#define rb	0xB2   	//б
#define rv	0xB3   	//в
#define rg	0xB4   	//г
#define zh	0xB6   	//ж
#define rz	0xB7   	//з
#define ri	0xB8   	//и
#define rj	0xB9   	//й
#define rk	0xBA   	//к
#define rl	0xBB   	//л
#define rm	0xBC   	//м
#define rn	0xBD   	//н
#define rp	0xBE   	//п
#define rt	0xBF   	//т
#define ch	0xC0   	//ч
#define sh	0xC1   	//ш
#define tv	0xC2   	//ъ
#define yy	0xC3   	//ы
#define mg	0xC4   	//ь
#define ra	0xC5   	//э
#define ju	0xC6   	//ю
#define ja	0xC7 	//я

#define RD	0xE0   	//Д
#define TS	0xE1   	//Ц
#define SS	0xE2   	//Щ
#define rd	0xE3   	//д
#define rf	0xE4   	//ф
#define ts	0xE5   	//ц
#define ss	0xE6   	//щ

#endif
