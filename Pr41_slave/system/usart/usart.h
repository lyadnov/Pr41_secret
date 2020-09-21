#ifndef _USART_INCLUDED
#define _USART_INCLUDED

#define USART_19200
//#define USART_115200
#define USART_9bit

void rs485_send_on(void);
void rs485_send_off(void);

void UsartTxByteX(unsigned char data,unsigned char bit9);

char UsartRxByte(unsigned short *data);
char UsartRxByte_withTimeout(unsigned short *data);
void UsartWaitForSilence(void);

void UsartInit(void);

#endif


