#include "system\modbus\modbus.h"
#include "system\modbus\modbus_slave.h"

#include "system\eeprom\eeprom.h"
#include "system\adc\adc.h"
#include "system\pmode\pmode.h"
#include "p33FJ128GP706.h"


unsigned int slave_regs[TOTAL_REG_NUM];

unsigned char  cfg_slave_addr;
unsigned short cfg_start_pause;
unsigned short cfg_measure_interval;
unsigned short cfg_k[NUMBER_OF_SENSORS];

///////////////////////////////////
void modbus_init(void)
{
	int i;
	
	//�����

	//���������� EEPROM
	cfg_slave_addr=eeprom_read(ADDR_EEPROM_SLAVE_ADDR);
	eeprom_read_buf(ADDR_EEPROM_START_PAUSE,(unsigned char*)&cfg_start_pause,2);
	eeprom_read_buf(ADDR_EEPROM_MEASURE_INTERVAL,(unsigned char*)&cfg_measure_interval,2);
	for(i=0;i<NUMBER_OF_SENSORS;i++)
	{
		eeprom_read_buf(ADDR_EEPROM_K1+i*2,(unsigned char*)&cfg_k[i],2);
	}

	//�������� MODBUS
	for(i=0;i<TOTAL_REG_NUM;i++) slave_regs[i]=0;
	slave_regs[REG_MAGIC]=0x1234;
	slave_regs[REG_CONFIG_SLAVE_ADDR]=cfg_slave_addr;
	slave_regs[REG_CONFIG_START_PAUSE]=cfg_start_pause;
	slave_regs[REG_CONFIG_MEASURE_INTERVAL]=cfg_measure_interval;
	for(i=0;i<NUMBER_OF_SENSORS;i++)
	{
		slave_regs[REG_CONFIG_K1+i]=cfg_k[i];
	}

	//�������� pmode
	slave_regs[REG_CONFIG_PMODE] = cfg_pmode;
	for (i = 0; i < NUMBER_OF_P2_INTERVALS; i++)
	{
		slave_regs[REG_CONFIG_P2_INTERVAL1 + i] = cfg_p2_intervals[i];
	}

	//����������

	return;
}

///////////////////////////////////
void modbus_eeprom_write_default_values(void)
{
	int i;

	eeprom_write_byte(ADDR_EEPROM_SLAVE_ADDR,DEFAULT_SLAVE_ADDR);
	eeprom_write_word(ADDR_EEPROM_START_PAUSE,DEFAULT_START_PAUSE);
	eeprom_write_word(ADDR_EEPROM_MEASURE_INTERVAL,DEFAULT_MEASURE_INTERVAL);
	for(i=0;i<NUMBER_OF_SENSORS-1;i++)
	{
		//�������� ��-��������� ��� �������� ��������
		eeprom_write_word(ADDR_EEPROM_K1+i*2,DEFAULT_K);
	}
	//�������� ��-��������� ��� ������� ������������� ����
	eeprom_write_word(ADDR_EEPROM_K1+(NUMBER_OF_SENSORS-1)*2,DEFAULT_R);
	
	return;
}



void modbus_main(void)
{
		modbus_slave_loop(); //� ���� ����� ����� �����

	 return;
}


void modbus_read_reg_func(unsigned int addr, unsigned int num)
{
	int i;
	unsigned long temp;
	
	if (addr >= REG_SENSOR1_RECORD_VALUE)
		return; //�� ���� ��������� ������ ������ 0x64 ��������
	
	for (i = addr; i < (addr + num ); i++)
	{
		if( (i >= REG_SENSOR1_CURRENT_VALUE) && (i <= REG_SENSOR10_CURRENT_VALUE) )
		{
			//slave_regs[i]=ADC_get(i-REG_SENSOR1_CURRENT_VALUE+1);
			
			//�������� �������������� ������������:
			//���� 
			//���           0..4095
			//��������, ����  0..16000 = ��� � ���� �, �.� K=��� ������������ �������� � ����, �������������� ������������� ���=4095
			//������� �������� ���� = ������*�(2�����)/4095
			//�.� �� ������ �������� �������� � ����, � ������ ����� �� 1000 � ������� �� ����� � ���

			temp = ADC_get(i - REG_SENSOR1_CURRENT_VALUE);
			//temp=(temp*slave_regs[REG_CONFIG_K1+j])>>12;
			temp = (temp * slave_regs[REG_CONFIG_K1 + i - REG_SENSOR1_CURRENT_VALUE]) / 4095;
			slave_regs[i] = temp;
		}
		if(i == REG_WATER_LEVEL_CURRENT_VALUE)
			slave_regs[i] = PORTGbits.RG6; //������ ������ ����
	}
	return;
}



static void modbus_save_reg(unsigned int value)
{
	int i;
	switch (value)
	{
		case CMD_SAVE_K:
			for(i = 0; i < NUMBER_OF_SENSORS; i++)
			{
				if (cfg_k[i] != slave_regs[REG_CONFIG_K1 + i])
				{
					cfg_k[i] = slave_regs[REG_CONFIG_K1 + i];
					eeprom_write_word(ADDR_EEPROM_K1 + i * 2, cfg_k[i]);
				}
			}
			break;
		case CMD_SAVE_SLAVE_ADDR:
			cfg_slave_addr=slave_regs[REG_CONFIG_SLAVE_ADDR];
			eeprom_write_byte(ADDR_EEPROM_SLAVE_ADDR,cfg_slave_addr);
			break;
		case CMD_SAVE_START_PAUSE:
			cfg_start_pause=slave_regs[REG_CONFIG_START_PAUSE];
			eeprom_write_word(ADDR_EEPROM_START_PAUSE,cfg_start_pause);
			break;
		case CMD_SAVE_MEASURE_INTERVAL:
			cfg_measure_interval=slave_regs[REG_CONFIG_MEASURE_INTERVAL];
			eeprom_write_byte(ADDR_EEPROM_MEASURE_INTERVAL,cfg_measure_interval);
			break;
		case CMD_SAVE_PMODE:
			for(i = 0; i < NUMBER_OF_P2_INTERVALS; i++)
			{
				if(cfg_p2_intervals[i] != slave_regs[REG_CONFIG_P2_INTERVAL1 + i])
				{
					cfg_p2_intervals[i] = slave_regs[REG_CONFIG_P2_INTERVAL1 + i];
					eeprom_write_byte(ADDR_EEPROM_P2_INTERVAL1 + i, cfg_p2_intervals[i]);
				}
			}	
			break;
		default:
			break;
	}
}


void modbus_write_reg_func(unsigned int addr,unsigned int value)
{
	switch (addr)
	{
		case REG_CONFIG_SLAVE_ADDR:
			if ((value < 1) || (value > 247)) //��. https://ru.wikipedia.org/wiki/Modbus#%D0%92%D0%B2%D0%B5%D0%B4%D0%B5%D0%BD%D0%B8%D0%B5
				return;
			break;
		case REG_CONFIG_START_PAUSE:
			if ((value < START_PAUSE_MIN_VALUE) || (value > START_PAUSE_MAX_VALUE))
				return;
			break;
		case REG_CONFIG_MEASURE_INTERVAL:
			if ((value < 1) || (value > 100))
				return;
			break;
		case REG_CONFIG_K1:
		case REG_CONFIG_K2:
		case REG_CONFIG_K3:  
		case REG_CONFIG_K4:  
		case REG_CONFIG_K5:  
		case REG_CONFIG_K6:  
		case REG_CONFIG_K7:  
		case REG_CONFIG_K8:  
		case REG_CONFIG_K9:  
		case REG_CONFIG_K10:  
			if ((value < 1) || (value > K_MAX_VALUE))
				return;
			break;
		case REG_CONTROL_MEASURE_START:
			if(value == CMD_MEASURE_START)
			{
				modbus_start_full_measurement();
				slave_regs[REG_STAT_FULL_DUMP_NUM]++;
			}
			return;
		case REG_CONTROL_SAVE:
			modbus_save_reg(value);
			return;
		case REG_CONFIG_PMODE:
			if (value > 3)
				return;
			pmode_change(value);
			slave_regs[addr] = cfg_pmode;
			return;
		case REG_CONFIG_P2_INTERVAL1:
		case REG_CONFIG_P2_INTERVAL2:
		case REG_CONFIG_P2_INTERVAL3:
		case REG_CONFIG_P2_INTERVAL4:
		case REG_CONFIG_P2_INTERVAL5:
		case REG_CONFIG_P2_INTERVAL6:
		case REG_CONFIG_P2_INTERVAL7:
		case REG_CONFIG_P2_INTERVAL8:
		case REG_CONFIG_P2_INTERVAL9:
		case REG_CONFIG_P2_INTERVAL10:
		case REG_CONFIG_P2_INTERVAL11:
		case REG_CONFIG_P2_INTERVAL12:
		case REG_CONFIG_P2_INTERVAL13:
		case REG_CONFIG_P2_INTERVAL14:
		case REG_CONFIG_P2_INTERVAL15:
		case REG_CONFIG_P2_INTERVAL16:
			if (value > P2_INTERVAL_PERIOD_MKS)
				return;
			break;
		default:
			return;   //�����, ���� addr > TOTAL_REG_NUM, �� �������� ���
	}
	
	slave_regs[addr]=value;
	return;
}


void modbus_start_full_measurement(void)
{
		int i,j;
		unsigned long temp;
	 
		//����������� ��������� ����� ����� ������� ���������
		T8CON=0;
		T8CONbits.T32=1;        //������  TMR9:TMR8 - �������� � 32�-������ ������
		T8CONbits.TCS=0;        //Timer8 Clock Source Select bit: Internal clock (Fcy=40MHz=Fosc/2=80���/2
		T8CONbits.TCKPS=3;      //1:256  1����=25*256��=6.4���
		PR8=( (unsigned long)cfg_start_pause*156 ) & 0xFFFF;  //1��=256(������������)*25��*156  ������� ����� ������� 32�-������� �������
		PR9=( (unsigned long)cfg_start_pause*156 ) >> 16;      //1��=256(������������)*25��*156  ������� �����
		TMR8=0;
		TMR9=0;
		T8CONbits.TON=1; //�������� ������ 8-9
		while(IFS3bits.T9IF==0);//T9 � �� T8!
		IFS3bits.T9IF=0;  //���������� ����

		//�������������� ������ � ������ ����� �����������   
		T8CON=0;
		T8CONbits.TCS=0;        //Timer8 Clock Source Select bit: Internal clock (Fcy=40MHz=Fosc/2=80���/2
		T8CONbits.TCKPS=3;      //1:256  1����=25*256��=6.4���
		PR8=cfg_measure_interval*156;  //1��=256(������������)*25��*156
		TMR8=0;
		T8CONbits.TON=1; //�������� ������ 8
		
		//��������� ��������� �������� �� ���� ��������
		for(i=0;i<NUMBER_OF_MEASUREMENTS;i++)
		{
			for(j=0;j<(NUMBER_OF_SENSORS-1);j++) //��� ���� ���� ������� ����������� 448 ��� ���-��
			{
				//�������� �������������� ������������:
				//���� 
				//���           0..4095
				//��������, ����  0..16000 = ��� � ���� �
				//������� �������� ���� = ������*�(2�����)/4095
				//�.� �� ������ �������� �������� � ����, � ������ ����� �� 1000 � ������� �� ����� � ���

				temp=ADC_get(j);
				//temp=(temp*slave_regs[REG_CONFIG_K1+j])>>12;
				temp=(temp*slave_regs[REG_CONFIG_K1+j])/4095;
				slave_regs[REG_SENSOR1_RECORD_VALUE+(j*NUMBER_OF_MEASUREMENTS)+i]=temp;
			}
			//����������� ����� ����� �����������
			while(IFS3bits.T8IF==0);
			IFS3bits.T8IF=0;  //���������� ����
		}
	 
		return;
}
