#ifndef _NMB_LOW_INCLUDED
#define _NMB_LOW_INCLUDED

#define NMB_MAX_DATA_SIZE                  0xFF

#define USART_NMB_ERROR_PAUSE              135 // 860мкс в случае любой ошибки ждем тишины в линии этот интервал времени
#define USART_NMB_PAUSE                    135 /* 400мкc выставляем ножку TX в 1 и ждем пока сигнал на линии установится 
                                                  + еще 460мкс, чтобы пауза между NMB-пакетами была равна 860мкс */
#define USART_NMB_RECEIVE_TIMEOUT          135 // все те же полтора символа

extern unsigned short stat_nmb_total_frame_num;
    extern unsigned short stat_nmb_frame_good;
	extern unsigned short stat_usart_error_frame;
	extern unsigned short stat_usart_error_timout;
	extern unsigned short stat_usart_error_parity;
	extern unsigned short stat_usart_error_overrun;
	extern unsigned short stat_nmb_slave_addr_error;
	extern unsigned short stat_nmb_crc_error;
	extern unsigned short stat_nmb_frame_format_error;
	extern unsigned short stat_nmb_frame_format_9bit_error;
	extern unsigned short stat_nmb_not_supported_error;


int nmb_send_frame(unsigned char master_addr, unsigned char frame_number, unsigned char *data, unsigned char size);
int nmb_receive_frame(unsigned char *master_addr, unsigned char *frame_number, unsigned char *data, unsigned char *size);

#endif //_NMB_LOW_INCLUDED
