#include "p33FJ128GP706.h"

#include "system\menu\menu_proc.h"
#include "system\menu\menu03_contrast.h"
#include "system\menu\menu16_current.h"
#include "system\menu\menu18_calibration.h"
#include "system\menu\menu19_start.h"

//---------переменные--------------
unsigned char flag_security_menu;

//----------функции----------------
void menu0_out(void);
void menu0_refresh(void);
void menu0_key_pressed(void);

//--------------------------основная часть-------------------------------------------


///////////////////////////////////
/*rom near*/ const unsigned char menu_0[]=
//  '--------------------'
//  'НАСТРОЙКА   ПРОСМОТР'
//  '                    '
//  '                    '
//  '              ЗАПУСК'


// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
// Н  А  С  Т  Р  О  Й  К  А           П  Р  О  С  М  О  Т  Р
 {_H,_A,xC,_T,xP,_O,RJ,_K,_A,__,__,__,RP,xP,_O,xC,_M,_O,_T,xP,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//                                                           
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//  
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
//                                           З  А  П  У  С  К
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,RZ,_A,RP,RU,xC,_K}; 

 
void menu0_out(void)
{
  //инициаллизация параметров меню
  menu_body_out(menu_0);

  active_menu=m0;
  flags_menu.f_refresh=0;
  flags_menu.f_autochange=0;
  flag_security_menu=0;
  //инициаллизация переменных меню
  //выводим переменные
  menu0_refresh();
  return;
}


///////////////////////////////////
void menu0_refresh(void)
{
  return;
}

///////////////////////////////////
void menu0_key_pressed(void)
{
  switch(button_status)
    {
    
    case (0b00001000):  //левая верхняя кнопка
      menu18_out();
      break;  
    case (0b00000100):  //левая нижняя кнопка
      if(flag_security_menu==0) flag_security_menu=1;
      break;  
    case (0b00000010):  //правая верхняя кнопка
       menu16_out();
      break;  
    case (0b00000001):  //правая нижняя кнопка
      menu19_out();
      break;
    
     //для скрытых менюшек
    case (0b00001100):  
      if(flag_security_menu==1) flag_security_menu=2;
      else flag_security_menu=5;     
      break;
    case (0b00000110):  
      if(flag_security_menu==2) flag_security_menu=3; 
      else flag_security_menu=5;
      break;
    case (0b00000101):  
      if(flag_security_menu==3) flag_security_menu=4; 
      else flag_security_menu=5;
      break;
    case (0b00000000):  
      if(flag_security_menu==1);
      if(flag_security_menu==4)  menu3_out();
      flag_security_menu=0;
      break;    
    default:
      break;
  }
  return;
}

