#include "p33FJ128GP706.h"
#include "system\eeprom\eeprom.h"
#include "system\usart\usart.h"
#include "system\nmb\nmb.h"
#include "system\nmb\nmb_low.h"
#include "system\nmb\nmb_high.h"

#define NMB_DEFAULT_K      4095
unsigned short nmb_k[NMB_NUMBER_OF_SENSORS];


void nmb_main_loop(void)
{
	int res;
	unsigned char master_addr;
	unsigned char frame_number;
	unsigned char data_in[NMB_MAX_DATA_SIZE];
	unsigned char size_in;
	unsigned char data_out[NMB_MAX_DATA_SIZE];
	unsigned char size_out;
	
	do
	{

		res = nmb_receive_frame(&master_addr, &frame_number, data_in, &size_in);
		if (res)
		{
			//error
			UsartWaitForSilence();
			continue;
		}

		res = nmb_process_data(data_in, size_in, data_out, &size_out);
		if (res)
		{
			//error
			UsartWaitForSilence();
			continue;
		}

		res = nmb_send_frame(master_addr, frame_number, data_out, size_out);
		if (res)
		{
			//error
			UsartWaitForSilence();
			continue;
		}
	}
	while(1);
}


void nmb_eeprom_write_default_values(void)
{
	int i;

	for (i = 0; i < NMB_NUMBER_OF_SENSORS; i++)
	{
		//�������� ��-��������� ��� ������������� �������������
		eeprom_write_word(ADDR_EEPROM_K1 + i * 2, NMB_DEFAULT_K);
	}
	
	return;
}


void nmb_init(void)
{
	int i;
	
	nmb_mode = 0;
	nmb_error = 0;

	//���������� EEPROM
	for(i = 0; i < NMB_NUMBER_OF_SENSORS; i++)
		eeprom_read_buf(ADDR_EEPROM_K1 + i * 2, (unsigned char*)&nmb_k[i], 2);

	//����� �� ����
	TRISDbits.TRISD9 = 1;  //���� ��(���������) ���\���� ������� ��
	TRISDbits.TRISD10 = 1; //���� �� ���\���� ������� ���
	TRISDbits.TRISD11 = 1; //���� �� ���\���� ���
	TRISFbits.TRISF0 = 1;  //���� ���. �� ���/����
	TRISGbits.TRISG2 = 1;  //���� ���. ��  ���/����
	
	//����� �� �����
	TRISBbits.TRISB14 = 0; //����� ���������� ������� ��
	PORTBbits.RB14 = 0;
	TRISFbits.TRISF4 = 0;  //����� ���0
	PORTFbits.RF4 = 0;
	TRISFbits.TRISF5 = 0;  //����� ���1
	PORTFbits.RF5 = 0;
	TRISFbits.TRISF6 = 0;  //����� ���2
	PORTFbits.RF6 = 0;

	TRISDbits.TRISD4 = 0;  //����� ��������� ������� ��
	PORTDbits.RD4 = 0;
	TRISDbits.TRISD5 = 0;  //����� ���������� ������� ��
	PORTDbits.RD5 = 0;
	TRISDbits.TRISD7 = 0;  //����� ��������� ���
	PORTDbits.RD7 = 1;     //0=������� 1=��������
	TRISDbits.TRISD8 = 0;  //����� ��������� ������� ���
	PORTDbits.RD8 = 0;
	
	TRISGbits.TRISG3 = 0;  //����� �� �� (0=��� ������, 1=�����)
	PORTGbits.RG3 = 0;
}
