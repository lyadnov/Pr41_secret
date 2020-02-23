#ifndef _ERRORS_INCLUDED
#define _ERRORS_INCLUDED


//---------переменные--------------
extern unsigned char error_num;

#define err_FlashOddAddress	    0x09    //попытка чтения нечетного адреса флеш
#define err_EepromBigAddress	0x0A    //адрес вычитываемый из еепром больше чем EEPROM_SIZE


//----------функции----------------
void error_out(void);
void oscillograph_out(unsigned short x);
void reset(void);

#endif






