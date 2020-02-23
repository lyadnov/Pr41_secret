#include "p33FJ128GP706.h"

#include "system\eeprom\eeprom.h"
#include "system\menu\menu_proc.h"
#include "system\menu\menu00_main.h"

#include "system\modbus\modbus.h"
#include "system\modbus\modbus_master.h"


//----------���������--------------


//---------����������--------------

////----------�������----------------
void menu19_out(void);
void menu19_key_pressed(void);


//--------------------------�������� �����-------------------------------------------

///////////////////////////////////
/*rom near*/const unsigned char menu_19[]=
//  '--------------------'
//  '       ������       '
//  '      ���������     '
//  '                    '
//  'M                   '

// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//                      �  �  �  �  �  �
 {__,__,__,__,__,__,__,RZ,_A,RP,xP,_O,xC,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//                   �  �  �  �  �  �  �  �  �     
  __,__,__,__,__,__,_O,_T,RP,xP,_A,_B,RL,_E,_H,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
//  
  __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  
// �                            
  _M,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__}; 


void menu19_out(void)
{
  //�������������� ���������� ����
  menu_body_out(menu_19);
  active_menu=m19;
  flags_menu.f_refresh=0;
  flags_menu.f_autochange=0;
  
  //������ �� ����� ���������
  modbus_master_write_reg_broadcast(REG_CONTROL_MEASURE_START, CMD_MEASURE_START); 
  return;
}


///////////////////////////////////
void menu19_key_pressed(void)
{
  switch(button_status)
    {
    case (0b00001000):  //����� ������� ������
      break;  
    case (0b00000100):  //����� ������ ������
      menu0_out();
      break;  
    case (0b00000010):  //������ ������� ������
      break;  
    case (0b00000001):  //������ ������ ������
      break;  
    
    default:
      break;
  }

  return;
}



