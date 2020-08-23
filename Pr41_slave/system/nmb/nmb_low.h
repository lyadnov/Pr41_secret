#ifndef _NMB_LOW_INCLUDED
#define _NMB_LOW_INCLUDED

#define NMB_MAX_DATA_SIZE                  0xFF

#define USART_NMB_ERROR_PAUSE              135 //860мкс в случае любой ошибки ждем тишины в линии этот интервал времени
#define USART_NMB_PAUSE                    62  //400мкc выставляем ножку TX в 1 и ждем пока сигнал на линии установиться + пауза между NMB-пакетами

extern unsigned short stat_nmb_frame_format_error;
extern unsigned short stat_nmb_not_supported_error;

int nmb_send_frame(unsigned char master_addr, unsigned char frame_number, unsigned char *data, unsigned char size);
int nmb_receive_frame(unsigned char *master_addr, unsigned char *frame_number, unsigned char *data, unsigned char *size);

#endif //_NMB_LOW_INCLUDED
