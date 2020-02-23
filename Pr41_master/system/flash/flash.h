#ifndef _FLASH_INCLUDED
#define _FLASH_INCLUDED

//---------константы---------------

//---------переменные--------------
//extern unsigned short flash_addr;
//extern unsigned short flash_result;

//----------функции----------------
unsigned long flash_read_word(unsigned long addr);
void flash_erase_block(unsigned long addr);
void flash_write_word(unsigned long addr,unsigned long data);
void flash_write_row(unsigned long addr,unsigned char* data);
void flash_write_block(unsigned long addr,unsigned char* data);

#endif





