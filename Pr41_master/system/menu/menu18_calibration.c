#include "p33FJ128GP706.h"

#include "system\eeprom\eeprom.h"

#include "system\menu\menu_proc.h"
#include "system\menu\menu00_main.h"
#include "system\menu\menu20_addr.h"

#include "system\modbus\modbus.h"
#include "system\modbus\modbus_master.h"

//----------константы--------------

//---------переменные--------------
unsigned char slave_m18=0x01;
unsigned char sensor_m18=0;
unsigned short k_m18=0;
unsigned short val_m18=0;

unsigned short temp_k_m18=0;
unsigned char  change_flag_m18=0;
unsigned char  return_main_menu_flag_m18=0;

//для курсора
cursor_struct cursor_m18;



////----------функции----------------
void menu18_out(void);
void menu18_refresh(void);
void menu18_key_pressed(void);

void menu18_2_out(void);
void menu18_2_refresh(void);
void menu18_2_key_pressed(void);
  void menu18_2_select(void);
  void menu18_2_plus(void);
  void menu18_2_minus(void);
  void menu18_2_save(void);
  
void menu18_3_out(void);
void menu18_3_refresh(void);
void menu18_3_key_pressed(void);
  void menu18_3_yes(void);



//    void menu18_2_yes(void);

//--------------------------основная часть-------------------------------------------
///////////////////////////////////
/*rom near*/const unsigned char menu_18[]=
//  '--------------------'
//  '^    КАЛИБРОВКА     '
//  '      ДАТЧИКОВ      '
//  '                    '
//  'M         ИЗМЕНИТЬ j'


// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//^               К  А  Л  И  Б  Р  О  В  К  А
 {zu,__,__,__,__,_K,_A,RL,RI,RB,xP,_O,_B,_K,_A,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//                   Д  А  Т  Ч  И  К  О  В
  __,__,__,__,__,__,RD,_A,_T,CH,RI,_K,_O,_B,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//  
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
// М                             И  З  М  Е  Н  И  Т  Ь     j
  _M,__,__,__,__,__,__,__,__,__,RI,RZ,_M,_E,_H,RI,_T,_b,__,zj}; 

void menu18_out(void)
{
  //инициаллизация параметров меню
  menu_body_out(menu_18);
  active_menu=m18;
  flags_menu.f_refresh=0;
  flags_menu.f_autochange=0;
  //инициаллизация переменных menu18
  
  //выводим переменные
  menu18_refresh();
  return;
}

///////////////////////////////////
void menu18_refresh(void)
{
  return;
}

///////////////////////////////////
void menu18_key_pressed(void)
{
  switch(button_status)
    {
    case (0b00001000):  //левая верхняя кнопка
      menu20_out();
      break;  
    case (0b00000100):  //левая нижняя кнопка
      menu0_out();
      break;  
    case (0b00000010):  //правая верхняя кнопка
      break;  
    case (0b00000001):  //правая нижняя кнопка
      menu18_2_out();
      break;  
    
    default:
      break;
  }

  return;
}



///////////////////////////////////
/*rom near*/ const unsigned char menu_18_2[]=
//  '--------------------'
//  '-    БК      01   +'
//  '     ДАТЧИК  P9    '
//  ' K=12.345 <> 13.443 '
//  'M                  >'
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
// -              Б  К                    0  1              +
 {zm,__,__,__,__,RB,_K,__,__,__,__,__,__,__,__,__,__,__,__,zp,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
//                Д  А  Т  Ч  И  К        P  9     
  __,__,__,__,__,RD,_A,_T,CH,RI,_K,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
//    K  =  1  2  .  3  4  5     <  >     1  3  .  4  4  3
  __,_K,ze,__,__,__,__,__,__,__,zl,zo,__,__,__,__,__,__,__,__, 
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
// M                                                        >
  _M,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,z0}; 


void menu18_2_out(void)
{
  //инициаллизация параметров меню
  menu_body_out(menu_18_2);
  active_menu=m18_2;
  flags_menu.f_refresh=1;
  flags_menu.f_autochange=1;
  autochange_rate=AUTOCHANGE_RATE_FAST;
  //курсор
  cursor_m18.position=0;
  cursor_m18.count=0;
  cursor_m18.flags.byte=0;
  //выводим переменные
  menu18_2_refresh();
  return;
}


///////////////////////////////////
void menu18_2_refresh(void)
{
  unsigned char cursor_string[6]={zb,zb,zb,zb,zb,zb};
  unsigned char err_val[6]={__,zw,zw,zw,__,__};
  unsigned char sensor0[2] ={xP,_1};
  unsigned char sensor1[2] ={xP,_2};
  unsigned char sensor2[2] ={xP,_3};
  unsigned char sensor3[2] ={xP,_4};
  unsigned char sensor4[2] ={xP,_5};
  unsigned char sensor5[2] ={xP,_6};
  unsigned char sensor6[2] ={xP,_7};
  unsigned char sensor7[2] ={xP,_8};
  unsigned char sensor8[2] ={xP,_9};
  unsigned char sensor9[2] ={_R,__};

  //выводим переменные
  //адрес слейва
  menu_variable_out(0,13,slave_m18,2);
  //датчик
  switch(sensor_m18)
  {
    case 0:
      menu_string_out(1,13,sensor0,2);
      break;
    case 1:
      menu_string_out(1,13,sensor1,2);
      break;
    case 2:
      menu_string_out(1,13,sensor2,2);
      break;
    case 3:
      menu_string_out(1,13,sensor3,2);
      break;
    case 4:
      menu_string_out(1,13,sensor4,2);
      break;
    case 5:
      menu_string_out(1,13,sensor5,2);
      break;
    case 6:
      menu_string_out(1,13,sensor6,2);
      break;
    case 7:
      menu_string_out(1,13,sensor7,2);
      break;
    case 8:
      menu_string_out(1,13,sensor8,2);
      break;
    case 9:
      menu_string_out(1,13,sensor9,2);
      break;
    default:
      break;
  }
  //K
  if(!modbus_master_read_reg(slave_m18,REG_CONFIG_K1+sensor_m18,1, &k_m18))
  {
    //приняли данные
    menu_float_variable_out(2,3,k_m18,2,3);
    //attempt_m16=0;
  }
  else
  {
    //ошибка  
    menu_string_out(2,3,err_val,6);
    //menu_variable_out(3,15,attempt_m16,3);
    //attempt_m16++;
    //if(attempt_m16>999) attempt_m16=0;
  }
  //P
  if(!modbus_master_read_reg(slave_m18,REG_SENSOR1_CURRENT_VALUE+sensor_m18,1, &val_m18))
  {
    //приняли данные
    menu_float_variable_out(2,13,val_m18,2,3);
    //attempt_m16=0;
  }
  else
  {
    //ошибка  
    menu_string_out(2,13,err_val,6);
    //attempt_m16++;
    //if(attempt_m16>999) attempt_m16=0;
  }

  //выводим курсор
  cursor_m18.count++;
  if(cursor_m18.flags.f_cursor_on==1)
  {
    //курсор должен выводится
    if(cursor_m18.count>=CURSOR_ON_TIME)
    {
      cursor_m18.count=0;
      cursor_m18.flags.f_cursor_on=0;
    }
    switch(cursor_m18.position)
      {
      case (0):
        menu_string_out(0,13,cursor_string,2);
        break;
      case (1):
        menu_string_out(1,13,cursor_string,2);
        break;
      case (2):
        menu_string_out(2,3,cursor_string,6);
        break;
    }
  }
  else
  {
    //курсвор не должен выводится
    if(cursor_m18.count>=CURSOR_OFF_TIME)
    {
      cursor_m18.count=0;
      cursor_m18.flags.f_cursor_on=1;
    }

  }
  
  return;
}

///////////////////////////////////
void menu18_2_key_pressed(void)
{
  switch(button_status)
  {
    case (0b00001000):  //левая верхняя кнопка
      menu18_2_minus();
      break;  
    case (0b00000100):  //левая нижняя кнопка
      //menu18_out();
      menu18_2_save();
      break;  
    case (0b00000010):  //правая верхняя кнопка
      menu18_2_plus();
      break;  
    case (0b00000001):  //правая нижняя кнопка
      menu18_2_select();
      break;  
    default:
      break;
  }

  return;
}

///////////////////////////////////
void menu18_2_select(void)
{
  if((cursor_m18.position==2)&&(change_flag_m18==1) && (temp_k_m18!=k_m18) )
  {
    return_main_menu_flag_m18=0;
    menu18_3_out();
  }
    
  cursor_m18.position++;
  if(cursor_m18.position==3) cursor_m18.position=0;
  cursor_m18.count=0;
  cursor_m18.flags.f_cursor_on=1;
  return;
}

///////////////////////////////////
void menu18_2_plus(void)
{
  switch(cursor_m18.position)
  {
    case (0):   //номер слейва
      slave_m18++;
      if(slave_m18==73) slave_m18=1;
      break;
    case (1):   //номер датчика
      sensor_m18++;
      if(sensor_m18==NUMBER_OF_SENSORS) sensor_m18=0;
      break;
    case (2):   //K
      if(change_flag_m18==0)
      {
        change_flag_m18=1;
        temp_k_m18=k_m18;
      }
      k_m18++;
      if( sensor_m18==(NUMBER_OF_SENSORS-1) )
      {
        //датчик сопротивления воды
        if(k_m18>R_MAX_VALUE) k_m18=R_MIN_VALUE;
      }
      else
      {
        //1..9 ый датчик давления воды
        if(k_m18>K_MAX_VALUE) k_m18=K_MIN_VALUE;
      }
      
      
      modbus_master_write_reg(slave_m18,REG_CONFIG_K1+sensor_m18,k_m18);
      //if(!modbus_master_write_reg(slave_m18,REG_CONFIG_K1+sensor_m18,k_m18)) //dml!!! вот здесь возможно надо прикрутить проверку, записались ли данные
      //{
      //  //успешно записали данные
      //  menu_float_variable_out(2,13,val_m18,2,3);
     // }
      //else
      //{
        //ошибка  
        //k_m18=33333;
      //}
  
        
        
      break;
  }
  cursor_m18.count=0;
  cursor_m18.flags.f_cursor_on=0;
  menu18_refresh();
  return;
}

///////////////////////////////////
void menu18_2_minus(void)
{
  switch(cursor_m18.position)
  {
    case (0):   //номер слейва
      slave_m18--;
      if(slave_m18==0) slave_m18=72;
      break;
    case (1):   //номер датчика
      sensor_m18--;
      if(sensor_m18==255) sensor_m18=NUMBER_OF_SENSORS-1;
      break;
    case (2):   //K
      if(change_flag_m18==0)
      {
        change_flag_m18=1;
        temp_k_m18=k_m18;
      }

      k_m18--;
      if( sensor_m18==(NUMBER_OF_SENSORS-1) )
      {
        //датчик сопротивления воды
        if(k_m18<R_MIN_VALUE) k_m18=R_MAX_VALUE;
      }
      else
      {
        //1..9 ый датчик давления воды
        if(k_m18<K_MIN_VALUE) k_m18=K_MAX_VALUE;
      }
      
      
        
      modbus_master_write_reg(slave_m18,REG_CONFIG_K1+sensor_m18,k_m18);
      break;
  }
  
  cursor_m18.count=0;
  cursor_m18.flags.f_cursor_on=0;
  menu18_refresh();
  return;
}


///////////////////////////////////
void menu18_2_save(void)
{
  //проверяем изменились ли данные
  if( (cursor_m18.position==2)&&(change_flag_m18==1) )
  {
    if(temp_k_m18!=k_m18)
    {
      return_main_menu_flag_m18=1;
      menu18_3_out();
    }
    else
    {
      change_flag_m18=0;
      menu18_out();
    }
  }
  else menu18_out();
  
  return;
}


/////////////////////////////////////////////////////////////////
//
///////////////////////////////////
/*rom near*/ const unsigned char menu_18_3[]=
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

const unsigned char menu_18_3_pic[]=
{
    0xFE, 0x02, 0xFE, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0xFE, 0x0A, 0xFE, 0x3F, 0x40, 
    0x40, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x45, 0x45, 0x7C, 0x40, 0x7F, 
};

void menu18_3_out(void)
{
  //инициаллизация параметров меню
  menu_body_out(menu_18_3);
  menu_picture_out((unsigned char*) menu_18_3_pic,24,0,14,16);
  
  active_menu=m18_3;
  flags_menu.f_refresh=0;
  flags_menu.f_autochange=0;
  //инициаллизация переменных menu
  //выводим переменные
  menu18_3_refresh();
    return;
}

///////////////////////////////////
void menu18_3_refresh(void)
{
  return;
}

///////////////////////////////////
void menu18_3_key_pressed(void)
{
  switch(button_status)
    {
    case (0b00001000):  //левая верхняя кнопка
      break;  
    case (0b00000100):  //левая нижняя кнопка
      //отмена
      modbus_master_write_reg(slave_m18,REG_CONFIG_K1+sensor_m18,temp_k_m18);
      change_flag_m18=0;
      if(return_main_menu_flag_m18==1)
      {
        menu18_out();
      }
      else
      {
        menu18_2_out();  
      }
      break;  
    case (0b00000010):  //правая верхняя кнопка
      break;  
    case (0b00000001):  //правая нижняя кнопка
      menu18_3_yes();
      break;  
    default:
      break;
  }
  return;
}

///////////////////////////////////
void menu18_3_yes(void)
{
  modbus_master_write_reg(slave_m18,REG_CONTROL_SAVE,CMD_SAVE_K);
  change_flag_m18=0;
  if(return_main_menu_flag_m18==1)
  {
    menu18_out();
  }
  else
  {
    menu18_2_out();  
  }
  return;
}



