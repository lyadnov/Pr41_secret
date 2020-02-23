#ifndef _ERRORS_INCLUDED
#define _ERRORS_INCLUDED


//---------����������--------------
extern unsigned char error_num;

#define	err_UnknownActiveMenu	0x01
#define	err_VariableTooLong		0x02

#define	err_UsartRxBufSizeSmall	0x03	//������ ������������ ������ ������������ �������				
#define	err_UsartRxAckError		0x04	//ACK ������������ ������ ������ ��������				
#define err_UsartRxFrameError	0x05
#define err_UsartRxParityError	0x06
#define err_UsartRxOverrunError	0x07
#define err_UsartRxTimeout		0x08	//������ �� USART ��� � �� ������

#define err_FlashOddAddress	    0x09    //������� ������ ��������� ������ ����
#define err_EepromBigAddress	0x0A    //����� ������������ �� ������ ������ ��� EEPROM_SIZE

#define err_TimerCodeTooBig 	0x0B    //��� �������, ������� ������� ������ �� ��������� � 4000h ���� = 24576 ����


//----------�������----------------
void error_out(void);

#endif






