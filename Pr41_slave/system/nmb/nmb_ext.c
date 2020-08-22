#include "p33FJ128GP706.h"
#include "system\nmb\nmb.h"
#include "system\adc\adc.h"
#include "system\eeprom\eeprom.h"

unsigned short nmb_k[NMB_NUMBER_OF_SENSORS];

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

typedef struct {
	union {
		struct {
			unsigned char is_change:1;       //0
			unsigned char Ubi:1;
			unsigned char Tbz:1;
			unsigned char Tbtr:1;
			unsigned char Tne:1;
			unsigned char Hne:1;
			unsigned char Pne:1;
			unsigned char U:1;               //7
		};
		unsigned char byte;
	};
} NMB_K_SET_CONFIG;


typedef struct {
	union {
		struct {
			unsigned char is_change:1;       //0
			unsigned char not_used1:1;
			unsigned char not_used2:1;
			unsigned char not_used3:1;
			unsigned char not_used4:1;
			unsigned char not_used5:1;
			unsigned char ok:1;
			unsigned char not_used6:1;       //7
		};
		unsigned char byte;
	};
} NMB_K_SET_STATUS;


static void nmb_timer2_stop(void)
{
	T2CONbits.TON = 0;
	IEC0bits.T2IE = 0;
	IFS0bits.T2IF = 0;
}


void nmb_timer2_callback(void)
{
	if (PORTGbits.RG2 == 1) //���� ���. ��  ���/����
	{
		PORTGbits.RG3 = 0; //����� �� �� (0=��� ������, 1=�����)
		nmb_timer2_stop();
	}
}


static void nmb_timer2_start(void)
{
	T2CON = 0;
	T2CONbits.TCS = 0;   //Clock Source Select bit: Internal clock (Fcy=40MHz=Fosc/2=80���/2
	T2CONbits.T32 = 0;   //16bit mode
	T2CONbits.TCKPS = 2; //1:64
	PR2 = 0x3D09;        //������ = 25�� * 64 * 0x3D09 = 25��
	IPC1bits.T2IP = 5;    //��������� ���������� = 5
	IFS0bits.T2IF = 0;    //�� ������ ������ ���������� ���� ���������� 
	IEC0bits.T2IE = 1;    //��������� ���������� �� �������2
	T2CONbits.TON = 1;    //�������� ������ 2
}


static void nmb_timer_stop(void)
{
	T1CONbits.TON = 0;
	IEC0bits.T1IE = 0;
	IFS0bits.T1IF = 0;
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
					nmb_timer_stop();
				}
				else
				{
					//�� �� �� ���������
					if (nmb_clock_ms >= 1000)
					{
						//�������, ������
						nmb_error = 1;
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
			nmb_timer_stop();
			break;
	}
}


static int nmb_timer_is_active(void)
{
	return T1CONbits.TON;
}


static void nmb_timer_start(void)
{
	nmb_clock_ms = 0;
	nmb_timer_mode = nmb_mode;
	T1CON = 0;
	T1CONbits.TCS = 0;    //Timer1 Clock Source Select bit: Internal clock (Fcy=40MHz=Fosc/2=80���/2
	T1CONbits.TCKPS = 2;  //1:64 
	PR1 = 0x3D09;       //������ = 25�� * 64 * 0x3D09 = 25��
	IPC0bits.T1IP = 4;  //��������� ���������� = 4
	IFS0bits.T1IF = 0;  //�� ������ ������ ���������� ���� ���������� 
	IEC0bits.T1IE = 1;  //��������� ���������� �� �������1
	T1CONbits.TON = 1;  //�������� ������
}


static int nmb_set_k(unsigned char *data_in, unsigned char size_in, unsigned char *data_out, unsigned char *size_out)
{
	NMB_K_SET_CONFIG config;
	NMB_K_SET_STATUS status;
	unsigned short k_in;
	int index;
	unsigned long temp;

	if (size_in != 0x04)
	{
		stat_nmb_frame_format_error++;
		return 1;
	}
	config.byte = data_in[1];
	if (config.Ubi)
		index = 0;
	else if (config.Tbz)
		index = 1;
	else if (config.Tbtr)
		index = 2;
	else if (config.Tne)
		index = 3;
	else if (config.Hne)
		index = 4;
	else if (config.Pne)
		index = 5;
	else if (config.U)
		index = 6;
	else
		return 1;

	k_in = *((unsigned short*)&data_in[2]);	
	if (k_in != 0x00)
	{
		nmb_k[index] = k_in;
		//dml!!! �������� �������� �� ���������� �������� � k_in
		if (config.is_change == 0)
			eeprom_write_word(ADDR_EEPROM_K1 + index * 2, nmb_k[index]);
	}

	data_out[0] = NBM_CMD_SET_K_RESPONSE;
	
	status.byte = 0x00;
	status.is_change = config.is_change;
	status.ok = 1; //dml!!!
	data_out[1] = status.byte;
	
	*((unsigned short*)&data_out[2]) = nmb_k[index];
	
	temp = ADC_get(index);
	temp = (temp * nmb_k[index]) / 4095;
	*((unsigned short*)&data_out[4]) = temp;

	*size_out = 6;
	
	return 0;
}


static int nmb_get_status_ext(unsigned char cmd, unsigned char *data_out, unsigned char *size_out)
{
	NMB_STATUS status;
	int i;
	unsigned long temp;

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
		//�������� �������������� ������������:
		//���� 
		//���           0..4095
		//��������, ����  0..16000 = ��� � ���� �, �.� K=��� ������������ �������� � ����, �������������� ������������� ���=4095
		//������� �������� ���� = ������*�(2�����)/4095
		//�.� �� ������ �������� �������� � ����, � ������ ����� �� 1000 � ������� �� ����� � ���
		temp = (temp * nmb_k[i]) / 4095;
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
	if (nmb_timer_is_active() && (nmb_mode != 3))
		return 1;
	if ((nmb_mode != 0) && (nmb_mode != 3))
		return 1;

	//set outputs
	if (nmb_mode == 0)
	{
		PORTDbits.RD7 = 0; //��������� ��� (0=������� 1=��������)
		nmb_timer_start();
	}

	nmb_get_status_ext(NBM_CMD_MODE3_RESPONSE, data_out, size_out);
	
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
	if (nmb_timer_is_active() && (nmb_mode != 2))
		return 1;
	if ((nmb_mode != 0) && (nmb_mode != 2))
		return 1;

	//set outputs
	if (nmb_mode == 0)
	{
		PORTDbits.RD8 = 1; //��������� ������� ���
		nmb_timer_start();
	}
	
	nmb_get_status_ext(NBM_CMD_MODE2_RESPONSE, data_out, size_out);

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
	if (nmb_timer_is_active() && (nmb_mode != 1))
		return 1;
	if ((nmb_mode != 0) && (nmb_mode != 1))
		return 1;

	//set outputs
	config.byte = data_in[1];
	PORTBbits.RB14 = config.charging_allow;
	PORTFbits.RF4 = config.I_bit0;
	PORTFbits.RF5 = config.I_bit1;
	PORTFbits.RF6 = config.I_bit2;
	if (nmb_mode == 0)
	{
		PORTDbits.RD4 = 1; //��������� ������� ��
		nmb_timer_start();
	}

	nmb_get_status_ext(NBM_CMD_MODE1_RESPONSE, data_out, size_out);
	
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
	if (nmb_timer_is_active() && (nmb_mode != 0))
		return 1;

	nmb_error = 0;

	//set outputs
	if (nmb_mode != 0)
	{
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
				return 1;
		}
		nmb_timer_start();
	}

	nmb_get_status_ext(NBM_CMD_MODE0_RESPONSE, data_out, size_out);
	
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
	nmb_timer2_start();

	nmb_get_status_ext(NBM_CMD_PROTECTION_RESET_RESPONSE, data_out, size_out);

	return 0;
}


static int nmb_get_status(unsigned char size_in, unsigned char *data_out, unsigned char *size_out)
{
	if (size_in != 0x01)
	{
		stat_nmb_frame_format_error++;
		return 1;
	}

	nmb_get_status_ext(NBM_CMD_STATUS_RESPONSE, data_out, size_out);

	return 0;
}


static int nmb_get_serial_number(unsigned char size_in, unsigned char *data_out, unsigned char *size_out)
{
	if (size_in != 0x01)
	{
		stat_nmb_frame_format_error++;
		return 1;
	}

	data_out[0] = NBM_CMD_GET_SERIAL_NUMBER_RESPONSE;
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
		case NBM_CMD_STATUS_REQUEST:
			return nmb_get_status(size_in, data_out, size_out);
		case NBM_CMD_MODE0_REQUEST:
			//������������� �����
			return nmb_set_mode0_stop(size_in, data_out, size_out);
		case NBM_CMD_MODE1_REQUEST:
			//������� ��
			return nmb_set_mode1_charging(data_in, size_in, data_out, size_out);
		case NBM_CMD_MODE2_REQUEST:
			//������ �� ���
			return nmb_set_mode2_gvi(size_in, data_out, size_out);
		case NBM_CMD_MODE3_REQUEST:
			//��������������� ��������
			return nmb_set_mode3_discharging(size_in, data_out, size_out);
		case NBM_CMD_PROTECTION_RESET_REQUEST:
			//����� ������ ��
			return nmb_protection_reset(size_in, data_out, size_out);
		case NBM_CMD_GET_SERIAL_NUMBER_REQUEST:
			return nmb_get_serial_number(size_in, data_out, size_out);
		case NBM_CMD_SET_K_REQUEST:
			//����� �������������� ������������
			return nmb_set_k(data_in, size_in, data_out, size_out);
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
