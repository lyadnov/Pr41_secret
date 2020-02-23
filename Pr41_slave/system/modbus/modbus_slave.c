#include "system\modbus\modbus.h"
#include "system\modbus\modbus_slave.h"
#include "system\modbus\crc16.h"

#include "system\usart\usart.h"
#include "system\adc\adc.h"
#include "p33FJ128GP706.h"


unsigned char frame[256]; //����� ���������� ����������,��� ����� ��� ������

char Mbs_read(unsigned char *c)
{
	char res;

	res=UsartRxByte(c);
	if(res)
	{
		//������ ��������� � ������� �� ����� usart
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
	 
	//����� 400���
	T8CON=0;
	T8CONbits.TCS=0;            //Timer8 Clock Source Select bit: Internal clock (Fcy=40MHz=Fosc/2=80���/2
	T8CONbits.TCKPS=3;          //1:256  1����=25*256��=6.4���
	PR8=USART_MODBUS_PAUSE;     //400���
	TMR8=0;
	IFS3bits.T8IF=0; //���������� ����
	T8CONbits.TON=1; //�������� ������ 8
	while(IFS3bits.T8IF==0);
	////////////////
	
	 for(i=0;i<len;i++)
	 {
		UsartTxByteX(frame_out[i]);
	 }
	 
	//////////////////////////////
	while(U1STAbits.TRMT==0); //���� ����������� ���������� ��������
	PORTBbits.RB15=0;
	//�������� RX ������, �.� ��� �������� ��� ��������������, ���� ������ 4 ���� RX �������.
	data=U1RXREG;  //��������� RX ����� ��� USART, ����� ������ 4 �����
	data=U1RXREG;  //������� ���������� 4 �����,������� ��������� ��������� ������� ��� ����� TX �����������
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
		PORTBbits.RB0=0; //��� �������
		if(Mbs_read(&slave_addr))
		{
			//������ ��������� � ������� �� ����� usart
			PORTBbits.RB0=1; //��� �������
			continue;
		}
		PORTBbits.RB0=1; //NOTE: ��� �������
	 
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
							UsartWaitForSilence(); //����� � �������� ����� � 3.5 ������� �� �����
							break;
					}
					slave_regs[REG_STAT_CRC_OK]++;
					
					//�������� ����� ������� �������, �������������� �����
					modbus_read_reg_func(reg_addr, len); //�������� ��������� ��������� ����������� � ���� �������
					frame[2]=len*2;
					for (i=0;i<len;i++)
					{
						if ((reg_addr + i) < TOTAL_REG_NUM) {
							frame[3 + i * 2]=slave_regs[reg_addr + i] >> 8;
							frame[4 + i * 2]=slave_regs[reg_addr + i ] & 0xff;
						} else {
							//�������� ���������� �������������� ��������, ���������� 0
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
							UsartWaitForSilence(); //����� � �������� ����� � 3.5 ������� �� �����
							break;
					}
					slave_regs[REG_STAT_CRC_OK]++;

					/* store data */
					modbus_write_reg_func(reg_addr, data);
					//�������� ����� ������� �������, �������������� �����
					Mbs_write(frame,8);
					break;
				default:
					slave_regs[REG_STAT_SLAVE_FUNC_MISMATCH]++;
					UsartWaitForSilence(); //����� � �������� ����� � 3.5 ������� �� �����
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
						UsartWaitForSilence(); //����� � �������� ����� � 3.5 ������� �� �����
				}
				else
				{
					slave_regs[REG_STAT_CRC_OK]++;
					modbus_write_reg_func(reg_addr, data); //������ ��������� modbus_start_full_measurement()
				}
			}
			else
			{
				slave_regs[REG_STAT_SLAVE_FUNC_MISMATCH]++;
				UsartWaitForSilence(); //����� � �������� ����� � 3.5 ������� �� �����
			}
		}
		else
		{
			slave_regs[REG_STAT_SLAVE_ADDR_MISMATCH]++;
			UsartWaitForSilence(); //����� � �������� ����� � 3.5 ������� �� �����
		}
	}while (1);
}



