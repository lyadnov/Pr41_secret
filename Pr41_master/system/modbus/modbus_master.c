#include "system\modbus\modbus.h"
#include "system\modbus\modbus_master.h"
#include "system\modbus\crc16.h"

#include "p33FJ128GP706.h"
#include "system\usart\usart.h"


unsigned char frame_out[256];
unsigned char frame_in[256];


static int Csm_get_data(unsigned short len)
//0=ошибка 1=ок 
{
	int i;
	unsigned short read_data;

	for (i = 0; i < len; i++)
	{
		if (!UsartRxByte_withTimeout(&read_data))
			frame_in[i] = read_data & 0xFF;
		else
			return 0;
	}

	return 1;
}


static void Csm_send(unsigned short len_out) //для широковещательных пакетов
{
	int i;
	unsigned char data;

	/////////////////
	PORTBbits.RB15 = 1; //управляем приёмо-передатчиком: разрешаем запись данных в линию
		 
	//пауза 400мкс, чтобы помехи после включения приёмо-передачтика затухли + стандартная пауза между Modbus пакетами
	T8CON = 0;
	T8CONbits.TCS = 0;            //Timer8 Clock Source Select bit: Internal clock (Fcy=40MHz=Fosc/2=80мгц/2
	T8CONbits.TCKPS = 3;          //1:256  1такт=25*256нс=6.4мкс
	PR8 = USART_MODBUS_PAUSE;     //=400мкс
	TMR8 = 0;
	IFS3bits.T8IF = 0; //сбрасываем флаг
	T8CONbits.TON = 1; //включаем таймер 8
	while (IFS3bits.T8IF == 0);
	////////////////
	
	for (i = 0; i < len_out; i++)
		UsartTxByteX(frame_out[i], 0);

	//////////////////////////////
	while (U1STAbits.TRMT == 0); //ждем опустошения сдвигового регистра

	PORTBbits.RB15 = 0; //управляем приёмо-передатчиком: запрещаем запись данных в линию

	//вычищаем RX данные, т.к при отправке они заворачиваются, надо просто 4 раза RX считать.
	data = U1RXREG;  //сдвиговый RX буфер для USART, имеет размер 4 байта
	data = U1RXREG;  //поэтому вычитываем 4 байта,которые зеркально прилетают обратно при любых TX транзакциях
	data = U1RXREG;
	data = U1RXREG;
	//////////////////////////

	return;
}


static int Csm_send_and_get_result(unsigned short len_out, unsigned short len_in)
//0=ошибка 1=ок 
{
	int ret;
	
	stat_modbus_frame_all++;
	Csm_send(len_out);
	ret = Csm_get_data(len_in);
	if (ret)
		stat_modbus_usart_good++;
	else
		stat_modbus_usart_bad++;
	return ret;
}


static int modbus_master_write_reg_ext(Mbm_frame Mbframe, unsigned short data_out[])
//возвращает 0 если все хорошо -x если ошибка
{
	int len_in, len_out;
	unsigned char slave, function;
	unsigned short reg_addr, frame_len;
 
	slave = Mbframe.slave;
	function = Mbframe.function;
	reg_addr = Mbframe.address;
	frame_len = Mbframe.length;
	
	/* write one byte */
	frame_out[0] = slave;
	frame_out[1] = 6;
	frame_out[2] = reg_addr >> 8;
	frame_out[3] = reg_addr & 0xFF;
	frame_out[4] = data_out[0] >> 8;
	frame_out[5] = data_out[0] & 0xFF;
	/* compute crc */
	Mb_calcul_crc(frame_out, 6);
	/* comput length of the packet to send */
	len_out = 8;
	if (slave == MODBUS_BROADCAST_ADDR)
	{
		//широковещательный пакет, поэтому ответа от слейва не дожидаемся
		Csm_send(len_out);
		return 0;
	}

	len_in = 8;

	if (!Csm_send_and_get_result(len_out, len_in)) //dml:len_out=сколько отправляем len_in=сколько ожидаем принять
		return -3;  /* timeout error */

	if (frame_in[0] != slave)
	{
		stat_modbus_frame_error_slave_addr++;
		return -4;  /* this is not the right slave */
	}

	//анализируем принятые данные
	if (frame_in[1] != 0x06)
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

	return 0;
}


static int modbus_master_read_reg_ext(Mbm_frame Mbframe, unsigned short data_in[])
/* возвращает:
    0 : OK
   -1 : unknown modbus function
   -2 : CRC error in the slave answer
   -3 : timeout error
   -4 : answer come from an other slave
*/
{
	int i, len_in, len_out;
	unsigned char slave, function;
	unsigned short reg_addr, frame_len;
 
	slave = Mbframe.slave;
	function = Mbframe.function;
	reg_addr = Mbframe.address;
	frame_len = Mbframe.length;
	
	frame_out[0] = slave;
	frame_out[1] = 0x03;
	frame_out[2] = reg_addr >> 8;
	frame_out[3] = reg_addr & 0xFF;
	frame_out[4] = frame_len >> 8;
	frame_out[5] = frame_len & 0xFF;
	/* compute crc */
	Mb_calcul_crc(frame_out, 6);
	/* compute length of the packet to send */
	len_out = 8;

	len_in = 5 + (frame_len * 2); //5 = slave_addr(1) + func(1) + len(1) + crc(2)

	if (!Csm_send_and_get_result(len_out, len_in)) //dml:len_out = сколько отправляем len_in = сколько ожидаем принять
		return -3;  /* timeout error */

	if (frame_in[0] != slave)
	{
		stat_modbus_frame_error_slave_addr++;
		return -4;  /* this is not the right slave */
	}

	//анализируем принятые данные
	/* test received data */
	if (frame_in[1] != 0x03 && frame_in[1] != 0x04)
	{
		stat_modbus_frame_error_func++;
		return -2;
	}
	if (Mb_test_crc(frame_in, 3 + (frame_len * 2)) )
	{
		stat_modbus_frame_error_crc++;
		return -2;
	}
	/* data are ok */
	for (i = 0; i < frame_len; i++)
	{
		data_in[i] = (frame_in[3 + (i * 2)] << 8) + frame_in[4 + i * 2];
	}
	stat_modbus_frame_crc_ok++;

	return 0;
}


int modbus_master_read_reg(unsigned char slave_addr, unsigned short reg_addr, unsigned short num, unsigned short *buf_in)
//возвращает 0 если все хорошо -x если ошибка
{
	Mbm_frame frame;
	int result, i;
		
	frame.function = 0x03;      /* send data */
	frame.slave = slave_addr;   /* slave number*/ 
	frame.address = reg_addr;   //номер регистра который хотим прочитать
	frame.length = num;         //количество регистров
	
	i=0;
	do
	{
		// UsartWaitForSilence();  //выдерживаем паузу 3.5 символа между транзакциями dml: убрал эту паузу т.к выдерживаем паузу при установки TX-режима
		result = modbus_master_read_reg_ext(frame, buf_in);
		if (result == 0) return result;
		i++;
	} while (i < NUMBER_OF_RESENDS);
	
	return result;;  
}


int modbus_master_write_reg(unsigned char slave_addr, unsigned short reg_addr, unsigned short val)
//возвращает 0 если все хорошо -x если ошибка
{
	Mbm_frame frame;
	int result,i;

	frame.function = 0x06;         /* write 1 register */
	frame.slave = slave_addr;   /* slave number*/ 
	frame.address = reg_addr;   //номер регистра который хотим записать
	
	i=0;
	do
	{
		//UsartWaitForSilence();  //выдерживаем паузу 3.5 символа между транзакциями dml: убрал эту паузу т.к выдерживаем паузу при установки TX-режима
		result = modbus_master_write_reg_ext(frame, &val);
		if (result == 0)
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
		
	frame.function = 0x06;                    /* write 1 register */
	frame.slave = MODBUS_BROADCAST_ADDR;   /* slave number*/ 
	frame.address = reg_addr;             //номер регистра который хотим записать
	
 	// UsartWaitForSilence();  //выдерживаем паузу 3.5 символа между транзакциями
	result = modbus_master_write_reg_ext(frame, &val);
	return result;;  
}
