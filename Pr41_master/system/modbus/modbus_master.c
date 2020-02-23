#include "system\modbus\modbus.h"
#include "system\modbus\modbus_master.h"
#include "system\modbus\crc16.h"

#include "p33FJ128GP706.h"
#include "system\usart\usart.h"


unsigned char frame_out[256];
unsigned char frame_in[256];


int Csm_get_data(unsigned short len)
//0=ошибка 1=ок 
{
	int i;
	unsigned char read_data;

	for(i=0;i<len;i++)
	{
		if(!UsartRxByte_withTimeout(&read_data))
		{
			frame_in[i]=read_data;
		}
		else
		{
			return(0);
		}
	}
	return(1);
}

void Csm_send(unsigned short len_out) //для широковещательных пакетов
{
	int i;
	unsigned char data;
	
	/////////////////
	PORTBbits.RB15=1; //управляем приёмо-передатчиком: разрешаем запись данных в линию
		 
	//пауза 400мкс, чтобы помехи после включения приёмо-передачтика затухли + стандартная пауза между Modbus пакетами
	T8CON=0;
	T8CONbits.TCS=0;            //Timer8 Clock Source Select bit: Internal clock (Fcy=40MHz=Fosc/2=80мгц/2
	T8CONbits.TCKPS=3;          //1:256  1такт=25*256нс=6.4мкс
	PR8=USART_MODBUS_PAUSE;     //=400мкс
	TMR8=0;
	IFS3bits.T8IF=0; //сбрасываем флаг
	T8CONbits.TON=1; //включаем таймер 8
	while(IFS3bits.T8IF==0);
	////////////////
	
	for(i=0;i<len_out;i++)
	{
		UsartTxByteX(frame_out[i]);
	}

	//////////////////////////////
	while(U1STAbits.TRMT==0); //ждем опустошения сдвигового регистра

	PORTBbits.RB15=0; //управляем приёмо-передатчиком: запрещаем запись данных в линию

	//вычищаем RX данные, т.к при отправке они заворачиваются, надо просто 4 раза RX считать.
	data=U1RXREG;  //сдвиговый RX буфер для USART, имеет размер 4 байта
	data=U1RXREG;  //поэтому вычитываем 4 байта,которые зеркально прилетают обратно при любых TX транзакциях
	data=U1RXREG;
	data=U1RXREG;
	//////////////////////////

	return;
}


int Csm_send_and_get_result(unsigned short len_out, unsigned short len_in)
//0=ошибка 1=ок 
{
	int i;
	int ret;
	unsigned char data;

	/////////////////
	PORTBbits.RB15=1; //управляем приёмо-передатчиком: разрешаем запись данных в линию
	 
	 //пауза 400мкс, чтобы помехи после включения приёмо-передачтика затухли + стандартная пауза между Modbus пакетами
	T8CON=0;
	T8CONbits.TCS=0;            //Timer8 Clock Source Select bit: Internal clock (Fcy=40MHz=Fosc/2=80мгц/2
	T8CONbits.TCKPS=3;          //1:256  1такт=25*256нс=6.4мкс
	PR8=USART_MODBUS_PAUSE;     //=400мкс
	TMR8=0;
	IFS3bits.T8IF=0; //сбрасываем флаг
	T8CONbits.TON=1; //включаем таймер 8
	while(IFS3bits.T8IF==0);
	////////////////
	 
	for(i=0;i<len_out;i++)
	{
		/* send data */
		UsartTxByteX(frame_out[i]);
	}

	//////////////////////////////
	while(U1STAbits.TRMT==0); //ждем опустошения сдвигового регистра

	PORTBbits.RB15=0; //управляем приёмо-передатчиком: запрещаем запись данных в линию

	//вычищаем RX данные, т.к при отправке они заворачиваются, надо просто 4 раза RX считать.
	data=U1RXREG;  //сдвиговый RX буфер для USART, имеет размер 4 байта
	data=U1RXREG;  //поэтому вычитываем 4 байта,которые зеркально прилетают обратно при любых TX транзакциях
	data=U1RXREG;
	data=U1RXREG;
	
	//////////////////////////
	
	ret = Csm_get_data(len_in);

	return ret;
}


/************************************************************************************
					Mbm_master : comput and send a master packet
*************************************************************************************
input :
-------
Mb_frame    : struct describing the packet to comput
						device    : device descriptor
						slave     : slave number to call
						function  : modbus function
						address   : address of the slave to read or write
						length    : lenght of data to send
data_out    : data to send to the slave
data_in   : data to read from the slave
timeout   : timeout duration in ms
ptrfoncsnd : function to call when master send data (can be NULL if you don't whant to use it)
ptrfoncrcv : function to call when master receive data (can be NULL if you don't whant to use it)
*************************************************************************************
answer :
--------
0 : OK
-1 : unknow modbus function
-2 : CRC error in the slave answer
-3 : timeout error
-4 : answer come from an other slave
*************************************************************************************/


int modbus_master(Mbm_frame Mbframe,unsigned short data_out[], unsigned short data_in[],void *ptrfoncsnd, void *ptrfoncrcv)
//возвращает 0 если все хорошо -x если ошибка
{
	int i,len_in,len_out;
	unsigned char slave,function;
	unsigned short reg_addr,frame_len;
 
	stat_modbus_all_frame++;
 
	slave=Mbframe.slave;
	function=Mbframe.function;
	reg_addr=Mbframe.address;
	frame_len=Mbframe.length;
	
	switch (function)
	{
		case 0x03:
		//case 0x04:
			/* read n byte */
			frame_out[0]=slave;
			frame_out[1]=function;
			frame_out[2]=reg_addr>>8;
			frame_out[3]=reg_addr&0xFF;
			frame_out[4]=frame_len>>8;
			frame_out[5]=frame_len&0xFF;
			/* comput crc */
			Mb_calcul_crc(frame_out,6);
			/* comput length of the packet to send */
			len_out=8;
			break;

		case 0x06:
			/* write one byte */
			frame_out[0]=slave;
			frame_out[1]=function;
			frame_out[2]=reg_addr>>8;
			frame_out[3]=reg_addr&0xFF;
			frame_out[4]=data_out[0]>>8;
			frame_out[5]=data_out[0]&0xFF;
			/* comput crc */
			Mb_calcul_crc(frame_out,6);
			/* comput length of the packet to send */
			len_out=8;
			if(slave==MODBUS_BROADCAST_ADDR)
			{
				//широковещательный пакет, поэтому ответа от слейва не дожидаемся
				Csm_send(len_out);
				return 0;
			}
			break;

		default:
			return -1;
	}
	
	/* compute length of the slave answer */
	switch (function)
	{
		case 0x03:
		//case 0x04:
			len_in=5+(frame_len*2); //5=slave_addr(1) + func(1) + len(1) + crc(2)
			break;
		case 0x06:
				len_in=8;
			break;
		case 0x044:
				len_in=0; //ответа не ждем
			break;
		default:
			return -1;
			break;
	}

	if(!Csm_send_and_get_result(len_out,len_in)) //dml:len_out=сколько отправляем len_in=сколько ожидаем принять
	{
		stat_usart_frame_bad++;
		return -3;  /* timeout error */
	}
	else
	{
		stat_usart_frame_good++;
	}


	if (frame_in[0]!=slave)
	{
		stat_modbus_frame_error_slave_addr++;
		return -4;  /* this is not the right slave */
	}

		//анализируем принятые данные
	switch (function)
	{
		case 0x03:
		//case 0x04:
			/* test received data */
			if (frame_in[1]!=0x03 && frame_in[1]!=0x04)
			{
				stat_modbus_frame_error_func++;
				return -2;
			}
			if (Mb_test_crc(frame_in,3+(frame_len*2)))
			{
				stat_modbus_frame_error_crc++;
				Nop();
				Nop();
				Nop();
				Nop();
				Nop();
				Nop();
				return -2;
			}
			/* data are ok */
			for (i=0;i<frame_len;i++)
			{
				data_in[i]=(frame_in[3+(i*2)]<<8)+frame_in[4+i*2];
			}
			stat_modbus_frame_crc_ok++;
			break;
			
		case 0x06:
			/* test received data */
			if (frame_in[1]!=0x06)
			{
				stat_modbus_frame_error_func++;
				return -2;
			}
			if (Mb_test_crc(frame_in,6))
				{
				stat_modbus_frame_error_crc++;
				return -2;
			}
			/* data are ok */
			stat_modbus_frame_crc_ok++;
			break;

		default:
			return -1;
			break;
	}
	return 0;
}


int modbus_master_read_reg(unsigned char slave_addr, unsigned short reg_addr, unsigned short num, unsigned short *buf_in)
//возвращает 0 если все хорошо -x если ошибка
{
	Mbm_frame frame;
	int result,i;
		
	frame.function=3;         /* send data */
	frame.slave=slave_addr;   /* slave number*/ 
	frame.address=reg_addr;   //номер регистра который хотим прочитать
	frame.length=num;         //количество регистров
	
	i=0;
	do
	{
		// UsartWaitForSilence();  //выдерживаем паузу 3.5 символа между транзакциями dml: убрал эту паузу т.к выдерживаем паузу при установки TX-режима
		result=modbus_master(frame,NULL,buf_in,NULL,NULL); //датаоут - ответ от слейва
		if(result==0) return result;
		i++;
	}while(i<NUMBER_OF_RESENDS);
	
	return result;;  
}


int modbus_master_write_reg(unsigned char slave_addr, unsigned short reg_addr, unsigned short val)
//возвращает 0 если все хорошо -x если ошибка
{
	Mbm_frame frame;
	int result,i;

	frame.function=6;         /* write 1 register */
	frame.slave=slave_addr;   /* slave number*/ 
	frame.address=reg_addr;   //номер регистра который хотим записать
	
	i=0;
	do
	{
		//UsartWaitForSilence();  //выдерживаем паузу 3.5 символа между транзакциями dml: убрал эту паузу т.к выдерживаем паузу при установки TX-режима
		result=modbus_master(frame,&val,NULL,NULL,NULL);
		if (result==0)
			return result;
		i++;
	} while(i < NUMBER_OF_RESENDS);

	return result;;  
}


int modbus_master_write_reg_broadcast(unsigned short reg_addr, unsigned short val)
//возвращает 0 если все хорошо -x если ошибка
{
	Mbm_frame frame;
	int result;
		
	frame.function=6;                    /* write 1 register */
	frame.slave=MODBUS_BROADCAST_ADDR;   /* slave number*/ 
	frame.address=reg_addr;             //номер регистра который хотим записать
	
 	// UsartWaitForSilence();  //выдерживаем паузу 3.5 символа между транзакциями
	result=modbus_master(frame,&val,NULL,NULL,NULL);
	return result;;  
}
