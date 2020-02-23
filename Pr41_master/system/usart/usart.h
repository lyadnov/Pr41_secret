#ifndef _USART_INCLUDED
#define _USART_INCLUDED


//----------константы-------------- 

//---------переменные--------------


//----------функции----------------
void UsartInit(void);

void UsartTxByteX(char data);
char UsartRxByte(unsigned char *data);
char UsartRxByte_withTimeout(unsigned char *data);
void UsartWaitForSilence(void);



#endif


