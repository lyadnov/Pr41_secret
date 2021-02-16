#ifndef _NMB_INCLUDED  //NBM = not modbus
#define _NMB_INCLUDED

#define NMB_NUMBER_OF_SENSORS              7
extern unsigned short nmb_val[NMB_NUMBER_OF_SENSORS];
extern unsigned short nmb_adc[NMB_NUMBER_OF_SENSORS];

void nmb_eeprom_write_default_values(void);
void nmb_main_loop(void);
void nmb_init(void);

#endif //_NMB_INCLUDED
