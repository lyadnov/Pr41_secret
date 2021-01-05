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


typedef struct __attribute__((packed)) { //NOTE: without packed Ubi will be aligned to word
	union {
		struct {
			unsigned char is_save:1;       //0
			unsigned char is_Ubi:1;
			unsigned char is_Tbz:1;
			unsigned char is_Tbtr:1;
			unsigned char is_Tne:1;
			unsigned char is_Hne:1;
			unsigned char is_Pne:1;
			unsigned char is_U:1;           //7
		};
		unsigned char byte;
	};
	unsigned short Ubi;
	unsigned short Tbz;
	unsigned short Tbtr;
	unsigned short Tne;
	unsigned short Hne;
	unsigned short Pne;
	unsigned short U;
} NMB_CALIBRATION;

static
int nmb_get_status_ext(unsigned char cmd, unsigned char *data_out, unsigned char *size_out);

static
unsigned short convert_if_negative(long temp)
{
	unsigned short result;

	if (temp >= 0)
		result = (unsigned short)temp;
	else
	{
		if (temp < -0x7FFE)
			result = 0; //error
		else
		{
			result = (unsigned short)(~temp + 1);
			result |= 0x8000;
		}
	}
	return result;
}


static
unsigned short nmb_sensor_get_value(unsigned char sensor_num)
{
	long temp;

	temp = ADC_get(sensor_num);

	switch(sensor_num)
	{
		case 0: //Напряжение БИ
			if (nmb_adc[sensor_num] == 0)
				return 0; //error
			temp = (temp * nmb_val[sensor_num]) / nmb_adc[sensor_num];
			break;
		case 1: //Температура радиатора БЗ
			if (nmb_adc[sensor_num] == 2734)
				return 0;
			temp = ((temp - 2734) * nmb_val[sensor_num]) / ((long)nmb_adc[sensor_num] - 2734);
			break;  
		case 2: //Температура радиатора БТР
			if (nmb_adc[sensor_num] == 2734)
				return 0;
			temp = ((temp - 2734) * nmb_val[sensor_num]) / ((long)nmb_adc[sensor_num] - 2734);
			break;  
		case 3: //Температура внутри НЭ
			if (nmb_adc[sensor_num] == 685)
				return 0;
			temp = ((temp - 685) * nmb_val[sensor_num]) / ((long)nmb_adc[sensor_num] - 685);
			break;  
		case 4: //Влажность внутри НЭ
			if (nmb_adc[sensor_num] == 681)
				return 0;
			temp = ((temp - 681) * nmb_val[sensor_num]) / ((long)nmb_adc[sensor_num] - 681);
			break;  
		case 5: //Давление внутри НЭ
			if (nmb_adc[sensor_num] == 210)
				return 0;
			temp = ( ((temp - 210) * ((long)nmb_val[sensor_num] - 204)) /
			         ((long)nmb_adc[sensor_num] - 210)
			       ) + 204;
			break;  
		case 6: //Питание процессора 3,3В
			if (nmb_adc[sensor_num] == 0)
				return 0;
			temp = (temp * nmb_val[sensor_num]) / nmb_adc[sensor_num];
			break;
		default:
			return 0;
	} 

	return convert_if_negative(temp);
}


static
int nmb_set_k_ext(unsigned char sensor_num, unsigned short val, unsigned char is_save)
{
	unsigned long temp;

	temp = ADC_get(sensor_num);

	if (val & 0x8000)
		return 1; //ignore negative val values at calibration

	//проверки
	switch(sensor_num)
	{
		case 0: //Напряжение БИ
			if (temp == 0)
				return 1;
			break;
		case 1: //Температура радиатора БЗ
			if (temp <= 2734)
				return 1;
			break;  
		case 2: //Температура радиатора БТР
			if (temp <= 2734)
				return 1;
			break;  
		case 3: //Температура внутри НЭ
			if (temp <= 685)
				return 1;
			break;  
		case 4: //Влажность внутри НЭ
			if (temp <= 681)
				return 1;
			break;  
		case 5: //Давление внутри НЭ
			if (temp <= 210)
				return 1;
			if (val < 204)
				return 1;
			break;  
		case 6: //Питание процессора 3,3В
			if (temp == 0)
				return 1;
			break;
		default:
			return 1;
	} 

	nmb_val[sensor_num] = val;
	nmb_adc[sensor_num] = temp;
	if (is_save)
	{
		eeprom_write_word(ADDR_EEPROM_SENSOR1_VAL + sensor_num * 2, nmb_val[sensor_num]);
		eeprom_write_word(ADDR_EEPROM_SENSOR1_ADC + sensor_num * 2, nmb_adc[sensor_num]);
	}

	return 0;
}


static
int nmb_set_k(unsigned char *data_in, unsigned char size_in, unsigned char *data_out, unsigned char *size_out)
{
	NMB_CALIBRATION *request;

	if (size_in != 0x10)
	{
		stat_nmb_frame_format_error++;
		return 1;
	}

	request = (NMB_CALIBRATION *) &data_in[1];

	if (request->is_Ubi)
		nmb_set_k_ext(0, request->Ubi, request->is_save);
	if (request->is_Tbz)
		nmb_set_k_ext(1, request->Tbz, request->is_save);
	if (request->is_Tbtr)
		nmb_set_k_ext(2, request->Tbtr, request->is_save);
	if (request->is_Tne)
		nmb_set_k_ext(3, request->Tne, request->is_save);
	if (request->is_Hne)
		nmb_set_k_ext(4, request->Hne, request->is_save);
	if (request->is_Pne)
		nmb_set_k_ext(5, request->Pne, request->is_save);
	if (request->is_U)
		nmb_set_k_ext(6, request->U, request->is_save);
	
	nmb_get_status_ext(NBM_CMD_SET_K_RESPONSE, data_out, size_out);

	return 0;
}


static
int nmb_get_status_ext(unsigned char cmd, unsigned char *data_out, unsigned char *size_out)
{
	NMB_STATUS status;
	int i;

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
		*((unsigned short*)&data_out[2 + i * 2]) = nmb_sensor_get_value(i);

	*size_out = 2 + NMB_NUMBER_OF_SENSORS * 2;
	return 0;
}


static
int nmb_set_mode3_discharging(unsigned char size_in, unsigned char *data_out, unsigned char *size_out)
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
	if (nmb_timer_is_active() && (nmb_mode != 0))
		return 1;

	nmb_error = 0;

	//set outputs
	if (nmb_mode != 0)
	{
		switch (nmb_mode)
		{
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
	}

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


