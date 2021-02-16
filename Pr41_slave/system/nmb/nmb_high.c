#include "p33FJ128GP706.h"
#include "system\nmb\nmb.h"
#include "system\nmb\nmb_low.h"
#include "system\nmb\nmb_timer.h"
#include "system\adc\adc.h"
#include "system\eeprom\eeprom.h"

int nmb_mode;
int nmb_error;

#define NBM_CMD_STATUS_REQUEST             0x0A
#define NBM_CMD_STATUS_RESPONSE            0xF5
#define NBM_CMD_MODE0_REQUEST              0x05 //stop
#define NBM_CMD_MODE0_RESPONSE             0xFA
#define NBM_CMD_MODE1_REQUEST              0x02 //зарядка БИ
#define NBM_CMD_MODE1_RESPONSE             0xFD
#define NBM_CMD_MODE2_REQUEST              0x04 //работа на ГВИ
#define NBM_CMD_MODE2_RESPONSE             0xFB
#define NBM_CMD_MODE3_REQUEST              0x03 //технологическая разрядка
#define NBM_CMD_MODE3_RESPONSE             0xFC
#define NBM_CMD_PROTECTION_RESET_REQUEST   0x06
#define NBM_CMD_PROTECTION_RESET_RESPONSE  0xF9
#define NBM_CMD_GET_SERIAL_NUMBER_REQUEST  0x0F
#define NBM_CMD_GET_SERIAL_NUMBER_RESPONSE 0xF0
#define NBM_CMD_SET_K_REQUEST              0x07
#define NBM_CMD_SET_K_RESPONSE             0xF8


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
		//dml!!! добавить проверку на допустимые значения в k_in
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
		//алгоритм калибровочного коэффициента:
		//есть 
		//АЦП           0..4095
		//Давление, мАтм  0..16000 = это и есть К, т.е K=это максимальное давление в мАтм, соответсвующее максимальному АЦП=4095
		//текущее значение мАтм = текАЦП*К(2байта)/4095
		//т.е на мастер передаем значение в мАтм, а мастер делит на 1000 и выводит на экран в Атм
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
		PORTDbits.RD7 = 0; //включение БТР (0=включен 1=выключен)
		nmb_timer_start();
	}

	nmb_get_status_ext(NBM_CMD_MODE3_RESPONSE, data_out, size_out);
	
	nmb_mode = 3;

	return 0;
}


static
int nmb_set_mode2_gvi(unsigned char size_in, unsigned char *data_out, unsigned char *size_out)
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

	if (PORTDbits.RD9 == 0)
	{
		//СВ БЗ включился => нельзя включать питание ГВИ (paranoid verification)
		nmb_error = 1;
		return 1;
	}
	
	//set outputs
	if (nmb_mode == 0)
	{
		PORTDbits.RD8 = 1; //включение питания ГВИ
		nmb_timer_start();
	}
	
	nmb_get_status_ext(NBM_CMD_MODE2_RESPONSE, data_out, size_out);

	nmb_mode = 2;

	return 0;
}


static
int nmb_set_mode1_charging(unsigned char *data_in, unsigned char size_in, unsigned char *data_out, unsigned char *size_out)
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

	if (PORTDbits.RD10 == 0)
	{
		//СВ ГВИ включился => нельзя включать питание БЗ (paranoid verification)
		nmb_error = 1;
		return 1;
	}

	//set outputs
	config.byte = data_in[1];
	PORTBbits.RB14 = config.charging_allow;
	PORTFbits.RF4 = config.I_bit0;
	PORTFbits.RF5 = config.I_bit1;
	PORTFbits.RF6 = config.I_bit2;
	if (nmb_mode == 0)
	{
		PORTDbits.RD4 = 1; //включение питания БЗ
		nmb_timer_start();
	}

	nmb_get_status_ext(NBM_CMD_MODE1_RESPONSE, data_out, size_out);
	
	nmb_mode = 1;

	return 0;
}


static
int nmb_set_mode0_stop(unsigned char size_in, unsigned char *data_out, unsigned char *size_out)
{
	if (size_in != 0x01)
	{
		stat_nmb_frame_format_error++;
		return 1;
	}
	if (nmb_timer_is_active())
		return 1;

	nmb_error = 0;

	//set outputs
	switch (nmb_mode)
	{
		case 0:
			PORTDbits.RD7 = 1; //костыль, чтобы при старте RD7 из 0 переключилась в 1
			break;
		case 1:
			PORTBbits.RB14 = 0; //разрешение зарядки БИ
			PORTDbits.RD5 = 1;  //выключение питания БЗ
			break;
		case 2:
			PORTDbits.RD8 = 0; //включение питания ГВИ
			break;
		case 3:
			PORTDbits.RD7 = 1; //выключение БТР (0=включен 1=выключен)
			break;
		default:
			return 1;
	}
	nmb_timer_start();

	nmb_get_status_ext(NBM_CMD_MODE0_RESPONSE, data_out, size_out);
	
	nmb_mode = 0;
	return 0;
}


static
int nmb_protection_reset(unsigned char size_in, unsigned char *data_out, unsigned char *size_out)
{
	if (size_in != 0x01)
	{
		stat_nmb_frame_format_error++;
		return 1;
	}

	//set outputs
	PORTGbits.RG3 = 1; //сброс АЗ БЗ (0=нет сброса, 1=сброс)
	nmb_timer2_start();

	nmb_get_status_ext(NBM_CMD_PROTECTION_RESET_RESPONSE, data_out, size_out);

	return 0;
}


static
int nmb_get_status(unsigned char size_in, unsigned char *data_out, unsigned char *size_out)
{
	if (size_in != 0x01)
	{
		stat_nmb_frame_format_error++;
		return 1;
	}

	nmb_get_status_ext(NBM_CMD_STATUS_RESPONSE, data_out, size_out);

	return 0;
}


static
int nmb_get_serial_number(unsigned char size_in, unsigned char *data_out, unsigned char *size_out)
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
			//Промежуточный режим
			return nmb_set_mode0_stop(size_in, data_out, size_out);
		case NBM_CMD_MODE1_REQUEST:
			//Зярядка БИ
			return nmb_set_mode1_charging(data_in, size_in, data_out, size_out);
		case NBM_CMD_MODE2_REQUEST:
			//работа на ГВИ
			return nmb_set_mode2_gvi(size_in, data_out, size_out);
		case NBM_CMD_MODE3_REQUEST:
			//Технологическая разрядка
			return nmb_set_mode3_discharging(size_in, data_out, size_out);
		case NBM_CMD_PROTECTION_RESET_REQUEST:
			//Сброс защиты БЗ
			return nmb_protection_reset(size_in, data_out, size_out);
		case NBM_CMD_GET_SERIAL_NUMBER_REQUEST:
			return nmb_get_serial_number(size_in, data_out, size_out);
		case NBM_CMD_SET_K_REQUEST:
			//смена калибровочного коэффициента
			return nmb_set_k(data_in, size_in, data_out, size_out);
		default:
			stat_nmb_not_supported_error++;
			return 1;
	}

	return 0;
}


