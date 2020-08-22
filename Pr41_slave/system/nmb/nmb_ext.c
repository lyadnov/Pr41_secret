#include "p33FJ128GP706.h"
#include "system\nmb\nmb.h"
#include "system\adc\adc.h"
#include "system\eeprom\eeprom.h"

int nmb_mode;
int nmb_error;
unsigned int nmb_clock_ms; //in ms
unsigned int nmb_timer_mode;

extern unsigned short stat_nmb_frame_format_error;
extern unsigned short stat_nmb_not_supported_error;

typedef struct {
	union {
		struct {
			unsigned char sv_supply_bz:1;  //0
			unsigned char sv_supply_gvi:1;
			unsigned char sv_btr:1;
			unsigned char not_used:1;
			unsigned char protect_bz:1;
			unsigned char protect_bi:1;
			unsigned char ok:1;
			unsigned char not_used2:1;     //7
		};
		unsigned char byte;
	};
} NMB_STATUS;

typedef struct {
	union {
		struct {
			unsigned char charging_allow:1;  //0
			unsigned char not_used:1;
			unsigned char I_bit0:1;
			unsigned char I_bit1:1;
			unsigned char I_bit2:1;
			unsigned char not_used2:1;
			unsigned char not_used3:1;
			unsigned char not_used4:1;       //7
		};
		unsigned char byte;
	};
} NMB_CONFIG;


// #define INTERRUPT_PROTECT(x) { char saved_ipl; SET_AND_SAVE_CPU_IPL(saved_ipl,7); x; RESTORE_CPU_IPL(saved_ipl);} (void) 0;
// void mutex_lock(char x)
// {
// 	char saved_ipl; 
// 	SET_AND_SAVE_CPU_IPL(saved_ipl, 7); 
// 	RESTORE_CPU_IPL(saved_ipl);
// }


void nmb_timer_stop(void)
{
	IFS0bits.T1IF = 0;
	IEC0bits.T1IE = 0;
	T1CONbits.TON = 0;
	nmb_clock_ms = 0;
}


void nmb_timer_callback(void)
{
	nmb_clock_ms += 25;
	
	switch (nmb_timer_mode)
	{
		case 0:
			if (nmb_mode == 1) //������� ��
			{
				//from mode0 to mode1
				if (PORTDbits.RD9 == 0)
				{
					//�� �� ���������
					PORTDbits.RD4 = 0; //��������� ������� ��
					nmb_timer_stop();
				}
				else
				{
					//�� �� �� ���������
					if (nmb_clock_ms >= 1000)
					{
						//�������, ������
						nmb_error = 1;
						PORTDbits.RD4 = 0; //��������� ������� ��
						nmb_timer_stop();
					}
				}
			}
			else if (nmb_mode == 2) //���
			{
				//from mode0 to mode2
				if (PORTDbits.RD10 == 0)
				{
					//�� ��� ���������
					//dml!!! PORTDbits.RD8 = 0; //��������� ������� ���
					nmb_timer_stop();
				}
				else
				{
					//�� �� �� ���������
					if (nmb_clock_ms >= 1000)
					{
						//�������, ������
						nmb_error = 1;
						//dml!!! PORTDbits.RD8 = 0; //��������� ������� ���
						nmb_timer_stop();
					}
				}
			}
			else if (nmb_mode == 3) //���
			{
				//from mode0 to mode3
				if (PORTDbits.RD11 == 1)
				{
					//�� ��� ���������
					nmb_timer_stop();
				}
				else
				{
					//�� �� �� ���������
					if (nmb_clock_ms >= 25)
					{
						//�������, ������
						nmb_error = 1;
						nmb_timer_stop();
					}
				}
			}
			break;
		case 1:
			//from mode1 to mode0
			if (PORTDbits.RD9 == 1)
			{
				//�� �� ����������
				PORTDbits.RD5 = 0;  //���������� ������� ��
				nmb_timer_stop();
			}
			else
			{
				//�� �� �� ����������
				if (nmb_clock_ms >= 1000)
				{
					//�������, ������
					nmb_error = 1;
					PORTDbits.RD5 = 0;  //���������� ������� ��
					nmb_timer_stop();
				}
			}
			break;
		case 2:
			//from mode2 to mode0
			if (PORTDbits.RD10 == 1)
			{
				//�� ��� ����������
				nmb_timer_stop();
			}
			else
			{
				//�� ��� �� ����������
				if (nmb_clock_ms >= 1000)
				{
					//�������, ������
					nmb_error = 1;
					nmb_timer_stop();
				}
			}
			break;
		case 3:
			//from mode3 to mode0
			if (PORTDbits.RD11 == 0)
			{
				//�� ��� ����������
				nmb_timer_stop();
			}
			else
			{
				//�� ��� �� ����������
				if (nmb_clock_ms >= 25)
				{
					//�������, ������
					nmb_error = 1;
					nmb_timer_stop();
				}
			}
			break;
		default:
			break;
	}
}


void nmb_timer_start(void)
{
	nmb_clock_ms = 0;
	nmb_timer_mode = nmb_mode;
	T1CON = 0;
	T1CONbits.TCS = 0;    //Timer1 Clock Source Select bit: Internal clock (Fcy=40MHz=Fosc/2=80���/2
	T1CONbits.TCKPS = 2;  //1:64 
	PR1 = 0x3D09;       //������ = 25�� * 64 * 0x3D09 = 25��
	IPC0bits.T1IP=4;    //��������� ���������� = 4
	IFS0bits.T1IF=0;    //�� ������ ������ ���������� ���� ���������� 
	IEC0bits.T1IE=1;    //��������� ���������� �� �������1
	T1CONbits.TON=1;    //�������� ������
}


static int nmb_get_status_ext(unsigned char cmd, unsigned char *data_out, unsigned char *size_out)
{
	NMB_STATUS status;
	int i;
	unsigned short temp;

	status.sv_supply_bz = PORTDbits.RD9;
	status.sv_supply_gvi = PORTDbits.RD10;
	status.sv_btr = PORTDbits.RD11;
	status.not_used = 0;
	status.protect_bz = PORTGbits.RG2;
	status.protect_bi = PORTFbits.RF0;
	status.ok = nmb_error ? 0 : 1;
	status.not_used2 = 0;

	data_out[0] = cmd;
	data_out[1] = status.byte;
	for (i = 0; i < NMB_NUMBER_OF_SENSORS; i++)
	{
		temp = ADC_get(i);
		//temp = (temp * NMB_K) / 4095; //FIXME: use real K here
		*((unsigned short*)&data_out[2 + i * 2]) = temp;
	}

	*size_out = 2 + NMB_NUMBER_OF_SENSORS * 2;
	return 0;
}


static int nmb_set_mode3_discharging(unsigned char size_in, unsigned char *data_out, unsigned char *size_out)
{
	if (size_in != 0x01)
	{
		stat_nmb_frame_format_error++;
		return 1;
	}
	if (nmb_mode != 0)
		return 1;

	//set outputs
	PORTDbits.RD7 = 0; //��������� ��� (0=������� 1=��������)
	nmb_timer_start();

	nmb_get_status_ext(0x11, data_out, size_out);
	
	nmb_mode = 3;

	return 0;
}


static int nmb_set_mode2_gvi(unsigned char size_in, unsigned char *data_out, unsigned char *size_out)
{
	if (size_in != 0x01)
	{
		stat_nmb_frame_format_error++;
		return 1;
	}
	if (nmb_mode != 0)
		return 1;

	//set outputs
	PORTDbits.RD8 = 1; //��������� ������� ���
	nmb_timer_start();
	
	nmb_get_status_ext(0x11, data_out, size_out);

	nmb_mode = 2;

	return 0;
}


static int nmb_set_mode1_charging(unsigned char *data_in, unsigned char size_in, unsigned char *data_out, unsigned char *size_out)
{
	NMB_CONFIG config;

	if (size_in != 0x02)
	{
		stat_nmb_frame_format_error++;
		return 1;
	}
	if (nmb_mode != 0)
		return 1;

	//set outputs
	config.byte = data_in[1];
	PORTBbits.RB14 = config.charging_allow;
	PORTFbits.RF4 = config.I_bit0;
	PORTFbits.RF5 = config.I_bit1;
	PORTFbits.RF6 = config.I_bit2;

	PORTDbits.RD4 = 1; //��������� ������� ��
	nmb_timer_start();

	nmb_get_status_ext(0x11, data_out, size_out);
	
	nmb_mode = 1;

	return 0;
}


static int nmb_set_mode0_stop(unsigned char size_in, unsigned char *data_out, unsigned char *size_out)
{
	if (size_in != 0x01)
	{
		stat_nmb_frame_format_error++;
		return 1;
	}

	//set outputs
	switch (nmb_mode)
	{
		case 1:
			PORTBbits.RB14 = 0; //���������� ������� ��
			PORTDbits.RD5 = 1;  //���������� ������� ��
			break;
		case 2:
			PORTDbits.RD8 = 0; //��������� ������� ���
			break;
		case 3:
			PORTDbits.RD7 = 1; //���������� ��� (0=������� 1=��������)
			break;
		default:
			return 1; //dml!!! ��� ������, ���� nmb_mode = 0 ?
	}
	nmb_timer_start();	

	nmb_get_status_ext(0x11, data_out, size_out);
	
	nmb_mode = 0;
	return 0;
}


static int nmb_protection_reset(unsigned char size_in, unsigned char *data_out, unsigned char *size_out)
{
	if (size_in != 0x01)
	{
		stat_nmb_frame_format_error++;
		return 1;
	}

	//set outputs
	PORTGbits.RG3 = 1; //����� �� �� (0=��� ������, 1=�����)
	
	nmb_get_status_ext(0x11, data_out, size_out);

	return 0;
}


static int nmb_get_status(unsigned char size_in, unsigned char *data_out, unsigned char *size_out)
{
	if (size_in != 0x01)
	{
		stat_nmb_frame_format_error++;
		return 1;
	}

	nmb_get_status_ext(0x12, data_out, size_out);

	return 0;
}


static int nmb_get_serial_number(unsigned char size_in, unsigned char *data_out, unsigned char *size_out)
{
	if (size_in != 0x01)
	{
		stat_nmb_frame_format_error++;
		return 1;
	}

	data_out[0] = 0x1F;
	data_out[1] = 0x03;
	data_out[2] = 0x00;
	data_out[3] = 0x01;
	data_out[4] = 0x00;
	data_out[5] = 0x04;
	data_out[6] = 0x00;
	data_out[7] = 0x00;
	data_out[8] = 0x00;

	*size_out = 0x09;
	return 0;
}


int nmb_process_data(unsigned char *data_in, unsigned char size_in, 
						unsigned char *data_out, unsigned char *size_out)
{
	switch (data_in[0])
	{
		case 0x02:
			//������� ��
			return nmb_set_mode1_charging(data_in, size_in, data_out, size_out);
		case 0x03:
			//��������������� ��������
			return nmb_set_mode3_discharging(size_in, data_out, size_out);
		case 0x04:
			//������ �� ���
			return nmb_set_mode2_gvi(size_in, data_out, size_out);
		case 0x05:
			//������������� �����
			return nmb_set_mode0_stop(size_in, data_out, size_out);
		case 0x06:
			//����� ������ ��
			return nmb_protection_reset(size_in, data_out, size_out);
		case 0x0A:
			return nmb_get_status(size_in, data_out, size_out);
		case 0x0F:
			return nmb_get_serial_number(size_in, data_out, size_out);
		default:
			stat_nmb_not_supported_error++;
			return 1;
	}

	return 0;
}


void nmb_eeprom_write_default_values(void)
{
	int i;

	for (i = 0; i < NMB_NUMBER_OF_SENSORS; i++)
	{
		//�������� ��-��������� ��� ������������� �������������
		eeprom_write_word(ADDR_EEPROM_K1 + i * 2, NMB_K);
	}
	
	return;
}



void nmb_ext_init(void)
{
	nmb_mode = 0;
	nmb_error = 0;

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
