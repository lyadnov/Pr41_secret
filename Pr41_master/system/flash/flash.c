#include "p33FJ128GP706.h"

#include "main\errors.h" 

//---------переменные--------------
unsigned short flash_addr;
unsigned long flash_result;

//----------функции----------------
extern void flash_read_word_asm(void);
extern void flash_erase_block_asm(void);
extern void flash_write_word_asm(void);
extern void flash_write_row_asm(void);
extern void flash_write_block_asm(void);

//--------------------------основная часть-------------------------------------------

///////////////////////////////////
unsigned long flash_read_word(unsigned long addr)
//вычитывает слово (24бита) их памяти программ по заданному адресу
{
	if((addr%02)!=0)
	{
		error_num=err_FlashOddAddress;
		error_out();
	} 
	TBLPAG=(addr>>16);
	flash_addr=addr;
	
	flash_read_word_asm();
	
	return(flash_result);
}


///////////////////////////////////
void flash_erase_block(unsigned long addr)
//затирает блок(1536 байт) во флеш
{
	if((addr%1024)!=0)
	{
		error_num=err_FlashOddAddress;
		error_out();
	} 
	NVMCON=0;
	NVMCONbits.NVMOP=2;     //Memory page erase operation
	NVMCONbits.ERASE=1;     
	NVMCONbits.WREN=1;      //write enable
	
	TBLPAG=(addr>>16);
	flash_addr=addr;
	
	flash_erase_block_asm();
	NVMCONbits.WREN=0;      //write disable
	return;
}

///////////////////////////////////
void flash_write_word(unsigned long addr,unsigned long data)
//вычитывает слово (24бита) их памяти программ по заданному адресу
{
	if((addr%02)!=0)
	{
		error_num=err_FlashOddAddress;
		error_out();
	} 
	TBLPAG=(addr>>16);
	flash_addr=addr;
	flash_result=data;
	
	NVMCON=0;
	NVMCONbits.WREN=1;      //write enable
	NVMCONbits.NVMOP=3;     //single word write operation
	
	flash_write_word_asm();
	
	NVMCONbits.WREN=0;      //write disable
	return;
}

///////////////////////////////////
void flash_write_row(unsigned long addr,unsigned char* data)
//записывает 192 байта
	{
	if((addr%128)!=0)
	{
		error_num=err_FlashOddAddress;
		error_out();
	} 
	TBLPAG=(addr>>16);
	flash_addr=addr;
	flash_result=(unsigned int) data;
	
	NVMCON=0;
	NVMCONbits.WREN=1;      //write enable
	NVMCONbits.NVMOP=1;     //Memory row program operation
	
	flash_write_row_asm();
	
	NVMCONbits.WREN=0;      //write disable
	return;
}

///////////////////////////////////
void flash_write_block(unsigned long addr,unsigned char* data)
//записывает блок 1536 байт по заданному адресу
{
	if((addr%1024)!=0)
	{
		error_num=err_FlashOddAddress;
		error_out();
	} 
	TBLPAG=(addr>>16);
	flash_addr=addr;
	flash_result=(unsigned int) data;
	
	NVMCON=0;
	NVMCONbits.WREN=1;      //write enable
	NVMCONbits.NVMOP=1;     //Memory row program operation
	
	flash_write_block_asm();

	NVMCONbits.WREN=0;      //write disable

	return;
}
