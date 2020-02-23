#include "p33FJ128GP706.h"

#include "system\eeprom\eeprom.h"
#include "system\menu\menu_proc.h"
#include "system\menu\menu00_main.h"
#include "system\menu\menu17_record.h"

#include "system\modbus\modbus.h"
#include "system\modbus\modbus_master.h"


//----------���������--------------


//---------����������--------------
unsigned short cur_val_m16[6];     
unsigned char addr_m16=0x01;   //����� �������� ������
unsigned short attempt_m16=0;  //������� ��������� ��������� � ������


////----------�������----------------
void menu16_out(void);
void menu16_refresh(void);
void menu16_key_pressed(void);

void menu16_2_out(void);
void menu16_2_refresh(void);
void menu16_2_key_pressed(void);
  void menu16_2_plus(void);
  void menu16_2_minus(void);

void menu16_3_out(void);
void menu16_3_refresh(void);
void menu16_3_key_pressed(void);
  void menu16_3_plus(void);
  void menu16_3_minus(void);

//--------------------------�������� �����-------------------------------------------

///////////////////////////////////
/*rom near*/const unsigned char menu_16[]=
//  '--------------------'
//  '^     �������       '
//  '      ��������      '
//  '                    '
//  'M         �������� J'


// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
// ^                 �  �  �  �  �  �  �                     
 {zu,__,__,__,__,__,_T,_E,_K,RU,SS,RI,_E,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//                   �  �  �  �  �  �  �  �
  __,__,__,__,__,__,RZ,_H,_A,CH,_E,_H,RI,JA,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//  
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
// �                             �  �  �  �  �  �  �  �     J
  _M,__,__,__,__,__,__,__,__,__,RP,xP,_O,xC,_M,_O,_T,xP,__,zj}; 

void menu16_out(void)
{
  //�������������� ���������� ����
  menu_body_out(menu_16);
  active_menu=m16;
  flags_menu.f_refresh=0;
  flags_menu.f_autochange=0;
  //�������������� ���������� menu16
  
  //������� ����������
  menu16_refresh();
  return;
}

///////////////////////////////////
void menu16_refresh(void)
{
  return;
}

///////////////////////////////////
void menu16_key_pressed(void)
{
  switch(button_status)
    {
    case (0b00001000):  //����� ������� ������
      menu17_out(); 
      break;  
    case (0b00000100):  //����� ������ ������
      menu0_out();
      break;  
    case (0b00000010):  //������ ������� ������
      break;  
    case (0b00000001):  //������ ������ ������
      menu16_2_out();
      break;  
    
    default:
      break;
  }

  return;
}

///////////////////////////////////
/*rom near*/ const unsigned char menu_16_2[]=

//  '--------------------'
//  '-       �� 01      +'
//  ' P1=13.443 P2=13.457'
//  ' P3=13.443 P4=13.457'
//  'M    P5=13.457     >'
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// -                       �  �     0  1                    +^
 {zm,__,__,__,__,__,__,__,RB,_K,__,__,__,__,__,__,__,__,__,zp,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
//    P  1  =  1  3  .  4  4  3     P  2  =  1  3  .  4  5  7 
  __,xP,_1,ze,__,__,__,__,__,__,__,xP,_2,ze,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
//    P  3  =  1  3  .  4  4  3     P  4  =  1  3  .  4  5  7
  __,xP,_3,ze,__,__,__,__,__,__,__,xP,_4,ze,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
// M              P  3  =  1  3  .  4  4  3        1  2  3  >
  _M,__,__,__,__,xP,_5,ze,__,__,__,__,__,__,__,__,__,__,__,z0};


void menu16_2_out(void)
{
  //unsigned int i;
  
  //�������������� ���������� ����
  menu_body_out(menu_16_2);
  active_menu=m16_2;
  flags_menu.f_refresh=1;
  flags_menu.f_autochange=1;
  autochange_rate=AUTOCHANGE_RATE_FAST;
  //�������������� ����������
  //for(i=0;i<NUMBER_OF_SENSORS;i++) cur_val_m16[i]=0xFFFF;  //��������, ��� �� ������ ��� �� �����������.
  //������� ����������
  menu16_2_refresh();
  return;
}

///////////////////////////////////
void menu16_2_refresh(void)
{
  unsigned char err_val[6]={__,zw,zw,zw,__,__};
  unsigned char empty[3]={__,__,__};
 
  //������� ����������
  //����� ������
  menu_variable_out(0,11,addr_m16,2);
  //��������
  if(!modbus_master_read_reg(addr_m16, REG_SENSOR1_CURRENT_VALUE, 5, cur_val_m16))
  {
    //������� ������
    menu_float_variable_out(1,4,cur_val_m16[0],2,3);
    menu_float_variable_out(1,14,cur_val_m16[1],2,3);
    menu_float_variable_out(2,04,cur_val_m16[2],2,3);
    menu_float_variable_out(2,14,cur_val_m16[3],2,3);
    menu_float_variable_out(3,8,cur_val_m16[4],2,3);
    menu_string_out(3,15,empty,3);
    attempt_m16=0;
  }
  else
  {
    //������  
    menu_string_out(1,4,err_val,6);
    menu_string_out(1,14,err_val,6);
    menu_string_out(2,4,err_val,6);
    menu_string_out(2,14,err_val,6);
    menu_string_out(3,8,err_val,6);
    menu_variable_out(3,15,attempt_m16,3);
    attempt_m16++;
    if(attempt_m16>999) attempt_m16=0;
  }
  
  return;
}

///////////////////////////////////
void menu16_2_key_pressed(void)
{
  switch(button_status)
    {
    case (0b00001000):  //����� ������� ������
      menu16_2_minus();
      break;  
    case (0b00000100):  //����� ������ ������
      menu16_out();
      break;  
    case (0b00000010):  //������ ������� ������
      menu16_2_plus();
      break;  
    case (0b00000001):  //������ ������ ������
      menu16_3_out();
      break;  
    default:
      break;
  }

  return;
}

///////////////////////////////////
void menu16_2_plus(void)
{
  addr_m16++;
  if(addr_m16==73) addr_m16=1;
  menu16_2_refresh();
  return;
}

///////////////////////////////////
void menu16_2_minus(void)
{

  addr_m16--;
  if(addr_m16==0) addr_m16=72;
  menu16_2_refresh();
  return;
}



///////////////////////////////////
/*rom near*/ const unsigned char menu_16_3[]=

//  '--------------------'
//  '-       �� 01      +'
//  ' P6=13.443 P7=13.457'
//  ' P8=13.443 P9=13.457'
//  '� �=0 R=13.457     <'
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// -                       �  �     0  1                   +
 {zm,__,__,__,__,__,__,__,RB,_K,__,__,__,__,__,__,__,__,__,zp,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
//'   P  6  =  1  3  .  4  4  3     P  7  =  1  3  .  4  5  7'
  __,xP,_6,ze,__,__,__,__,__,__,__,xP,_7,ze,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
//'   P  8  =  1  3  .  4  4  3     P  9  =  1  3  .  4  5  7'
  __,xP,_8,ze,__,__,__,__,__,__,__,xP,_9,ze,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
// M     �  =  1     R  =  1  3  .  4  4  3                 <
  _M,__,_B,ze,__,__,_R,ze,__,__,__,__,__,__,__,__,__,__,__,z1};


void menu16_3_out(void)
{
  //unsigned int i;
  
  //�������������� ���������� ����
  menu_body_out(menu_16_3);
  active_menu=m16_3;
  flags_menu.f_refresh=1;
  flags_menu.f_autochange=1;
  autochange_rate=AUTOCHANGE_RATE_FAST;
  //�������������� ����������
  //for(i=0;i<NUMBER_OF_SENSORS;i++) cur_val_m16[i]=0xFFFF;  //��������, ��� �� ������ ��� �� �����������.
  //������� ����������
  menu16_3_refresh();
  return;
}

///////////////////////////////////
void menu16_3_refresh(void)
{
  unsigned char err_val[6]={__,zw,zw,zw,__,__};
  unsigned char empty[3]={__,__,__};
  unsigned char err_val2[1]={zw};
 
  //������� ����������
  //����� ������
  menu_variable_out(0,11,addr_m16,2);
  //��������
  if(!modbus_master_read_reg(addr_m16, REG_SENSOR6_CURRENT_VALUE, 6, cur_val_m16))
  {
    //������� ������
    menu_float_variable_out(1,4,cur_val_m16[0],2,3);
    menu_float_variable_out(1,14,cur_val_m16[1],2,3);
    menu_float_variable_out(2,04,cur_val_m16[2],2,3);
    menu_float_variable_out(2,14,cur_val_m16[3],2,3);
    menu_float_variable_out(3,8,cur_val_m16[4],2,3);
    menu_variable_out(3,4,cur_val_m16[5],1);
    menu_string_out(3,15,empty,3);
    attempt_m16=0;
  }
  else
  {
    //������  
    menu_string_out(1,4,err_val,6);
    menu_string_out(1,14,err_val,6);
    menu_string_out(2,4,err_val,6);
    menu_string_out(2,14,err_val,6);
    menu_string_out(3,8,err_val,6);
    menu_string_out(3,4,err_val2,1);
    menu_variable_out(3,15,attempt_m16,3);
    attempt_m16++;
    if(attempt_m16>999) attempt_m16=0;
  }
  return;
}

///////////////////////////////////
void menu16_3_key_pressed(void)
{
  switch(button_status)
    {
    case (0b00001000):  //����� ������� ������
      menu16_3_minus();
      break;  
    case (0b00000100):  //����� ������ ������
      menu16_out();
      break;  
    case (0b00000010):  //������ ������� ������
      menu16_3_plus();
      break;  
    case (0b00000001):  //������ ������ ������
      menu16_2_out();
      break;  
    default:
      break;
  }

  return;
}


///////////////////////////////////
void menu16_3_plus(void)
{
  addr_m16++;
  if(addr_m16==73) addr_m16=1;
  //menu16_3_intervals_calculate();
  menu16_3_refresh();
  return;
}

///////////////////////////////////
void menu16_3_minus(void)
{

  addr_m16--;
  if(addr_m16==0) addr_m16=72;
  //menu16_3_intervals_calculate();
  menu16_3_refresh();
  return;
}

