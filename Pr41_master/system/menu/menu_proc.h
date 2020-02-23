#ifndef _MENU_PROC_INCLUDED
#define _MENU_PROC_INCLUDED

////----------���������--------------
//typedef struct
//{
//    unsigned char xlen;     //������ ������� � �������� �� �����������
//    unsigned char ylen;     //������ ������� � �������� �� ��������
//    unsigned char* data;    //��������� �� ����� � �������      
//} picture_struct;

//----------���������--------------
#define BUTTON_ANTINOISE_PAUSE  5/*10*/ //����� ����� ������� ����� ��������� ����������

#define AUTOCHANGE_START_PAUSE  100 //��������� ����� ����� ������� �������� �������������� ������� ������

#define AUTOCHANGE_RATE_FAST    5   //�������� �������������� ��� ������� � ��������� ������
#define AUTOCHANGE_RATE_MEDIUM  10
#define AUTOCHANGE_RATE_SLOW    20

#define REFRASH_RATE            5 //10  //�������� ���������� ������� 5*13��=65��

#define CURSOR_ON_TIME          3
#define CURSOR_OFF_TIME         6


//---------����������--------------
typedef union 
{
	struct 
	{
		unsigned char f_refresh 	    :1; //��������� �������������� ������� ��� ���
		unsigned char f_autochange      :1; //��������� ��������� ������ � ������ ���� ��� ���
	}; 
	unsigned char byte;
} flags_menu_struct;
extern flags_menu_struct flags_menu;

extern unsigned char active_menu;
  //�������� ���������� active_menu
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

//��� �������
typedef union 
{
	struct                                  //���������� ��������
	{
		unsigned char f_cursor_on        :1; 
	}; 
	unsigned char byte;
} flags_cursor_struct;

typedef struct  
{
	flags_cursor_struct flags;
	unsigned char position;  //������� ������� �������
	unsigned char count;     //�������, ��������� ��������� �������    
} cursor_struct;


extern unsigned char button_status;
extern unsigned char autochange_rate;
//----------�������----------------
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
#define za  0x5B    //'[' ���� �� �������� :)
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

#define RB	0xA0 	//�
#define RG	0xA1 	//�
//#define _	0xA2 	//'�'
#define ZH	0xA3 	//�
#define RZ	0xA4 	//�
#define RI	0xA5    //�
#define RJ	0xA6    //�
#define RL	0xA7 	//�
#define RP	0xA8 	//�
#define RU	0xA9 	//� 
#define RF	0xAA  	//�
#define CH	0xAB  	//�
#define SH	0xAC  	//�
#define TV	0xAD   	//�
#define YY	0xAE   	//�
#define RA	0xAF   	//�
#define JU	0xB0   	//�
#define JA	0xB1   	//�
#define rb	0xB2   	//�
#define rv	0xB3   	//�
#define rg	0xB4   	//�
#define zh	0xB6   	//�
#define rz	0xB7   	//�
#define ri	0xB8   	//�
#define rj	0xB9   	//�
#define rk	0xBA   	//�
#define rl	0xBB   	//�
#define rm	0xBC   	//�
#define rn	0xBD   	//�
#define rp	0xBE   	//�
#define rt	0xBF   	//�
#define ch	0xC0   	//�
#define sh	0xC1   	//�
#define tv	0xC2   	//�
#define yy	0xC3   	//�
#define mg	0xC4   	//�
#define ra	0xC5   	//�
#define ju	0xC6   	//�
#define ja	0xC7 	//�

#define RD	0xE0   	//�
#define TS	0xE1   	//�
#define SS	0xE2   	//�
#define rd	0xE3   	//�
#define rf	0xE4   	//�
#define ts	0xE5   	//�
#define ss	0xE6   	//�

#endif
