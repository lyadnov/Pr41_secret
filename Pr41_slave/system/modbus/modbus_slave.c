#include "system\modbus\modbus.h"
#include "system\modbus\modbus_slave.h"
#include "system\modbus\crc16.h"

#include "system\usart\usart.h"
#include "system\adc\adc.h"
#include "p33FJ128GP706.h"


unsigned char frame[256]; //делаю переменною глобальной,так проще для отлаки

char Mbs_read(unsigned char *c)
{
	char res;

	res=UsartRxByte(c);
	if(res)
	{
		//данные принялись с ошибкой по линии usart
		UsartWaitForSilence();
	}

	return res;
}


void Mbs_write(unsigned char frame_out[256], unsigned short len)
{
	 unsigned short i;
	 unsigned char data;

 	/////////////////
	PORTBbits.RB15=1;
	 
	//пауза 400мкс
	T8CON=0;
	T8CONbits.TCS=0;            //Timer8 Clock Source Select bit: Internal clock (Fcy=40MHz=Fosc/2=80мгц/2
	T8CONbits.TCKPS=3;          //1:256  1такт=25*256нс=6.4мкс
	PR8=USART_MODBUS_PAUSE;     //400мкс
	TMR8=0;
	IFS3bits.T8IF=0; //сбрасываем флаг
	T8CONbits.TON=1; //включаем таймер 8
	while(IFS3bits.T8IF==0);
	////////////////
	
	 for(i=0;i<len;i++)
	 {
		UsartTxByteX(frame_out[i]);
	 }
	 
	//////////////////////////////
	while(U1STAbits.TRMT==0); //ждем опустошения сдвигового регистра
	PORTBbits.RB15=0;
	//вычищаем RX данные, т.к при отправке они заворачиваются, надо просто 4 раза RX считать.
	data=U1RXREG;  //сдвиговый RX буфер для USART, имеет размер 4 байта
	data=U1RXREG;  //поэтому вычитываем 4 байта,которые зеркально прилетают обратно при любых TX транзакциях
	data=U1RXREG;
	data=U1RXREG;
	//////////////////////////
}


void modbus_slave_loop(void)
{
	unsigned char slave_addr,function,c1,c2;
	unsigned short reg_addr,len,data,i;

	do
	{
		PORTBbits.RB0=0; //для отладки
		if(Mbs_read(&slave_addr))
		{
			//данные принялись с ошибкой по линии usart
			PORTBbits.RB0=1; //для отладки
			continue;
		}
		PORTBbits.RB0=1; //NOTE: для отладки
	 
		slave_regs[REG_STAT_TOTAL_FRAME_NUM]++;
		if(slave_addr==cfg_slave_addr)
		{
			if(Mbs_read(&function)) continue;
			frame[0]=cfg_slave_addr;
			frame[1]=function;
			switch (function)
			{
				/*********************************************************************/
				case 0x03:
				//case 0x04:
					/* read n byte */
					/* get adress */
					if(Mbs_read(&c1)) break;
					if(Mbs_read(&c2)) break;
					reg_addr=(c1<<8)+c2;
					frame[2]=c1;
					frame[3]=c2;
					/* get length */
					if(Mbs_read(&c1)) break;
					if(Mbs_read(&c2)) break;
					len=(c1<<8)+c2;
					frame[4]=c1;
					frame[5]=c2;
				 /* get crc16 */
					if(Mbs_read(&c1)) break;
					if(Mbs_read(&c2)) break;
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
					Mbs_write(frame,(len*2)+5);
					break;
				/*********************************************************************/
				case 0x06:
						
					/* write on byte */
					/* get adress */
					if(Mbs_read(&c1)) break;
					if(Mbs_read(&c2)) break;
					reg_addr=(c1<<8)+c2;
					frame[2]=c1;
					frame[3]=c2;
					/* get data */
					if(Mbs_read(&c1)) break;
					if(Mbs_read(&c2)) break;
					data=(c1<<8)+c2;
					frame[4]=c1;
					frame[5]=c2;
					/* get crc16 */
					if(Mbs_read(&c1)) break;
					if(Mbs_read(&c2)) break;
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

					/* store data */
					modbus_write_reg_func(reg_addr, data);
					//входящий фрейм приняли успешно, подготавливаем ответ
					Mbs_write(frame,8);
					break;
				default:
					slave_regs[REG_STAT_SLAVE_FUNC_MISMATCH]++;
					UsartWaitForSilence(); //висим в ожидании паузы в 3.5 символа на линии
					break;
			}
		}
		else if (slave_addr==MODBUS_BROADCAST_ADDR)  //Broadcast packet
		{
			if(Mbs_read(&function)) continue;
			frame[0]=MODBUS_BROADCAST_ADDR;
			frame[1]=function;
			if(function==0x06) /* write on byte */
			{
				/* get adress */
				if(Mbs_read(&c1)) continue;
				if(Mbs_read(&c2)) continue;
				reg_addr=(c1<<8)+c2;
				frame[2]=c1;
				frame[3]=c2;
				/* get data */
				if(Mbs_read(&c1)) continue;
				if(Mbs_read(&c2)) continue;
				data=(c1<<8)+c2;
				frame[4]=c1;
				frame[5]=c2;
				/* get crc16 */
				if(Mbs_read(&c1)) continue;
				if(Mbs_read(&c2)) continue;
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



