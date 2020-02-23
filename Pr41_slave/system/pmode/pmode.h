#ifndef _PMODE_INCLUDED
#define _PMODE_INCLUDED

//---------константы---------------
#define NUMBER_OF_P2_INTERVALS     16
#define P2_INTERVAL_PERIOD_MKS     100

//---------переменные--------------
extern unsigned char  cfg_pmode;
extern unsigned short cfg_p2_intervals[NUMBER_OF_P2_INTERVALS];

//----------функции----------------
void pmode_change(unsigned int value);
void pmode_eeprom_write_default_values(void);
void pmode_init(void);

#endif



