#ifndef _USART_INCLUDED
#define _USART_INCLUDED


//----------константы-------------- 

//---------переменные--------------


//----------функции----------------
void UsartInit(void);

void UsartTxByteX(char data, char bit9);
#if 0
char UsartRxByte(unsigned short *data);
#endif
char UsartRxByte_withTimeout(unsigned short *data);
void UsartWaitForSilence(void);



#endif


