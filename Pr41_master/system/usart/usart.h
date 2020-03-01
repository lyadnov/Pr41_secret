#ifndef _USART_INCLUDED
#define _USART_INCLUDED


void rs485_send_on(void);
void rs485_send_off(void);

void UsartTxByteX(unsigned char data,unsigned char bit9);
#if 0
char UsartRxByte(unsigned short *data);
#endif
char UsartRxByte_withTimeout(unsigned short *data);
void UsartWaitForSilence(void);

void UsartInit(void);

#endif


