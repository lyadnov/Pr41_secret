#include "system\modbus\modbus.h"
#include "system\modbus\modbus_slave.h"
#include "system\modbus\crc16.h"

#include "system\usart\usart.h"
#include "system\adc\adc.h"
#include "p33FJ128GP706.h"


unsigned char frame[256]; //делаю переменною глобальной,так проще для отладки

static char modbus_read_byte(unsigned char *c, int start_of_frame)
//return 1=error 0=ok
{
	char res;
	unsigned short val;

	res = UsartRxByte(&val);
	if (res)
	{
		//данные принялись с ошибкой по линии usart
		UsartWaitForSilence();
		return 1;
	}

	if (start_of_frame)
	{
		//в 0-вом байте запроса от мастера должна быть 1 в 9-ом бите
		if ((val & 0xFF00) != 0x0100)
		{
			slave_regs[REG_ERROR_USART_9BIT]++;
			return 1;
		}
	}
	else
	{
		//в остальных байтах запроса от мастера должен быть 0 в 9-ом бите
		if (val & 0xFF00)
		{
			slave_regs[REG_ERROR_USART_9BIT]++;
			return 1;
		}
	}

	*c = val & 0xFF;
	return 0;
}


static void modbus_send_frame(unsigned char frame_out[256], unsigned short len)
{
	unsigned short i;

	rs485_send_on();

	UsartTxByteX(frame_out[0], 1); //в нулевом кадре фрейма(=посылки) 9ый бит равен 1, в остальных кадрах 0.
	for (i = 1; i < len; i++)
		UsartTxByteX(frame_out[i], 0);

	rs485_send_off();
}


void modbus_slave_loop(void)
{
	unsigned char slave_addr,function,c1,c2;
	unsigned short reg_addr,len,data,i;

	do
	{
		PORTBbits.RB0=0; //для отладки
		if (modbus_read_byte(&slave_addr, 1))
		{
			//данные принялись с ошибкой по линии usart
			PORTBbits.RB0=1; //для отладки
			continue;
		}
		PORTBbits.RB0=1; //NOTE: для отладки
	 
		slave_regs[REG_STAT_TOTAL_FRAME_NUM]++;
		if(slave_addr==cfg_slave_addr)
		{
			if (modbus_read_byte(&function, 0)) continue;
			frame[0]=cfg_slave_addr;
			frame[1]=function;
			switch (function)
			{
				/*********************************************************************/
				case 0x03:
				//case 0x04:
					/* read n byte */
					/* get adress */
					if (modbus_read_byte(&c1, 0)) break;
					if (modbus_read_byte(&c2, 0)) break;
					reg_addr=(c1<<8)+c2;
					frame[2]=c1;
					frame[3]=c2;
					/* get length */
					if (modbus_read_byte(&c1, 0)) break;
					if (modbus_read_byte(&c2, 0)) break;
					len=(c1<<8)+c2;
					frame[4]=c1;
					frame[5]=c2;
				 /* get crc16 */
					if (modbus_read_byte(&c1, 0)) break;
					if (modbus_read_byte(&c2, 0)) break;
					frame[6]=c1;
					frame[7]=c2;
					/* check crc16 */
					if (Mb_test_crc(frame,6))
					{
							slave_regs[REG_STAT_CRC_ERROR]++;
							UsartWaitForSilence(); //висим в ожидании паузы в 3.5 символа на линии
							break;
					}
					slave_regs[REG_STAT_CRC_OK]++;
					
					//входящий фрейм приняли успешно, подготавливаем ответ
					modbus_read_reg_func(reg_addr, len); //значение некоторых регистров вычисляется в этой функции
					frame[2]=len*2;
					for (i=0;i<len;i++)
					{
						if ((reg_addr + i) < TOTAL_REG_NUM) {
							frame[3 + i * 2]=slave_regs[reg_addr + i] >> 8;
							frame[4 + i * 2]=slave_regs[reg_addr + i ] & 0xff;
						} else {
							//пытаются вычитывать несуществуюище регистры, возвращаем 0
							frame[3 + i * 2] = 0;
							frame[4 + i * 2] = 0;
						}
					}
					
					Mb_calcul_crc(frame,(len*2)+3);
					modbus_send_frame(frame, (len * 2) + 5);
					break;
				/*********************************************************************/
				case 0x06:
						
					/* write on byte */
					/* get adress */
					if (modbus_read_byte(&c1, 0)) break;
					if (modbus_read_byte(&c2, 0)) break;
					reg_addr=(c1<<8)+c2;
					frame[2]=c1;
					frame[3]=c2;
					/* get data */
					if (modbus_read_byte(&c1, 0)) break;
					if (modbus_read_byte(&c2, 0)) break;
					data=(c1<<8)+c2;
					frame[4]=c1;
					frame[5]=c2;
					/* get crc16 */
					if (modbus_read_byte(&c1, 0)) break;
					if (modbus_read_byte(&c2, 0)) break;
					frame[6]=c1;
					frame[7]=c2;
					/* check crc16 */
					if (Mb_test_crc(frame, 6))
					{
							slave_regs[REG_STAT_CRC_ERROR]++;
							UsartWaitForSilence(); //висим в ожидании паузы в 3.5 символа на линии
							break;
					}
					slave_regs[REG_STAT_CRC_OK]++;

					/* store data */
					modbus_write_reg_func(reg_addr, data);
					//входящий фрейм приняли успешно, подготавливаем ответ
					modbus_send_frame(frame, 8);
					break;
				default:
					slave_regs[REG_STAT_SLAVE_FUNC_MISMATCH]++;
					UsartWaitForSilence(); //висим в ожидании паузы в 3.5 символа на линии
					break;
			}
		}
		else if (slave_addr==MODBUS_BROADCAST_ADDR)  //Broadcast packet
		{
			if (modbus_read_byte(&function, 0)) continue;
			frame[0]=MODBUS_BROADCAST_ADDR;
			frame[1]=function;
			if(function==0x06) /* write on byte */
			{
				/* get adress */
				if (modbus_read_byte(&c1, 0)) continue;
				if (modbus_read_byte(&c2, 0)) continue;
				reg_addr=(c1<<8)+c2;
				frame[2]=c1;
				frame[3]=c2;
				/* get data */
				if (modbus_read_byte(&c1, 0)) continue;
				if (modbus_read_byte(&c2, 0)) continue;
				data=(c1<<8)+c2;
				frame[4]=c1;
				frame[5]=c2;
				/* get crc16 */
				if (modbus_read_byte(&c1, 0)) continue;
				if (modbus_read_byte(&c2, 0)) continue;
				frame[6]=c1;
				frame[7]=c2;
				/* check crc16 */
				if (Mb_test_crc(frame,6))
				{
						slave_regs[REG_STAT_CRC_ERROR]++;
						UsartWaitForSilence(); //висим в ожидании паузы в 3.5 символа на линии
				}
				else
				{
					slave_regs[REG_STAT_CRC_OK]++;
					modbus_write_reg_func(reg_addr, data); //отсюда вызовется modbus_start_full_measurement()
				}
			}
			else
			{
				slave_regs[REG_STAT_SLAVE_FUNC_MISMATCH]++;
				UsartWaitForSilence(); //висим в ожидании паузы в 3.5 символа на линии
			}
		}
		else
		{
			slave_regs[REG_STAT_SLAVE_ADDR_MISMATCH]++;
			UsartWaitForSilence(); //висим в ожидании паузы в 3.5 символа на линии
		}
	}while (1);
}



