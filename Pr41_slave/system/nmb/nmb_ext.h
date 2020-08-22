#ifndef _NOT_MODBUS_EXTENDED_INCLUDED
#define _NOT_MODBUS_EXTENDED_INCLUDED

void nmb_timer2_callback(void);
void nmb_timer_callback(void);

int nmb_process_data(unsigned char *data_in, unsigned char size_in, unsigned char *data_out, unsigned char *size_out);
void nmb_eeprom_write_default_values(void);
void nmb_ext_init(void);

#endif //_NOT_MODBUS_EXTENDED_INCLUDED
