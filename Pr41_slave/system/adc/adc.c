#include "p33FJ128GP706.h"

#include "main\errors.h" 
#include "system\adc\adc.h"

//---------переменные--------------


//----------функции----------------

//--------------------------основная часть-------------------------------------------


///////////////////////////////////
void ADC_Init(void)
{
	//ножки PORTB конфигурируем на вход
	TRISBbits.TRISB1=1; //Vref-
	TRISBbits.TRISB2=1; //сенсор1
	TRISBbits.TRISB3=1;
	TRISBbits.TRISB4=1;
	TRISBbits.TRISB5=1;
	TRISBbits.TRISB8=1;
	TRISBbits.TRISB9=1;
	TRISBbits.TRISB10=1;
	TRISBbits.TRISB11=1;
	TRISBbits.TRISB12=1;
	TRISBbits.TRISB13=1; //сенсор10
	
	//1
	AD1CON1bits.AD12B=1; //12-bit, 1-channel ADC operation
	//2
	AD1CON2bits.VCFG=0;   //Avdd Avss в качестве опорного напряжения
	//3
	AD1CON3bits.ADRC=0;   //Clock derived from system clock
	AD1CON3bits.ADCS=31;  //5;   //Tad=(ADCS+1)*Tcy . Tad должно быть > 75 nsec
	AD1CON3bits.SAMC=31;  //время семплирования для режима Auto Sample
	//4
	AD1PCFGLbits.PCFG0=0; //RBO - аналоговый вход
	//5
	//AD1CHS0bits.CH0SA=0;  //Channel 0 Sample A positive input is AN0
	AD1CHS0bits.CH0NA=0;  //Channel 0 Sample A Negative Input Vref-=AN1=RB1, похоже актуально для пиков с бОльшим количеством ножек, в нашем случае AN1=Vref- выставляй\невыставляй
	//6
	AD1CON2bits.CHPS=0;   //Converts CH0 only
	AD1CON2bits.ALTS=0;   //Always uses channel input selects for Sample A
	AD1PCFGLbits.PCFG2=0; //AN2=analog input сенсор 4
	AD1PCFGLbits.PCFG3=0; //AN3=analog input сенсор 3 
	AD1PCFGLbits.PCFG4=0; //AN4=analog input сенсор 2
	AD1PCFGLbits.PCFG5=0; //AN5=analog input сенсор 1
	AD1PCFGLbits.PCFG8=0; //AN8=analog input сенсор 5
	AD1PCFGLbits.PCFG9=0; //AN9=analog input сенсор 6
	AD1PCFGLbits.PCFG10=0;//AN10=analog input сенсор 7
	AD1PCFGLbits.PCFG11=0;//AN11=analog input сенсор 8
	AD1PCFGLbits.PCFG12=0;//AN12=analog input сенсор 9
	AD1PCFGLbits.PCFG13=0;//AN13=analog input сенсор 10
	//7
	AD1CON1bits.SIMSAM=0; //одновременное АЦП не используем
	AD1CON2bits.CSCNA=0;  //не используем режим сканирования для CH0 SampleA
	AD1CSSLbits.CSS0=1;   //Select AN0 for input scan, вообщем то ненужно, потому что режим сканирования пока не используем
	//8
	AD1CON1bits.ASAM=0;   //Sampling begins when SAMP bit is set
	//AD1CON1bits.ASAM=1; //Sampling begins immediately after last conversion. SAMP bit is auto-set.
	//9
	AD1CON1bits.SSRC=7;   //Internal counter ends sampling and starts conversion (auto-convert)
	//10
	AD1CON1bits.FORM=0;  //Data Output Format bits = Integer (DOUT= 0000 dddd dddd dddd)
	//11
	AD1CON2bits.SMPI=0;  //Increments the DMA address or generates interrupt after completion of every sample/conversion operation
	AD1CON2bits.BUFM=0;  //Always starts filling buffer at address 0x0. Для DMA?
	//12
	AD1CON4bits.DMABL=0; //Allocates 1 word of buffer to each analog input
	//13
	AD1CON1bits.ADON=1; //ADC module is operating
	
	return;
}


///////////////////////////////////
unsigned short ADC_get(unsigned char sensor_num) //сейчас 1АЦП длится 32мкс
{
	unsigned short result=0;
		 
	switch(sensor_num)
	{
		case 0:
			AD1CHS0bits.CH0SA=5; //PB5
			break;  
		case 1:
			AD1CHS0bits.CH0SA=4; //PB4
			break;  
		case 2:
			AD1CHS0bits.CH0SA=3; //PB3
			break;  
		case 3:
			AD1CHS0bits.CH0SA=2; //PB2      
			break;  
		case 4:
			AD1CHS0bits.CH0SA=8; //PB8
			break;  
		case 5:
			AD1CHS0bits.CH0SA=9; //PB9
			break;  
		case 6:
			AD1CHS0bits.CH0SA=10; //PB10
			break;  
		case 7:
			AD1CHS0bits.CH0SA=11; //PB11
			break;  
		case 8:
			AD1CHS0bits.CH0SA=12; //PB12
			break;  
		case 9:
			AD1CHS0bits.CH0SA=13; //PB13
			break;  
		default:
			break;
	} 
		
		//AD1CON1bits.DONE=0;  //сбрасываем бит перед очередним АЦП преобразованием, хотя он сбрасывается автоматически
		AD1CON1bits.SAMP=1;
		
		while(AD1CON1bits.DONE==0); //ждем окончания АЦП
		
		AD1CON1bits.DONE=0; //вот это обязательно надо делать, иначе считывается результат предыдущей оцифровки
		result=ADC1BUF0;
	
		return result;
}

