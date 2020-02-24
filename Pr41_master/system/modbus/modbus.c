#include "system\modbus\modbus.h"
#include "system\modbus\modbus_master.h"

#include "system\eeprom\eeprom.h"
#include "p33FJ128GP706.h"
#include "system\usart\usart.h" 


//для отладки
unsigned int stat_modbus_frame_all = 0;
	unsigned int stat_modbus_usart_bad = 0;
	unsigned int stat_modbus_usart_good = 0;
		unsigned int stat_modbus_frame_error_func = 0;
		unsigned int stat_modbus_frame_error_crc = 0;
		unsigned int stat_modbus_frame_error_slave_addr = 0;
		unsigned int stat_modbus_frame_crc_ok = 0;

///////////////////////////////////
void modbus_init(void)
{
	
	//ножки
	TRISBbits.TRISB15=0; //ножка RB15 на выход

	//переменные
		
	//прерывания
	
	return;
}

///////////////////////////////////
void modbus_eeprom_write_default_values(void)
{
		return;
}

