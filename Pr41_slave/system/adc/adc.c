#include "p33FJ128GP706.h"

#include "main\errors.h" 
#include "system\adc\adc.h"


void ADC_Init(void)
{
	//ножки PORTB конфигурируем на вход
	TRISBbits.TRISB1 = 1;  //Vref-
	TRISBbits.TRISB5 = 1;  //Температура радиатора БТР
	TRISBbits.TRISB8 = 1;  //Давление внутри НЭ
	TRISBbits.TRISB9 = 1;  //Питание процессора 3,3В
	TRISBbits.TRISB10 = 1; //Влажность внутри НЭ
	TRISBbits.TRISB11 = 1; //Температура внутри НЭ
	TRISBbits.TRISB12 = 1; //Температура радиатора БЗ
	TRISBbits.TRISB13 = 1; //Напряжение БИ
	
	//1
	AD1CON1bits.AD12B = 1; //12-bit, 1-channel ADC operation
	//2
	AD1CON2bits.VCFG = 0;   //Avdd Avss в качестве опорного напряжения
	//3
	AD1CON3bits.ADRC = 0;   //Clock derived from system clock
	AD1CON3bits.ADCS = 31;  //5;   //Tad=(ADCS+1)*Tcy . Tad должно быть > 75 nsec
	AD1CON3bits.SAMC = 31;  //время семплирования для режима Auto Sample
	//4
	AD1PCFGLbits.PCFG0 = 0; //RBO - аналоговый вход
	//5
	//AD1CHS0bits.CH0SA = 0;  //Channel 0 Sample A positive input is AN0
	AD1CHS0bits.CH0NA = 0;  //Channel 0 Sample A Negative Input Vref-=AN1=RB1, похоже актуально для пиков с бОльшим количеством ножек, в нашем случае AN1=Vref- выставляй\невыставляй
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
	AD1CON1bits.SIMSAM = 0; //одновременное АЦП не используем
	AD1CON2bits.CSCNA = 0;  //не используем режим сканирования для CH0 SampleA
	AD1CSSLbits.CSS0 = 1;   //Select AN0 for input scan, вообщем то ненужно, потому что режим сканирования пока не используем
	//8
	AD1CON1bits.ASAM = 0;   //Sampling begins when SAMP bit is set
	//AD1CON1bits.ASAM=1; //Sampling begins immediately after last conversion. SAMP bit is auto-set.
	//9
	AD1CON1bits.SSRC = 7;   //Internal counter ends sampling and starts conversion (auto-convert)
	//10
	AD1CON1bits.FORM = 0;  //Data Output Format bits = Integer (DOUT= 0000 dddd dddd dddd)
	//11
	AD1CON2bits.SMPI = 0;  //Increments the DMA address or generates interrupt after completion of every sample/conversion operation
	AD1CON2bits.BUFM = 0;  //Always starts filling buffer at address 0x0. Для DMA?
	//12
	AD1CON4bits.DMABL = 0; //Allocates 1 word of buffer to each analog input
	//13
	AD1CON1bits.ADON = 1; //ADC module is operating
	
	return;
}


unsigned short ADC_get(unsigned char sensor_num) //сейчас 1АЦП длится 32мкс
{
	unsigned short result = 0;
		 
	switch(sensor_num)
	{
		case 0:
			AD1CHS0bits.CH0SA = 13; //Напряжение БИ
			break;  
		case 1:
			AD1CHS0bits.CH0SA = 12; //Температура радиатора БЗ
			break;  
		case 2:
			AD1CHS0bits.CH0SA = 5; //Температура радиатора БТР
			break;  
		case 3:
			AD1CHS0bits.CH0SA = 11; //Температура внутри НЭ
			break;  
		case 4:
			AD1CHS0bits.CH0SA = 10; //Влажность внутри НЭ
			break;  
		case 5:
			AD1CHS0bits.CH0SA = 8; //Давление внутри НЭ
			break;  
		case 6:
			AD1CHS0bits.CH0SA = 9; //Питание процессора 3,3В
			break;  
		default:
			return 0;
	} 
		
		//AD1CON1bits.DONE = 0;  //сбрасываем бит перед очередним АЦП преобразованием, хотя он сбрасывается автоматически
		AD1CON1bits.SAMP = 1;
		
		while (AD1CON1bits.DONE == 0); //ждем окончания АЦП
		
		AD1CON1bits.DONE = 0; //вот это обязательно надо делать, иначе считывается результат предыдущей оцифровки
		result = ADC1BUF0;
	
		return result;
}

