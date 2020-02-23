#include "main\p33FJ128GP706.h"
#include "main\errors.h" 
#include "system\modbus\modbus.h"
#include "system\usart\usart.h"

//----------константы-------------- 

//---------переменные--------------
unsigned int stat_usart_error_timout=0;
unsigned int stat_usart_error_frame=0;
unsigned int stat_usart_error_parity=0;
unsigned int stat_usart_error_overrun=0;


//----------функции----------------

//--------------------------основная часть-------------------------------------------


///////////////////////////////////
void UsartInit(void)
{
	//ножки
	TRISFbits.TRISF2=1;     //RX
	TRISFbits.TRISF3=0;     //TX
	
	TRISBbits.TRISB15=0;    //управляем выходом 1=TX 0=RX

			
	//модуль usart
	U1MODEbits.BRGH=1;      //High Baud Rate Enable bit
	U1BRG=86;                //115200кб/сек= (40*1000*1000Гц)/(4*(86+1))  
	//U1MODEbits.BRGH=0;         //High Baud Rate Enable bit
	//U1BRG=21;                //115200кб/сек: 113636бит/сек=(40*1000*1000Гц)/(16*(86+1))  

	U1MODEbits.UEN=0;       //TX RX - используем, а всякие CTS RTS - нет    
	U1MODEbits.PDSEL=0;     //8bit четность выкл.
	U1MODEbits.STSEL=0;     //1 стоп бит
	U1MODEbits.URXINV=0;    //UxRX Idle state is ‘1’
		 
	//прерывания
	U1STAbits.URXISEL=0;    //прерывание по каждому пришедшему байту
	U1STAbits.UTXISEL0=0;    //?
	U1STAbits.UTXISEL1=0;
	IPC2bits.U1RXIP=4;  //приоритет прерывания =4
	IPC3bits.U1TXIP=4;  //приоритет прерывания =4
	IPC16bits.U1EIP=4;  //приоритет прерывания =4
	IEC0bits.U1TXIE=0;  //прерывания на передачу запрещены
	IEC0bits.U1RXIE=0;  //прерывания на прием запрещены
	IEC4bits.U1EIE=0;   //прерывания по ошибке запрещены
		
	U1MODEbits.UARTEN=1;    //UART1 is enabled
	U1STAbits.UTXEN=1;      //Transmit Enable

	IFS0bits.U1TXIF=0;  //на всякий случай сбрасываем флаг прерывания      
	IFS0bits.U1RXIF=0;
	IFS4bits.U1EIF=0;
	
	 return;
}


/////////////////////////////////////
void UsartTxByteX(char data)
{

	while(U1STAbits.UTXBF==1); //ждем окончания отправки предыдущих данных
	U1TXREG = data;
 
}


///////////////////////////////////
char UsartRxByte(unsigned char *data)
//принимает байт, таймаут 13-26мсек
//возвращает 1, в случае ошибки, 0 если все успешно
{

	while(U1STAbits.URXDA==0);
		
	//проверяю что приняли данные без ошибок
	if(U1STAbits.FERR)                 // If a framing error occured
	{
		*data = U1RXREG; 
		//error_num=err_UsartRxFrameError;
		//slave_regs[REG_ERROR_USART_FRAME]++;
		return(1);
	}
	if(U1STAbits.PERR)                 // If a framing error occured
	{
		*data = U1RXREG; 
		//error_num=err_UsartRxParityError;
		 //slave_regs[REG_ERROR_USART_PARITY]++; 
		return(1);
	}

	*data = U1RXREG; 

	if(U1STAbits.OERR)                 //переполнение буфера
	{ 
		U1STAbits.OERR=0;
		 //slave_regs[REG_ERROR_USART_OVERRUN]++; 
		return(1);   
	}

	return (0);                     // Return the received data
}



///////////////////////////////////
char UsartRxByte_withTimeout(unsigned char *data)  //используется только мастером
//принимает байт, таймаут 13-26мсек
//возвращает 1, в случае ошибки, 0 если все успешно
{
	T8CON=0;
	T8CONbits.TCS=0;        //Timer8 Clock Source Select bit: Internal clock (Fcy=40MHz=Fosc/2=80мгц/2
	T8CONbits.TCKPS=3;      //1:256  1такт=25*256нс=6.4мкс
	PR8=USART_MASTER_RX_TIMEOUT;   //=3.2мс сейчас
	do
	{
		TMR8=0;
		IFS3bits.T8IF=0; //сбрасываем флаг
		T8CONbits.TON=1; //включаем таймер 8
		while( (U1STAbits.URXDA==0)&&(IFS3bits.T8IF==0) );

		if(U1STAbits.URXDA==0)
		{
			//выходим по таймауту
			T8CONbits.TON=0; //выключаем таймер 8
			stat_usart_error_timout++;
			return(1); //ошибка таймаут
		}
		else
		{
			T8CONbits.TON=0; //выключаем таймер 8
			break;
		}
	}while(1);

	//проверяю что приняли данные без ошибок
	if(U1STAbits.FERR)                 // If a framing error occured
	{
		*data = U1RXREG; 
		stat_usart_error_frame++;
		return(1);
	}
		if(U1STAbits.PERR)                 // If a parity error occured
		{
		*data = U1RXREG; 
		stat_usart_error_parity++;
		return(1);
	}

	*data = U1RXREG; 

	if(U1STAbits.OERR)                 //переполнение буфера
	{ 
		U1STAbits.OERR=0;
		stat_usart_error_overrun++;
		return(1);   
	}

	return (0);                     // Return the received data
}


///////////////////////////////////
void UsartWaitForSilence(void)
//функция ждет пока в линни не наступит тишина длительностью 3.5символа
//3.5символа= ( 1сек / ((115200бит_сек/10бит_в_байте)) )*3.5 = 300мкс
//внешний тактовый генератор 80Мгц  на TMR8 подается 80МГЦ/2 + делитель 256. 
// Т.е увеличение TMR8 на 1 происходит раз в (1/80Мгц)*256= раз в 6,4мкс
//поэтому после включения таймера пройдет период 300мкс/6.4мкс=47 и выставится флаг IFS3bits.T8IF
{
	unsigned char data;
		
	
	data=U1RXREG;  //сдвиговый RX буфер для USART, имеет размер 4 байта
	data=U1RXREG;  //поэтому вычитываем 4 байта,которые зеркально прилетают обратно при любых TX транзакциях
	data=U1RXREG;
	data=U1RXREG;

	T8CON=0;
	T8CONbits.TCS=0;            //Timer8 Clock Source Select bit: Internal clock (Fcy=40MHz=Fosc/2=80мгц/2
	T8CONbits.TCKPS=3;          //1:256  1такт=25*256нс=6.4мкс
	PR8=USART_MODBUS_ERROR_PAUSE;  //=300мкс

	do
	{
		TMR8=0;
		IFS3bits.T8IF=0; //сбрасываем флаг
		T8CONbits.TON=1; //включаем таймер 8
		while( (U1STAbits.URXDA==0)&&(IFS3bits.T8IF==0) );
				
		if(U1STAbits.URXDA==0)
		{
			//нужная пауза без данных произошла
			T8CONbits.TON=0; //выключаем таймер 8
			return;
		}
		else
		{
			//данные пришли, а нужную паузу не выдержали - запускаем счетчик заново
			data=U1RXREG;
			if(U1STAbits.OERR) U1STAbits.OERR=0; //переполнение буфера
		}
	}while(1);    

}

