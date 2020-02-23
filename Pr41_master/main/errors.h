#ifndef _ERRORS_INCLUDED
#define _ERRORS_INCLUDED


//---------переменные--------------
extern unsigned char error_num;

#define	err_UnknownActiveMenu	0x01
#define	err_VariableTooLong		0x02

#define	err_UsartRxBufSizeSmall	0x03	//размер принимаемого буфера недостаточно большой				
#define	err_UsartRxAckError		0x04	//ACK принимаемого пакета данных несовпал				
#define err_UsartRxFrameError	0x05
#define err_UsartRxParityError	0x06
#define err_UsartRxOverrunError	0x07
#define err_UsartRxTimeout		0x08	//данные по USART так и не пришли

#define err_FlashOddAddress	    0x09    //попытка чтения нечетного адреса флеш
#define err_EepromBigAddress	0x0A    //адрес вычитываемый из еепром больше чем EEPROM_SIZE

#define err_TimerCodeTooBig 	0x0B    //код таймера, который генерим налету не умещается в 4000h слов = 24576 байт


//----------функции----------------
void error_out(void);

#endif






