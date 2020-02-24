#ifndef _USART_INCLUDED
#define _USART_INCLUDED


//----------константы-------------- 

//---------переменные--------------


//----------функции----------------
void UsartInit(void);

void UsartTxByteX(char data, char bit9);

char UsartRxByte(unsigned short *data);
#if 0
char UsartRxByte_withTimeout(unsigned short *data);
#endif
void UsartWaitForSilence(void);



#endif


