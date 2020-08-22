#ifndef _NOT_MODBUS_INCLUDED
#define _NOT_MODBUS_INCLUDED

#define NMB_SLAVE_ADDR                  0x64
#define NMB_MAX_DATA_SIZE               0xFF

#define NMB_NUMBER_OF_SENSORS           7
#define NMB_K                           4095


#define USART_NMB_ERROR_PAUSE           135      //860��� � ������ ����� ������ ���� ������ � ����� ���� �������� �������
#define USART_NMB_PAUSE                 62       //400��c ���������� ����� TX � 1 � ���� ���� ������ �� ����� ������������ + ����� ����� NMB-��������

void nmb_slave_loop(void);
void nmb_init(void);

#endif //_NOT_MODBUS_INCLUDED
