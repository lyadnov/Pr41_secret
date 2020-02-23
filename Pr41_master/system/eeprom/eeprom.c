#include "p33FJ128GP706.h"

#include "main\errors.h" 
#include "system\flash\flash.h"
#include "system\eeprom\eeprom.h"

//---------переменные--------------


//----------функции----------------

//--------------------------основная часть-------------------------------------------


///////////////////////////////////
unsigned char eeprom_read(unsigned char addr)
{
	unsigned long temp;
	unsigned char result;
	
	if(addr>=EEPROM_SIZE)
	{
		error_num=err_EepromBigAddress;
		error_out();
	}
	temp=ADDR_EEPROM_START+((addr/3)<<1);
	temp=flash_read_word(temp);
	if((addr%3)==0)
	{ 
		result= temp&0x000000ff;
	}
	else if ((addr%3)==1)
	{
		result= (temp&0x0000ff00)>>8;
	}
	else
	{
		result= (temp&0x00ff0000)>>16;
	}
	
	return(result);
}

///////////////////////////////////
void eeprom_read_buf(unsigned char addr,unsigned char *buf,unsigned char size)
{
	unsigned int i;
	
	for(i=0;i<size;i++)
		buf[i]=eeprom_read(addr+i);
	
	return;
}

///////////////////////////////////
void eeprom_write_byte(unsigned char addr,unsigned char data)
{
	unsigned int i;
	unsigned long temp;
	unsigned char eeprom_buf[1536]; //буфер для вычитывания целого блока флеш-памяти
	
	if(addr>=EEPROM_SIZE)
	{
		error_num=err_EepromBigAddress;
		error_out();
	}
	//сперва считываем блок, содержащий EEPROM в буфер
	for(i=0;i<1536;i+=3)
	{
		temp=flash_read_word(ADDR_BLOCK_WITH_EEPROM_START+((i/3)<<1));
		eeprom_buf[i]=temp;
		eeprom_buf[i+1]=temp>>8;
		eeprom_buf[i+2]=temp>>16;            
	}
	//записываем в буфер требуемое значение
	eeprom_buf[1536-EEPROM_SIZE+addr]=data;
	//затираем блок в EEPROM
	flash_erase_block(ADDR_BLOCK_WITH_EEPROM_START);
	//записываем буфер целиком обратно во флеш
	flash_write_block(ADDR_BLOCK_WITH_EEPROM_START,eeprom_buf);
	return;
}

///////////////////////////////////
void eeprom_write_word(unsigned char addr,unsigned short data)
{
	eeprom_write_buf(addr,(unsigned char*)&data,2);
	return;
}

///////////////////////////////////
void eeprom_write_buf(unsigned char addr,unsigned char *buf,unsigned char size)
{
	unsigned int i;
	unsigned long temp;
	unsigned char eeprom_buf[1536]; //буфер для вычитывания целого блока флеш-памяти
	
	if( (addr>=EEPROM_SIZE) || ((addr+size) >= EEPROM_SIZE) )
	{
		error_num=err_EepromBigAddress;
		error_out();
	}
	//сперва считываем блок, содержащий EEPROM в буфер
	for(i=0;i<1536;i+=3)
	{
		temp=flash_read_word(ADDR_BLOCK_WITH_EEPROM_START+((i/3)<<1));
		eeprom_buf[i]=temp;
		eeprom_buf[i+1]=temp>>8;
		eeprom_buf[i+2]=temp>>16;            
	}
	//записываем в буфер требуемые значения
	for(i=0;i<size;i++) eeprom_buf[1536-EEPROM_SIZE+addr+i]=buf[i];
	//затираем блок в EEPROM
	flash_erase_block(ADDR_BLOCK_WITH_EEPROM_START);
	//записываем буфер целиком обратно во флеш
	flash_write_block(ADDR_BLOCK_WITH_EEPROM_START,eeprom_buf);
	return;
}
