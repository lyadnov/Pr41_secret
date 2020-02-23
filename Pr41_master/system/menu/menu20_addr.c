#include "p33FJ128GP706.h"

#include "system\eeprom\eeprom.h"

#include "system\menu\menu_proc.h"
#include "system\menu\menu00_main.h"
#include "system\menu\menu18_calibration.h"
#include "system\menu\menu21_interval.h"

#include "system\modbus\modbus.h"
#include "system\modbus\modbus_master.h"

//----------константы--------------


//---------переменные--------------
unsigned char  addr_m20=0x01;
unsigned short newaddr_m20=0x01;

unsigned short temp_newaddr_m20=0;
unsigned char  change_flag_m20=0;
unsigned char  return_main_menu_flag_m20=0;

//для курсора
cursor_struct cursor_m20;


////----------функции----------------
void menu20_out(void);
void menu20_refresh(void);
void menu20_key_pressed(void);

void menu20_2_out(void);
void menu20_2_refresh(void);
void menu20_2_key_pressed(void);
  void menu20_2_select(void);
  void menu20_2_plus(void);
  void menu20_2_minus(void);
  void menu20_2_save(void);
  
void menu20_3_out(void);
void menu20_3_refresh(void);
void menu20_3_key_pressed(void);
  void menu20_3_yes(void);



//    void menu20_2_yes(void);

//--------------------------основная часть-------------------------------------------
///////////////////////////////////
/*rom near*/const unsigned char menu_20[]=
//  '^     АДРЕС БК     v'
//  '                    '
//  '                    '
//  'M        ИЗМЕНИИТЬ J'

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//^                  А  Д  Р  Е  С     Б  К                 v
 {zu,__,__,__,__,__,_A,RD,xP,_E,xC,__,RB,_K,__,__,__,__,__,zd,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//  
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//  
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
// М                             И  З  М  Е  Н  И  Т  Ь     j
  _M,__,__,__,__,__,__,__,__,__,RI,RZ,_M,_E,_H,RI,_T,_b,__,zj}; 

void menu20_out(void)
{
  //инициаллизация параметров меню
  menu_body_out(menu_20);
  active_menu=m20;
  flags_menu.f_refresh=0;
  flags_menu.f_autochange=0;
  //инициаллизация переменных menu20
  
  //выводим переменные
  menu20_refresh();
  return;
}

///////////////////////////////////
void menu20_refresh(void)
{
  return;
}

///////////////////////////////////
void menu20_key_pressed(void)
{
  switch(button_status)
    {
    case (0b00001000):  //левая верхняя кнопка
      menu21_out();
      break;  
    case (0b00000100):  //левая нижняя кнопка
      menu0_out();
      break;  
    case (0b00000010):  //правая верхняя кнопка
      menu18_out();
      break;  
    case (0b00000001):  //правая нижняя кнопка
      menu20_2_out();
      break;  
    
    default:
      break;
  }

  return;
}



///////////////////////////////////
/*rom near*/ const unsigned char menu_20_2[]=
//  '-      БК  01      +'
//  '                    '
//  '   НОВЫЙ АДРЕС 02   '
//  'M                  >'

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// -                    Б  К        0  1                    +
 {zm,__,__,__,__,__,__,RB,_K,__,__,__,__,__,__,__,__,__,__,zp,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//     
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//          Н  О  В  Ы  Й     А  Д  Р  Е  С     0  2 
  __,__,__,_H,_O,_B,YY,RJ,__,_A,RD,xP,_E,xC,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// M                                                        >
  _M,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,z0}; 


void menu20_2_out(void)
{
  //инициаллизация параметров меню
  menu_body_out(menu_20_2);
  active_menu=m20_2;
  flags_menu.f_refresh=1;
  flags_menu.f_autochange=1;
  autochange_rate=AUTOCHANGE_RATE_FAST;
  //курсор
  cursor_m20.position=0;
  cursor_m20.count=0;
  cursor_m20.flags.byte=0;
  //выводим переменные
  menu20_2_refresh();
  return;
}


///////////////////////////////////
void menu20_2_refresh(void)
{
  unsigned char cursor_string[2]={zb,zb};
  unsigned char err_val[2]={zw,zw};
  
  //выводим переменные
  //адрес слейва
  menu_variable_out(0,11,addr_m20,2);
  //новый адрес слейва
  if(!modbus_master_read_reg(addr_m20,REG_CONFIG_SLAVE_ADDR,1, &newaddr_m20))
  {
    //приняли данные
    menu_variable_out(2,15,newaddr_m20,2);
  }
  else
  {
    //ошибка  
    menu_string_out(2,15,err_val,2);
  }
  
  //выводим курсор
  cursor_m20.count++;
  if(cursor_m20.flags.f_cursor_on==1)
  {
    //курсор должен выводиться
    if(cursor_m20.count>=CURSOR_ON_TIME)
    {
      cursor_m20.count=0;
      cursor_m20.flags.f_cursor_on=0;
    }
    switch(cursor_m20.position)
      {
      case (0):
        menu_string_out(0,11,cursor_string,2);
        break;
      case (1):
        menu_string_out(2,15,cursor_string,2);
        break;
    }
  }
  else
  {
    //курсвор не должен выводиться
    if(cursor_m20.count>=CURSOR_OFF_TIME)
    {
      cursor_m20.count=0;
      cursor_m20.flags.f_cursor_on=1;
    }

  }
  
  return;
}

///////////////////////////////////
void menu20_2_key_pressed(void)
{
  switch(button_status)
  {
    case (0b00001000):  //левая верхняя кнопка
      menu20_2_minus();
      break;  
    case (0b00000100):  //левая нижняя кнопка
      menu20_2_save();
      break;  
    case (0b00000010):  //правая верхняя кнопка
      menu20_2_plus();
      break;  
    case (0b00000001):  //правая нижняя кнопка
      menu20_2_select();
      break;  
    default:
      break;
  }

  return;
}

///////////////////////////////////
void menu20_2_select(void)
{
  if((cursor_m20.position==1)&&(change_flag_m20==1) && (temp_newaddr_m20!=newaddr_m20) )
  {
    return_main_menu_flag_m20=0;
    menu20_3_out();
  }
    
  cursor_m20.position++;
  if(cursor_m20.position==2) cursor_m20.position=0;
  cursor_m20.count=0;
  cursor_m20.flags.f_cursor_on=1;
  return;
}

///////////////////////////////////
void menu20_2_plus(void)
{
  switch(cursor_m20.position)
  {
    case (0):   //адрес слейва
      addr_m20++;
      if(addr_m20==73) addr_m20=1;
      break;
    case (1):   //новый адрес слейва
      if(change_flag_m20==0)
      {
        change_flag_m20=1;
        temp_newaddr_m20=newaddr_m20;
      }
      newaddr_m20++;
      if(newaddr_m20>72) newaddr_m20=1;
      
      modbus_master_write_reg(addr_m20,REG_CONFIG_SLAVE_ADDR,newaddr_m20);
      break;
  }
  cursor_m20.count=0;
  cursor_m20.flags.f_cursor_on=0;
  menu20_refresh();
  return;
}

///////////////////////////////////
void menu20_2_minus(void)
{
  switch(cursor_m20.position)
  {
    case (0):   //адрес слейва
      addr_m20--;
      if(addr_m20==0) addr_m20=72;
      break;
    case (1):   //новый адрес слейва
      if(change_flag_m20==0)
      {
        change_flag_m20=1;
        temp_newaddr_m20=newaddr_m20;
      }

      newaddr_m20--;
      if(newaddr_m20<1) newaddr_m20=72;
        
      modbus_master_write_reg(addr_m20,REG_CONFIG_SLAVE_ADDR,newaddr_m20);
      break;
  }
  
  cursor_m20.count=0;
  cursor_m20.flags.f_cursor_on=0;
  menu20_refresh();
  return;
}


///////////////////////////////////
void menu20_2_save(void)
{
  //проверяем изменились ли данные
  if( (cursor_m20.position==1)&&(change_flag_m20==1) )
  {
    if(temp_newaddr_m20!=newaddr_m20)
    {
      return_main_menu_flag_m20=1;
      menu20_3_out();
    }
    else
    {
      change_flag_m20=0;
      menu20_out();
    }
  }
  else menu20_out();
  
  return;
}


/////////////////////////////////////////////////////////////////
//
///////////////////////////////////
/*rom near*/ const unsigned char menu_20_3[]=
//	'--------------------'
//  '   xx СОХРАНИТЬ     ' 
//  '   xx ИЗМЕНЕНИЯ?    ' 
//	'                    '   
//  'НЕТ               ДА'

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
//                      С  О  Х  Р  А  Н  И  Т  Ь 
 {__,__,__,__,__,__,__,xC,_O,_X,xP,_A,_H,RI,_T,_b,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//                      И  З  М  Е  Н  Е  Н  И  Я  ?                                                                                         
  __,__,__,__,__,__,__,RI,RZ,_M,_E,_H,_E,_H,RI,JA,zw,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//  
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
// Н  Е  Т                                               Д  А                                                                           
  _H,_E,_T,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,RD,_A}; 

const unsigned char menu_20_3_pic[]=
{
    0xFE, 0x02, 0xFE, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0xFE, 0x0A, 0xFE, 0x3F, 0x40, 
    0x40, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x45, 0x45, 0x7C, 0x40, 0x7F, 
};

void menu20_3_out(void)
{
  //инициаллизация параметров меню
  menu_body_out(menu_20_3);
  menu_picture_out((unsigned char*) menu_20_3_pic,24,0,14,16);
  
  active_menu=m20_3;
  flags_menu.f_refresh=0;
  flags_menu.f_autochange=0;
  //инициаллизация переменных menu
  //выводим переменные
  menu20_3_refresh();
    return;
}

///////////////////////////////////
void menu20_3_refresh(void)
{
  return;
}

///////////////////////////////////
void menu20_3_key_pressed(void)
{
  switch(button_status)
    {
    case (0b00001000):  //левая верхняя кнопка
      break;  
    case (0b00000100):  //левая нижняя кнопка
      //отмена
      modbus_master_write_reg(addr_m20,REG_CONFIG_SLAVE_ADDR,temp_newaddr_m20);
      change_flag_m20=0;
      if(return_main_menu_flag_m20==1)
      {
        menu20_out();
      }
      else
      {
        menu20_2_out();  
      }
      break;  
    case (0b00000010):  //правая верхняя кнопка
      break;  
    case (0b00000001):  //правая нижняя кнопка
      menu20_3_yes();
      break;  
    default:
      break;
  }
  return;
}

///////////////////////////////////
void menu20_3_yes(void)
{
  modbus_master_write_reg(addr_m20,REG_CONTROL_SAVE,CMD_SAVE_SLAVE_ADDR);
  change_flag_m20=0;
  if(return_main_menu_flag_m20==1)
  {
    menu20_out();
  }
  else
  {
    menu20_2_out();  
  }
  return;
}



