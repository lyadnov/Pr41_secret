#ifndef _ERRORS_INCLUDED
#define _ERRORS_INCLUDED


//---------����������--------------
extern unsigned char error_num;

#define err_FlashOddAddress	    0x09    //������� ������ ��������� ������ ����
#define err_EepromBigAddress	0x0A    //����� ������������ �� ������ ������ ��� EEPROM_SIZE


//----------�������----------------
void error_out(void);
void oscillograph_out(unsigned short x);
void reset(void);

#endif






