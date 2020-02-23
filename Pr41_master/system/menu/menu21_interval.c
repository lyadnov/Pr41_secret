#include "p33FJ128GP706.h"

#include "system\eeprom\eeprom.h"

#include "system\menu\menu_proc.h"
#include "system\menu\menu00_main.h"
#include "system\menu\menu18_calibration.h"
#include "system\menu\menu20_addr.h"
#include "system\menu\menu22_pause.h"

#include "system\modbus\modbus.h"
#include "system\modbus\modbus_master.h"

//----------���������--------------

//---------����������--------------
unsigned char  addr_m21=0x01;
unsigned short interval_m21=0x01;

unsigned short temp_interval_m21=0;
unsigned char  change_flag_m21=0;
unsigned char  return_main_menu_flag_m21=0;

//��� �������
cursor_struct cursor_m21;


////----------�������----------------
void menu21_out(void);
void menu21_refresh(void);
void menu21_key_pressed(void);

void menu21_2_out(void);
void menu21_2_refresh(void);
void menu21_2_key_pressed(void);
  void menu21_2_select(void);
  void menu21_2_plus(void);
  void menu21_2_minus(void);
  void menu21_2_save(void);
  
void menu21_3_out(void);
void menu21_3_refresh(void);
void menu21_3_key_pressed(void);
  void menu21_3_yes(void);
  void menu21_3_yes_for_all(void);


//    void menu21_2_yes(void);

//--------------------------�������� �����-------------------------------------------
///////////////////////////////////
/*rom near*/const unsigned char menu_21[]=
//  '^  �������� �����  v'
//  '    �����������     '
//  '                    '
//  'M         �������� J'

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
// ^        �  �  �  �  �  �  �  �     �  �  �  �  �       v'
 {zu,__,__,RI,_H,_T,_E,xP,_B,_A,RL,__,_M,_E,ZH,RD,RU,__,__,zd,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//             �  �  �  �  �  �  �  �  �  �  � 
  __,__,__,__,RI,RZ,_M,_E,xP,_E,_H,RI,JA,_M,RI,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//  
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
// �                             �  �  �  �  �  �  �  �     j
  _M,__,__,__,__,__,__,__,__,__,RI,RZ,_M,_E,_H,RI,_T,_b,__,zj}; 

void menu21_out(void)
{
  //�������������� ���������� ����
  menu_body_out(menu_21);
  active_menu=m21;
  flags_menu.f_refresh=0;
  flags_menu.f_autochange=0;
  //�������������� ���������� menu21
  
  //������� ����������
  menu21_refresh();
  return;
}

///////////////////////////////////
void menu21_refresh(void)
{
  return;
}

///////////////////////////////////
void menu21_key_pressed(void)
{
  switch(button_status)
    {
    case (0b00001000):  //����� ������� ������
      menu22_out();
      break;  
    case (0b00000100):  //����� ������ ������
      menu0_out();
      break;  
    case (0b00000010):  //������ ������� ������
      menu20_out();
      break;  
    case (0b00000001):  //������ ������ ������
      menu21_2_out();
      break;  
    
    default:
      break;
  }

  return;
}



///////////////////////////////////
/*rom near*/ const unsigned char menu_21_2[]=
//  '-      ��  01      +'
//  '                    '
//  '   ����� ����� 02   '
//  'M                  >'

//  '--------------------'
//  '-    ��     01     +'
//  '                    '
//  '  ��������  100 ��  '
//  'M                  >'

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// -              �  �                 0  1                 +
 {zm,__,__,__,__,RB,_K,__,__,__,__,__,__,__,__,__,__,__,__,zp,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//     
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//       �  �  �  �  �  �  �  �        1  0  0     �  �
  __,__,RI,_H,_T,_E,xP,_B,_A,RL,__,__,__,__,__,__,rm,_c,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// M                                                        >
  _M,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,z0}; 


void menu21_2_out(void)
{
  //�������������� ���������� ����
  menu_body_out(menu_21_2);
  active_menu=m21_2;
  flags_menu.f_refresh=1;
  flags_menu.f_autochange=1;
  autochange_rate=AUTOCHANGE_RATE_FAST;
  //������
  cursor_m21.position=0;
  cursor_m21.count=0;
  cursor_m21.flags.byte=0;
  //������� ����������
  menu21_2_refresh();
  return;
}


///////////////////////////////////
void menu21_2_refresh(void)
{
  unsigned char cursor_string[3]={zb,zb,zb};
  unsigned char err_val[3]={zw,zw,zw};
  
  //������� ����������
  //����� ������
  menu_variable_out(0,12,addr_m21,2);
  //��������
   if(!modbus_master_read_reg(addr_m21,REG_CONFIG_MEASURE_INTERVAL,1, &interval_m21))
  {
    //������� ������
    menu_variable_out(2,12,interval_m21,3);
  }
  else
  {
    //������  
    menu_string_out(2,12,err_val,3);
  }
  
  //������� ������
  cursor_m21.count++;
  if(cursor_m21.flags.f_cursor_on==1)
  {
    //������ ������ ����������
    if(cursor_m21.count>=CURSOR_ON_TIME)
    {
      cursor_m21.count=0;
      cursor_m21.flags.f_cursor_on=0;
    }
    switch(cursor_m21.position)
      {
      case (0):
        menu_string_out(0,12,cursor_string,2);
        break;
      case (1):
        menu_string_out(2,12,cursor_string,3);
        break;
    }
  }
  else
  {
    //������� �� ������ ����������
    if(cursor_m21.count>=CURSOR_OFF_TIME)
    {
      cursor_m21.count=0;
      cursor_m21.flags.f_cursor_on=1;
    }

  }
  
  return;
}

///////////////////////////////////
void menu21_2_key_pressed(void)
{
  switch(button_status)
  {
    case (0b00001000):  //����� ������� ������
      menu21_2_minus();
      break;  
    case (0b00000100):  //����� ������ ������
      menu21_2_save();
      break;  
    case (0b00000010):  //������ ������� ������
      menu21_2_plus();
      break;  
    case (0b00000001):  //������ ������ ������
      menu21_2_select();
      break;  
    default:
      break;
  }

  return;
}

///////////////////////////////////
void menu21_2_select(void)
{
  if((cursor_m21.position==1)&&(change_flag_m21==1) && (temp_interval_m21!=interval_m21) )
  {
    return_main_menu_flag_m21=0;
    menu21_3_out();
  }
    
  cursor_m21.position++;
  if(cursor_m21.position==2) cursor_m21.position=0;
  cursor_m21.count=0;
  cursor_m21.flags.f_cursor_on=1;
  return;
}

///////////////////////////////////
void menu21_2_plus(void)
{
  switch(cursor_m21.position)
  {
    case (0):   //����� ������
      addr_m21++;
      if(addr_m21==73) addr_m21=1;
      break;
    case (1):   //�������� ����� �����������
      if(change_flag_m21==0)
      {
        change_flag_m21=1;
        temp_interval_m21=interval_m21;
      }
      interval_m21++;
      if(interval_m21>100) interval_m21=1;
      
      modbus_master_write_reg(addr_m21,REG_CONFIG_MEASURE_INTERVAL,interval_m21);
      break;
  }
  cursor_m21.count=0;
  cursor_m21.flags.f_cursor_on=0;
  menu21_refresh();
  return;
}

///////////////////////////////////
void menu21_2_minus(void)
{
  switch(cursor_m21.position)
  {
    case (0):   //����� ������
      addr_m21--;
      if(addr_m21==0) addr_m21=72;
      break;
    case (1):   //�������� ����� �����������
      if(change_flag_m21==0)
      {
        change_flag_m21=1;
        temp_interval_m21=interval_m21;
      }

      interval_m21--;
      if(interval_m21<1) interval_m21=100;
        
      modbus_master_write_reg(addr_m21,REG_CONFIG_MEASURE_INTERVAL,interval_m21);
      break;
  }
  
  cursor_m21.count=0;
  cursor_m21.flags.f_cursor_on=0;
  menu21_refresh();
  return;
}


///////////////////////////////////
void menu21_2_save(void)
{
  //��������� ���������� �� ������
  if( (cursor_m21.position==1)&&(change_flag_m21==1) )
  {
    if(temp_interval_m21!=interval_m21)
    {
      return_main_menu_flag_m21=1;
      menu21_3_out();
    }
    else
    {
      change_flag_m21=0;
      menu21_out();
    }
  }
  else menu21_out();
  
  return;
}


/////////////////////////////////////////////////////////////////
//
///////////////////////////////////
/*rom near*/ const unsigned char menu_21_3[]=
//	'--------------------'
//  '   xx ���������     ' 
//  '   xx ���������?    ' 
//	'                    '   
//  '���               ��'

//	'--------------------'
//  '            ��� ����'
//  '   xx  ���������     ' 
//	'   xx  ���������?    '
//  '���               ��'


// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//                                     �  �  �     �  �  �  �
 {__,__,__,__,__,__,__,__,__,__,__,__,RD,RL,JA,__,_B,xC,_E,_X,

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
//                      c  �  �  �  �  �  �  �  � 
  __,__,__,__,__,__,__,_c,_o,_x,_p,_a,rn,ri,rt,mg,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//                      �  �  �  �  �  �  �  �  �  ?                                                                                         
  __,__,__,__,__,__,__,ri,rz,rm,_e,rn,_e,rn,ri,ja,zw,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
// �  �  �                                               �  �                                                                           
  _H,_E,_T,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,RD,_A}; 

const unsigned char menu_21_3_pic[]=
{
    0xFE, 0x02, 0xFE, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0xFE, 0x0A, 0xFE, 0x3F, 0x40, 
    0x40, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x45, 0x45, 0x7C, 0x40, 0x7F, 
};

void menu21_3_out(void)
{
  //�������������� ���������� ����
  menu_body_out(menu_21_3);
  menu_picture_out((unsigned char*) menu_21_3_pic,24,8,14,16);
  
  active_menu=m21_3;
  flags_menu.f_refresh=0;
  flags_menu.f_autochange=0;
  //�������������� ���������� menu
  //������� ����������
  menu21_3_refresh();
    return;
}

///////////////////////////////////
void menu21_3_refresh(void)
{
  return;
}

///////////////////////////////////
void menu21_3_key_pressed(void)
{
  switch(button_status)
    {
    case (0b00001000):  //����� ������� ������
      break;  
    case (0b00000100):  //����� ������ ������
      //������
      modbus_master_write_reg(addr_m21,REG_CONFIG_MEASURE_INTERVAL,temp_interval_m21);
      change_flag_m21=0;
      if(return_main_menu_flag_m21==1)
      {
        menu21_out();
      }
      else
      {
        menu21_2_out();  
      }
      break;  
    case (0b00000010):  //������ ������� ������
      menu21_3_yes_for_all();
      break;  
    case (0b00000001):  //������ ������ ������
      menu21_3_yes();
      break;  
    default:
      break;
  }
  return;
}

///////////////////////////////////
void menu21_3_yes(void)
{
  modbus_master_write_reg(addr_m21,REG_CONTROL_SAVE,CMD_SAVE_MEASURE_INTERVAL);
  change_flag_m21=0;
  if(return_main_menu_flag_m21==1)
  {
    menu21_out();
  }
  else
  {
    menu21_2_out();  
  }
  return;
}


///////////////////////////////////
void menu21_3_yes_for_all(void)
{
  modbus_master_write_reg_broadcast(REG_CONFIG_MEASURE_INTERVAL,interval_m21);
  modbus_master_write_reg_broadcast(REG_CONTROL_SAVE,CMD_SAVE_MEASURE_INTERVAL);
  change_flag_m21=0;
  if(return_main_menu_flag_m21==1)
  {
    menu21_out();
  }
  else
  {
    menu21_2_out();  
  }
  return;
}


