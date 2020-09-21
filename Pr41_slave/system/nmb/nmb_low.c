#include "p33FJ128GP706.h"
#include "system\usart\usart.h"
#include "system\nmb\nmb.h"
#include "system\nmb\nmb_low.h"
#include "system\nmb\nmb_high.h"

#define NMB_SLAVE_ADDR      0x64

unsigned short stat_nmb_total_frame_num = 0;
	unsigned short stat_nmb_frame_good = 0;
	unsigned short stat_usart_error_frame = 0;
	unsigned short stat_usart_error_timout = 0;
	unsigned short stat_usart_error_parity = 0;
	unsigned short stat_usart_error_overrun = 0;
	unsigned short stat_nmb_slave_addr_error = 0;
	unsigned short stat_nmb_crc_error = 0;
	unsigned short stat_nmb_frame_format_error = 0;
	unsigned short stat_nmb_frame_format_9bit_error = 0;
	unsigned short stat_nmb_not_supported_error = 0;


static unsigned short nmb_get_crc_ext(unsigned char *pcBlock, unsigned short len)
/* CRC-16/CCITT-FALSE */
{
	unsigned short crc = 0xFFFF;
	unsigned char i;

	while (len--)
	{
		crc ^= *pcBlock++ << 8;

		for (i = 0; i < 8; i++)
			crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
	}

	return crc;
}


static unsigned short nmb_get_crc(unsigned char dest_addr, unsigned char source_addr,
							unsigned char frame_number, 
							unsigned char *data, unsigned char size) 
{
	int i;
	unsigned char buf[NMB_MAX_DATA_SIZE + 4];

	buf[0] = dest_addr;
	buf[1] = source_addr;
	buf[2] = frame_number;
	buf[3] = size;
	
	for (i = 0; i < size; i++)
		buf[4 + i] = data[i];

	return nmb_get_crc_ext(buf, size + 4);
}


static char nmb_read_byte(unsigned char *c, int start_of_frame)
//return 1=error 0=ok
{
	char res;
	unsigned short val;

	if (start_of_frame)
		res = UsartRxByte(&val); //������ ������ - ���� ������ ��� ��������
	else
		res = UsartRxByte_withTimeout(&val);  //�������� ������ - ������� 1.5 �������
	if (res)
		return res;

#ifdef USART_9bit
	if (start_of_frame)
	{
		//� 0-��� ����� ������� �� ������� ������ ���� 1 � 9-�� ����
		if ((val & 0xFF00) != 0x0100)
		{
			stat_nmb_frame_format_9bit_error++;
			return 1;
		}
	}
	else
	{
		//� ��������� ������ ������� �� ������� ������ ���� 0 � 9-�� ����
		if (val & 0xFF00)
		{
			stat_nmb_frame_format_9bit_error++;
			return 1;
		}
	}
#endif

	*c = val & 0xFF;
	return 0;
}


int nmb_send_frame(unsigned char master_addr, unsigned char frame_number, unsigned char *data, unsigned char size)
{
	unsigned char i;
	unsigned short crc;

	crc = nmb_get_crc(master_addr, NMB_SLAVE_ADDR, frame_number, data, size);
	rs485_send_on();
	
	UsartTxByteX(master_addr, 1); //� ������� ����� ������(=�������) 9�� ��� ����� 1, � ��������� ������ 0.
	UsartTxByteX(NMB_SLAVE_ADDR, 0);
	UsartTxByteX(frame_number, 0);
	UsartTxByteX(size, 0);

	for (i = 0; i < size; i++)
		UsartTxByteX(data[i], 0);

	UsartTxByteX(((unsigned char*)&crc)[1], 0);
	UsartTxByteX(((unsigned char*)&crc)[0], 0);

	rs485_send_off();

	return 0;
}


int nmb_receive_frame(unsigned char *master_addr, unsigned char *frame_number, unsigned char *data, unsigned char *size)
{
	int res;
	unsigned char val;
	unsigned int i;
	unsigned short crc_in;
	unsigned short crc_tmp;

	//1 receive slave addr
	res = nmb_read_byte(&val, 1);
  stat_nmb_total_frame_num++;
	if (res)
		return res;
	if (val != NMB_SLAVE_ADDR)
	{
		stat_nmb_slave_addr_error++;
		return 1;
	}
	
	//2 get master addr
	res = nmb_read_byte(master_addr, 0);
	if (res)
		return res;
	
	//3 get frame N
	res = nmb_read_byte(frame_number, 0);
	if (res)
		return res;
	
	//4 get data size
	res = nmb_read_byte(size, 0);
	if (res)
		return res;
	
	//5 get data
	for (i = 0; i < *size; i++)
	{
		res = nmb_read_byte(&data[i], 0);
		if (res)
			return res;
	}
	
	//6 get crc
	res = nmb_read_byte(&((unsigned char*)&crc_in)[1], 0);
	if (res)
		return res;
	res = nmb_read_byte(&((unsigned char*)&crc_in)[0], 0);
	if (res)
		return res;
	
	//7 verify crc
	crc_tmp = nmb_get_crc(NMB_SLAVE_ADDR, *master_addr, *frame_number, data, *size);
	if (crc_tmp != crc_in)
	{
		stat_nmb_crc_error++;
		return 1;
	}

	stat_nmb_frame_good++;
	return 0;
}
