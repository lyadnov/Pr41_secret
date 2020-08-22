#ifndef _NOT_MODBUS_INCLUDED
#define _NOT_MODBUS_INCLUDED

#define NMB_SLAVE_ADDR                     0x64
#define NMB_MAX_DATA_SIZE                  0xFF

#define NMB_NUMBER_OF_SENSORS              7
#define NMB_K                              4095

#define USART_NMB_ERROR_PAUSE              135 //860мкс в случае любой ошибки ждем тишины в линии этот интервал времени
#define USART_NMB_PAUSE                    62  //400мкc выставляем ножку TX в 1 и ждем пока сигнал на линии установиться + пауза между NMB-пакетами

#define NBM_CMD_STATUS_REQUEST             0x0A
#define NBM_CMD_STATUS_RESPONSE            0xF5
#define NBM_CMD_MODE0_REQUEST              0x05 //stop
#define NBM_CMD_MODE0_RESPONSE             0xFA
#define NBM_CMD_MODE1_REQUEST              0x02 //зарядка БИ
#define NBM_CMD_MODE1_RESPONSE             0xFD
#define NBM_CMD_MODE2_REQUEST              0x04 //работа на ГВИ
#define NBM_CMD_MODE2_RESPONSE             0xFB
#define NBM_CMD_MODE3_REQUEST              0x03 //технологическая разрядка
#define NBM_CMD_MODE3_RESPONSE             0xFC
#define NBM_CMD_PROTECTION_RESET_REQUEST   0x06
#define NBM_CMD_PROTECTION_RESET_RESPONSE  0xF9
#define NBM_CMD_GET_SERIAL_NUMBER_REQUEST  0x0F
#define NBM_CMD_GET_SERIAL_NUMBER_RESPONSE 0xF0
#define NBM_CMD_SET_K_REQUEST              0x07
#define NBM_CMD_SET_K_RESPONSE             0xF8


void nmb_slave_loop(void);
void nmb_init(void);

#endif //_NOT_MODBUS_INCLUDED
