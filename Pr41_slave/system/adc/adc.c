#include "p33FJ128GP706.h"

#include "main\errors.h" 
#include "system\adc\adc.h"


void ADC_Init(void)
{
	//����� PORTB ������������� �� ����
	TRISBbits.TRISB1 = 1;  //Vref-
	TRISBbits.TRISB5 = 1;  //����������� ��������� ���
	TRISBbits.TRISB8 = 1;  //�������� ������ ��
	TRISBbits.TRISB9 = 1;  //������� ���������� 3,3�
	TRISBbits.TRISB10 = 1; //��������� ������ ��
	TRISBbits.TRISB11 = 1; //����������� ������ ��
	TRISBbits.TRISB12 = 1; //����������� ��������� ��
	TRISBbits.TRISB13 = 1; //���������� ��
	
	//1
	AD1CON1bits.AD12B = 1; //12-bit, 1-channel ADC operation
	//2
	AD1CON2bits.VCFG = 0;   //Avdd Avss � �������� �������� ����������
	//3
	AD1CON3bits.ADRC = 0;   //Clock derived from system clock
	AD1CON3bits.ADCS = 31;  //5;   //Tad=(ADCS+1)*Tcy . Tad ������ ���� > 75 nsec
	AD1CON3bits.SAMC = 31;  //����� ������������� ��� ������ Auto Sample
	//4
	AD1PCFGLbits.PCFG0 = 0; //RBO - ���������� ����
	//5
	//AD1CHS0bits.CH0SA = 0;  //Channel 0 Sample A positive input is AN0
	AD1CHS0bits.CH0NA = 0;  //Channel 0 Sample A Negative Input Vref-=AN1=RB1, ������ ��������� ��� ����� � ������� ����������� �����, � ����� ������ AN1=Vref- ���������\�����������
	//6
	AD1CON2bits.CHPS = 0;   //Converts CH0 only
	AD1CON2bits.ALTS = 0;   //Always uses channel input selects for Sample A
	AD1PCFGLbits.PCFG5 = 0; //AN5=analog input
	AD1PCFGLbits.PCFG8 = 0; //AN8=analog input
	AD1PCFGLbits.PCFG9 = 0; //AN9=analog input
	AD1PCFGLbits.PCFG10 = 0;//AN10=analog input
	AD1PCFGLbits.PCFG11 = 0;//AN11=analog input
	AD1PCFGLbits.PCFG12 = 0;//AN12=analog input
	AD1PCFGLbits.PCFG13 = 0;//AN13=analog input
	//7
	AD1CON1bits.SIMSAM = 0; //������������� ��� �� ����������
	AD1CON2bits.CSCNA = 0;  //�� ���������� ����� ������������ ��� CH0 SampleA
	AD1CSSLbits.CSS0 = 1;   //Select AN0 for input scan, ������� �� �������, ������ ��� ����� ������������ ���� �� ����������
	//8
	AD1CON1bits.ASAM = 0;   //Sampling begins when SAMP bit is set
	//AD1CON1bits.ASAM=1; //Sampling begins immediately after last conversion. SAMP bit is auto-set.
	//9
	AD1CON1bits.SSRC = 7;   //Internal counter ends sampling and starts conversion (auto-convert)
	//10
	AD1CON1bits.FORM = 0;  //Data Output Format bits = Integer (DOUT= 0000 dddd dddd dddd)
	//11
	AD1CON2bits.SMPI = 0;  //Increments the DMA address or generates interrupt after completion of every sample/conversion operation
	AD1CON2bits.BUFM = 0;  //Always starts filling buffer at address 0x0. ��� DMA?
	//12
	AD1CON4bits.DMABL = 0; //Allocates 1 word of buffer to each analog input
	//13
	AD1CON1bits.ADON = 1; //ADC module is operating
	
	return;
}


unsigned short ADC_get(unsigned char sensor_num) //������ 1��� ������ 32���
{
	unsigned short result=0;
		 
	switch(sensor_num)
	{
		case 0:
			AD1CHS0bits.CH0SA = 13; //���������� ��
			break;  
		case 1:
			AD1CHS0bits.CH0SA = 12; //����������� ��������� ��
			break;  
		case 2:
			AD1CHS0bits.CH0SA = 5; //����������� ��������� ���
			break;  
		case 3:
			AD1CHS0bits.CH0SA = 11; //����������� ������ ��
			break;  
		case 4:
			AD1CHS0bits.CH0SA = 10; //��������� ������ ��
			break;  
		case 5:
			AD1CHS0bits.CH0SA = 8; //�������� ������ ��
			break;  
		case 6:
			AD1CHS0bits.CH0SA = 9; //������� ���������� 3,3�
			break;  
		default:
			return 0;
	} 
		
		//AD1CON1bits.DONE=0;  //���������� ��� ����� ��������� ��� ���������������, ���� �� ������������ �������������
		AD1CON1bits.SAMP=1;
		
		while(AD1CON1bits.DONE==0); //���� ��������� ���
		
		AD1CON1bits.DONE=0; //��� ��� ����������� ���� ������, ����� ����������� ��������� ���������� ���������
		result=ADC1BUF0;
	
		return result;
}

