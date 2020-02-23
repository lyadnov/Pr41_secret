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
unsigned char  addr_m22=0x01;
unsigned short pause_m22=0x01;

unsigned short temp_pause_m22=0;
unsigned char  change_flag_m22=0;
unsigned char  return_main_menu_flag_m22=0;

//для курсора
cursor_struct cursor_m22;


////----------функции----------------
void menu22_out(void);
void menu22_refresh(void);
void menu22_key_pressed(void);

void menu22_2_out(void);
void menu22_2_refresh(void);
void menu22_2_key_pressed(void);
  void menu22_2_select(void);
  void menu22_2_plus(void);
  void menu22_2_minus(void);
  void menu22_2_save(void);
  
void menu22_3_out(void);
void menu22_3_refresh(void);
void menu22_3_key_pressed(void);
  void menu22_3_yes(void);
  void menu22_3_yes_for_all(void);


//    void menu22_2_yes(void);

//--------------------------основная часть-------------------------------------------
///////////////////////////////////
/*rom near*/const unsigned char menu_22[]=
//  '^ НАЧАЛЬНАЯ ПАУЗА  v'
//  '   ПЕРЕД СТАРТОМ    '
//  '                    '
//  'M         ИЗМЕНИТЬ J'


// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//       Н  А  Ч  А  Л  Ь  Н  А  Я     П  А  У  З  А        v'
 {__,__,_H,_A,CH,_A,RL,_b,_H,_A,JA,__,RP,_A,RU,RZ,_A,__,__,zd,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//          П  Е  Р  Е  Д     С  Т  А  Р  Т  О  М
  __,__,__,RP,_E,xP,_E,RD,__,xC,_T,_A,xP,_T,_O,_M,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//  
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
// М                             И  З  М  Е  Н  И  Т  Ь     j
  _M,__,__,__,__,__,__,__,__,__,RI,RZ,_M,_E,_H,RI,_T,_b,__,zj}; 

void menu22_out(void)
{
  //инициаллизация параметров меню
  menu_body_out(menu_22);
  active_menu=m22;
  flags_menu.f_refresh=0;
  flags_menu.f_autochange=0;
  //инициаллизация переменных menu22
  
  //выводим переменные
  menu22_refresh();
  return;
}

///////////////////////////////////
void menu22_refresh(void)
{
  return;
}

///////////////////////////////////
void menu22_key_pressed(void)
{
  switch(button_status)
    {
    case (0b00001000):  //левая верхняя кнопка
      break;  
    case (0b00000100):  //левая нижняя кнопка
      menu0_out();
      break;  
    case (0b00000010):  //правая верхняя кнопка
      menu21_out();
      break;  
    case (0b00000001):  //правая нижняя кнопка
      menu22_2_out();
      break;  
    
    default:
      break;
  }

  return;
}



///////////////////////////////////
/*rom near*/ const unsigned char menu_22_2[]=
//  '-      БК  01      +'
//  '                    '
//  '   НОВЫЙ АДРЕС 02   '
//  'M                  >'

//  '--------------------'
//  '-    БК    01      +'
//  '                    '
//  '    ПАУЗА  100 мс   '
//  'M                  >'

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// -              Б  К                 0  1                 +
 {zm,__,__,__,__,RB,_K,__,__,__,__,__,__,__,__,__,__,__,__,zp,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//     
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//             П  А  У  З  А        1  0  0     м  с 
  __,__,__,__,RP,_A,RU,RZ,_A,__,__,__,__,__,__,rm,_c,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// M                                                        >
  _M,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,z0}; 


void menu22_2_out(void)
{
  //инициаллизация параметров меню
  menu_body_out(menu_22_2);
  active_menu=m22_2;
  flags_menu.f_refresh=1;
  flags_menu.f_autochange=1;
  autochange_rate=AUTOCHANGE_RATE_FAST;
  //курсор
  cursor_m22.position=0;
  cursor_m22.count=0;
  cursor_m22.flags.byte=0;
  //выводим переменные
  menu22_2_refresh();
  return;
}


///////////////////////////////////
void menu22_2_refresh(void)
{
  unsigned char cursor_string[3]={zb,zb,zb};
  unsigned char err_val[3]={zw,zw,zw};
  
  //выводим переменные
  //адрес слейва
  menu_variable_out(0,12,addr_m22,2);
  //интревал
   if(!modbus_master_read_reg(addr_m22,REG_CONFIG_START_PAUSE,1, &pause_m22))
  {
    //приняли данные
    menu_variable_out(2,11,pause_m22,3);
  }
  else
  {
    //ошибка  
    menu_string_out(2,11,err_val,3);
  }
  
  //выводим курсор
  cursor_m22.count++;
  if(cursor_m22.flags.f_cursor_on==1)
  {
    //курсор должен выводиться
    if(cursor_m22.count>=CURSOR_ON_TIME)
    {
      cursor_m22.count=0;
      cursor_m22.flags.f_cursor_on=0;
    }
    switch(cursor_m22.position)
      {
      case (0):
        menu_string_out(0,12,cursor_string,2);
        break;
      case (1):
        menu_string_out(2,11,cursor_string,3);
        break;
    }
  }
  else
  {
    //курсвор не должен выводиться
    if(cursor_m22.count>=CURSOR_OFF_TIME)
    {
      cursor_m22.count=0;
      cursor_m22.flags.f_cursor_on=1;
    }

  }
  
  return;
}

///////////////////////////////////
void menu22_2_key_pressed(void)
{
  switch(button_status)
  {
    case (0b00001000):  //левая верхняя кнопка
      menu22_2_minus();
      break;  
    case (0b00000100):  //левая нижняя кнопка
      menu22_2_save();
      break;  
    case (0b00000010):  //правая верхняя кнопка
      menu22_2_plus();
      break;  
    case (0b00000001):  //правая нижняя кнопка
      menu22_2_select();
      break;  
    default:
      break;
  }

  return;
}

///////////////////////////////////
void menu22_2_select(void)
{
  if((cursor_m22.position==1)&&(change_flag_m22==1) && (temp_pause_m22!=pause_m22) )
  {
    return_main_menu_flag_m22=0;
    menu22_3_out();
  }
    
  cursor_m22.position++;
  if(cursor_m22.position==2) cursor_m22.position=0;
  cursor_m22.count=0;
  cursor_m22.flags.f_cursor_on=1;
  return;
}

///////////////////////////////////
void menu22_2_plus(void)
{
  switch(cursor_m22.position)
  {
    case (0):   //адрес слейва
      addr_m22++;
      if(addr_m22==73) addr_m22=1;
      break;
    case (1):   //интервал между измерениями
      if(change_flag_m22==0)
      {
        change_flag_m22=1;
        temp_pause_m22=pause_m22;
      }
      pause_m22++;
      if(pause_m22>START_PAUSE_MAX_VALUE) pause_m22=START_PAUSE_MIN_VALUE;
      
      modbus_master_write_reg(addr_m22,REG_CONFIG_START_PAUSE,pause_m22);
      break;
  }
  cursor_m22.count=0;
  cursor_m22.flags.f_cursor_on=0;
  menu22_refresh();
  return;
}

///////////////////////////////////
void menu22_2_minus(void)
{
  switch(cursor_m22.position)
  {
    case (0):   //адрес слейва
      addr_m22--;
      if(addr_m22==0) addr_m22=72;
      break;
    case (1):   //интервал между измерениями
      if(change_flag_m22==0)
      {
        change_flag_m22=1;
        temp_pause_m22=pause_m22;
      }

      pause_m22--;
      if(pause_m22<START_PAUSE_MIN_VALUE) pause_m22=START_PAUSE_MAX_VALUE;
        
      modbus_master_write_reg(addr_m22,REG_CONFIG_START_PAUSE,pause_m22);
      break;
  }
  
  cursor_m22.count=0;
  cursor_m22.flags.f_cursor_on=0;
  menu22_refresh();
  return;
}


///////////////////////////////////
void menu22_2_save(void)
{
  //проверяем изменились ли данные
  if( (cursor_m22.position==1)&&(change_flag_m22==1) )
  {
    if(temp_pause_m22!=pause_m22)
    {
      return_main_menu_flag_m22=1;
      menu22_3_out();
    }
    else
    {
      change_flag_m22=0;
      menu22_out();
    }
  }
  else menu22_out();
  
  return;
}


/////////////////////////////////////////////////////////////////
//
///////////////////////////////////
/*rom near*/ const unsigned char menu_22_3[]=
//	'--------------------'
//  '   xx СОХРАНИТЬ     ' 
//  '   xx ИЗМЕНЕНИЯ?    ' 
//	'                    '   
//  'НЕТ               ДА'

//	'--------------------'
//  '            ДЛЯ ВСЕХ'
//  '   xx  сохранить     ' 
//	'   xx  изменения?    '
//  'НЕТ               ДА'


// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//                                     Д  Л  Я     В  С  Е  Х
 {__,__,__,__,__,__,__,__,__,__,__,__,RD,RL,JA,__,_B,xC,_E,_X,

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
//                      c  о  х  р  а  н  и  т  ь 
  __,__,__,__,__,__,__,_c,_o,_x,_p,_a,rn,ri,rt,mg,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//                      и  з  м  е  н  е  н  и  я  ?                                                                                         
  __,__,__,__,__,__,__,ri,rz,rm,_e,rn,_e,rn,ri,ja,zw,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
// Н  Е  Т                                               Д  А                                                                           
  _H,_E,_T,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,RD,_A}; 

const unsigned char menu_22_3_pic[]=
{
    0xFE, 0x02, 0xFE, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0xFE, 0x0A, 0xFE, 0x3F, 0x40, 
    0x40, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x45, 0x45, 0x7C, 0x40, 0x7F, 
};

void menu22_3_out(void)
{
  //инициаллизация параметров меню
  menu_body_out(menu_22_3);
  menu_picture_out((unsigned char*) menu_22_3_pic,24,8,14,16);
  
  active_menu=m22_3;
  flags_menu.f_refresh=0;
  flags_menu.f_autochange=0;
  //инициаллизация переменных menu
  //выводим переменные
  menu22_3_refresh();
    return;
}

///////////////////////////////////
void menu22_3_refresh(void)
{
  return;
}

///////////////////////////////////
void menu22_3_key_pressed(void)
{
  switch(button_status)
    {
    case (0b00001000):  //левая верхняя кнопка
      break;  
    case (0b00000100):  //левая нижняя кнопка
      //отмена
      modbus_master_write_reg(addr_m22,REG_CONFIG_START_PAUSE,temp_pause_m22);
      change_flag_m22=0;
      if(return_main_menu_flag_m22==1)
      {
        menu22_out();
      }
      else
      {
        menu22_2_out();  
      }
      break;  
    case (0b00000010):  //правая верхняя кнопка
      menu22_3_yes_for_all();
      break;  
    case (0b00000001):  //правая нижняя кнопка
      menu22_3_yes();
      break;  
    default:
      break;
  }
  return;
}

///////////////////////////////////
void menu22_3_yes(void)
{
  modbus_master_write_reg(addr_m22,REG_CONTROL_SAVE,CMD_SAVE_START_PAUSE);
  change_flag_m22=0;
  if(return_main_menu_flag_m22==1)
  {
    menu22_out();
  }
  else
  {
    menu22_2_out();  
  }
  return;
}


///////////////////////////////////
void menu22_3_yes_for_all(void)
{
  modbus_master_write_reg_broadcast(REG_CONFIG_START_PAUSE,pause_m22);
  modbus_master_write_reg_broadcast(REG_CONTROL_SAVE,CMD_SAVE_START_PAUSE);
  change_flag_m22=0;
  if(return_main_menu_flag_m22==1)
  {
    menu22_out();
  }
  else
  {
    menu22_2_out();  
  }
  return;
}


