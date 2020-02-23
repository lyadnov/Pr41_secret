#include "p33FJ128GP706.h"

#include "system\eeprom\eeprom.h"
#include "system\menu\menu_proc.h"
#include "system\menu\menu00_main.h"
#include "system\menu\menu16_current.h"

#include "system\modbus\modbus.h"
#include "system\modbus\modbus_master.h"


//----------���������--------------

//---------����������--------------
cursor_struct cursor_m17;

unsigned char slave_m17=0x01;
unsigned char sensor_m17=0;
unsigned char index_m17=0;
unsigned short val_m17=0;
unsigned short start_N_m17=0;

////----------�������----------------
void menu17_out(void);
void menu17_refresh(void);
void menu17_key_pressed(void);

void menu17_2_out(void);
void menu17_2_refresh(void);
void menu17_2_key_pressed(void);
  void menu17_2_select(void);
  void menu17_2_plus(void);
  void menu17_2_minus(void);


//--------------------------�������� �����-------------------------------------------
///////////////////////////////////
/*rom near*/const unsigned char menu_17[]=
//  '--------------------'
//  '     ����������    v'
//  '      ��������      '
//  '                    '
//  'M         �������� j'



// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//                �  �  �  �  �  �  �  �  �  �              v
 {__,__,__,__,__,RZ,_A,RP,RI,xC,_A,_H,_H,YY,_E,__,__,__,__,zd,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//                   �  �  �  �  �  �  �  �
  __,__,__,__,__,__,RZ,_H,_A,CH,_E,_H,RI,JA,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//  
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
// �                             �  �  �  �  �  �  �  �     J
  _M,__,__,__,__,__,__,__,__,__,RP,xP,_O,xC,_M,_O,_T,xP,__,zj}; 

void menu17_out(void)
{
  //�������������� ���������� ����
  menu_body_out(menu_17);
  active_menu=m17;
  flags_menu.f_refresh=0;
  flags_menu.f_autochange=0;
  //�������������� ���������� menu17
  
  //������� ����������
  menu17_refresh();
  return;
}

///////////////////////////////////
void menu17_refresh(void)
{
  return;
}

///////////////////////////////////
void menu17_key_pressed(void)
{
  switch(button_status)
    {
    case (0b00001000):  //����� ������� ������
      break;  
    case (0b00000100):  //����� ������ ������
      menu0_out();
      break;  
    case (0b00000010):  //������ ������� ������
      menu16_out(); 
      break;  
    case (0b00000001):  //������ ������ ������
      menu17_2_out();
      break;  
    
    default:
      break;
  }

  return;
}



///////////////////////////////////
/*rom near*/ const unsigned char menu_17_2[]=

//  '--------------------'
//  '-   ��      01     +'
//  '    ������  P1      '
//  '    [010] = 13.443  '
//  'M                  >'

//  '--------------------'
//  '-   ��      01     +'
//  '    ������  P1      '
//  '    [010] = 13.443  '
//  'M   ������  12345  >'
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
// -           �  �                    0  1                 +
 {zm,__,__,__,RB,_K,__,__,__,__,__,__,__,__,__,__,__,__,__,zp,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
//             �  �  �  �  �  �        P  1         
  __,__,__,__,RD,_A,_T,CH,RI,_K,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
//             [  0  1  0  ]     =     1  3  .  4  4  3  
  __,__,__,__,z2,__,__,__,z3,__,ze,__,__,__,__,__,__,__,__,__, 
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
// -           �  �  �  �  �  �        1  2  3  4  5        +
  _M,__,__,__,RZ,_A,RP,RU,xC,_K,__,__,__,__,__,__,__,__,__,z0}; 



void menu17_2_out(void)
{
  //�������������� ���������� ����
  menu_body_out(menu_17_2);
  active_menu=m17_2;
  flags_menu.f_refresh=1;
  flags_menu.f_autochange=1;
  autochange_rate=AUTOCHANGE_RATE_FAST;
  //������
  cursor_m17.position=0;
  cursor_m17.count=0;
  cursor_m17.flags.byte=0;
  //������� ����������
  menu17_2_refresh();
	return;
}


///////////////////////////////////
void menu17_2_refresh(void)
{
  unsigned char cursor_string[3]={zb,zb,zb};
  unsigned char err_val[6]={zw,zw,zw,__,__,__};

  unsigned char sensor0[2] ={xP,_1};
  unsigned char sensor1[2] ={xP,_2};
  unsigned char sensor2[2] ={xP,_3};
  unsigned char sensor3[2] ={xP,_4};
  unsigned char sensor4[2] ={xP,_5};
  unsigned char sensor5[2] ={xP,_6};
  unsigned char sensor6[2] ={xP,_7};
  unsigned char sensor7[2] ={xP,_8};
  unsigned char sensor8[2] ={xP,_9};
  //unsigned char sensor9[2] ={_R,__};
  
  //������� ����������
  //����� ������
  menu_variable_out(0,12,slave_m17,2);
  //������
  switch(sensor_m17)
  {
    case 0:
      menu_string_out(1,12,sensor0,2);
      break;
    case 1:
      menu_string_out(1,12,sensor1,2);
      break;
    case 2:
      menu_string_out(1,12,sensor2,2);
      break;
    case 3:
      menu_string_out(1,12,sensor3,2);
      break;
    case 4:
      menu_string_out(1,12,sensor4,2);
      break;
    case 5:
      menu_string_out(1,12,sensor5,2);
      break;
    case 6:
      menu_string_out(1,12,sensor6,2);
      break;
    case 7:
      menu_string_out(1,12,sensor7,2);
      break;
    case 8:
      menu_string_out(1,12,sensor8,2);
      break;
    //case 9:
     // menu_string_out(1,12,sensor9,2);
     // break;
    default:
      break;
  }
  //������
  menu_variable_out(2,5,index_m17,3);
  //�������� �������    
  if(!modbus_master_read_reg(slave_m17,REG_SENSOR1_RECORD_VALUE+sensor_m17*NUMBER_OF_MEASUREMENTS+index_m17,1, &val_m17))
  {
    //������� ������
    menu_float_variable_out(2,12,val_m17,2,3);
    //attempt_m16=0;
  }
  else
  {
    //������  
    menu_string_out(2,12,err_val,6);
    //menu_variable_out(3,15,attempt_m16,3);
    //attempt_m16++;
    //if(attempt_m16>999) attempt_m16=0;
  }

  //����� �������
  if(!modbus_master_read_reg(slave_m17,REG_STAT_FULL_DUMP_NUM,1, &start_N_m17))
  {
    //������� ������
    menu_variable_out(3,12,start_N_m17,5);
  }
  else
  {
    //������  
    menu_string_out(3,12,err_val,5);
  }
  


    //������� ������
  cursor_m17.count++;
  if(cursor_m17.flags.f_cursor_on==1)
  {
    //������ ������ ���������
    if(cursor_m17.count>=CURSOR_ON_TIME)
    {
      cursor_m17.count=0;
      cursor_m17.flags.f_cursor_on=0;
    }
    switch(cursor_m17.position)
    {
      case (0):
        menu_string_out(0,12,cursor_string,2);
        break;
      case (1):
        menu_string_out(1,12,cursor_string,2);
        break;
      case (2):
        menu_string_out(2,5,cursor_string,3);
        break;
    }
  }
  else
  {
    //������� �� ������ ���������
    if(cursor_m17.count>=CURSOR_OFF_TIME)
    {
      cursor_m17.count=0;
      cursor_m17.flags.f_cursor_on=1;
    }

  }
  
  return;
}

///////////////////////////////////
void menu17_2_key_pressed(void)
{
  switch(button_status)
  {
    case (0b00001000):  //����� ������� ������
      menu17_2_minus();
      break;  
    case (0b00000100):  //����� ������ ������
      menu17_out();
      break;  
    case (0b00000010):  //������ ������� ������
      menu17_2_plus();
      break;  
    case (0b00000001):  //������ ������ ������
      menu17_2_select();
      break;  
    default:
      break;
  }

  return;
}

///////////////////////////////////
void menu17_2_select(void)
{
  cursor_m17.position++;
  if(cursor_m17.position==3) cursor_m17.position=0;
  cursor_m17.count=0;
  cursor_m17.flags.f_cursor_on=1;
  return;
}

///////////////////////////////////
void menu17_2_plus(void)
{
  switch(cursor_m17.position)
  {
    case (0):   //����� ������
      slave_m17++;
      if(slave_m17>72) slave_m17=1;
      break;
    case (1):   //����� �������
      sensor_m17++;
      if(sensor_m17>(NUMBER_OF_SENSORS-2)) sensor_m17=0;
      break;
    case (2):   //������
      index_m17++;
      if(index_m17==NUMBER_OF_MEASUREMENTS) index_m17=0;
      break;
  }
  cursor_m17.count=0;
  cursor_m17.flags.f_cursor_on=0;
  menu17_2_refresh();
  return;
}

///////////////////////////////////
void menu17_2_minus(void)
{
  switch(cursor_m17.position)
  {
    case (0):   //����� ������
      slave_m17--;
      if(slave_m17==0) slave_m17=72;
      break;
    case (1):   //����� �������
      sensor_m17--;
      if(sensor_m17==255) sensor_m17=NUMBER_OF_SENSORS-2;
      break;
    case (2):   //������
      index_m17--;
      if(index_m17==255) index_m17=NUMBER_OF_MEASUREMENTS-1;
      break;
  }
  cursor_m17.count=0;
  cursor_m17.flags.f_cursor_on=0;
  menu17_2_refresh();
  return;
}






