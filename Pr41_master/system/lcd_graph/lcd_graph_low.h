#ifndef _LCD_GRAPH_LOW_INCLUDED
#define _LCD_GRAPH_LOW_INCLUDED

//----------���������--------------
#define l   1               //����� �������� ��
#define r   0               //������ �������� ��

//---------����������--------------
extern unsigned char contrast_level;
extern unsigned char light_level;

//----------�������----------------
void graph_init_pic(void);
void graph_init_external_controller(void);
void graph_eeprom_write_default_values(void);

void graph_cmd_out(unsigned char left_or_right,unsigned char cmd);
void graph_data_out(unsigned char left_or_right,unsigned char cmd);
unsigned char graph_data_read(unsigned char left_or_right);

#endif






