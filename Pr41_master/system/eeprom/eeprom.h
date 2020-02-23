#ifndef _EEPROM_INCLUDED
#define _EEPROM_INCLUDED

//---------константы---------------
#define FLASH_BLOCK_SIZE 0x400  //в словах: 2слова=3байта   
#define ADDR_FLASH_END 0x015800

#define EEPROM_SIZE 192         //в байтах 
#define ADDR_EEPROM_START (ADDR_FLASH_END-(EEPROM_SIZE/3)*2)
#define ADDR_BLOCK_WITH_EEPROM_START (ADDR_FLASH_END-FLASH_BLOCK_SIZE)

//адреса в EEPROM
#define ADDR_EEPROM_VERSION             0x00
#define ADDR_EEPROM_LIGHT               0x03
#define ADDR_EEPROM_CONTRAST            0x04

//---------переменные--------------

//----------функции----------------
unsigned char eeprom_read(unsigned char addr);
void eeprom_read_buf(unsigned char addr,unsigned char *buf,unsigned char size);
void eeprom_write_byte(unsigned char addr,unsigned char data);
void eeprom_write_word(unsigned char addr,unsigned short data);
void eeprom_write_buf(unsigned char addr,unsigned char *buf,unsigned char size);

#endif

