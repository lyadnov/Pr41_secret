#include "main\p33FJ128GP706.h"
#include "main\errors.h" 
#include "system\modbus\modbus.h"
#include "system\usart\usart.h"

//#define USART_19200
#define USART_115200
#define USART_9bit

unsigned int stat_usart_error_timout = 0;
unsigned int stat_usart_error_frame = 0;
unsigned int stat_usart_error_parity = 0;
unsigned int stat_usart_error_overrun = 0;

void rs485_send_on(void)
{
	PORTBbits.RB15 = 1; //управляем приёмо-передатчиком: разрешаем запись данных в линию
		 
	//пауза 400мкс, чтобы помехи после включения приёмо-передачтика затухли + стандартная пауза между Modbus пакетами
	T8CON = 0;
	T8CONbits.TCS = 0;            //Timer8 Clock Source Select bit: Internal clock (Fcy=40MHz=Fosc/2=80мгц/2
	T8CONbits.TCKPS = 3;          //1:256  1такт=25*256нс=6.4мкс
	PR8 = USART_MODBUS_PAUSE;     //=400мкс
	TMR8 = 0;
	IFS3bits.T8IF = 0; //сбрасываем флаг
	T8CONbits.TON = 1; //включаем таймер 8
	while (IFS3bits.T8IF == 0);
}


void rs485_send_off(void)
{
	unsigned char data;

	while (U1STAbits.TRMT == 0); //ждем опустошения сдвигового регистра

	PORTBbits.RB15 = 0; //управляем приёмо-передатчиком: запрещаем запись данных в линию

	//вычищаем RX данные, т.к при отправке они заворачиваются, надо просто 4 раза RX считать.
	data = U1RXREG;  //сдвиговый RX буфер для USART, имеет размер 4 байта
	data = U1RXREG;  //поэтому вычитываем 4 байта,которые зеркально прилетают обратно при любых TX транзакциях
	data = U1RXREG;
	data = U1RXREG;
}


static void rs485_init(void)
{
	TRISBbits.TRISB15 = 0; //ножка RB15 на выход
	PORTBbits.RB15 = 0;    //управляем приёмо-передатчиком: запрещаем запись данных в линию
}



void UsartTxByteX(unsigned char data,unsigned char bit9)
{
	unsigned short val;
	while (U1STAbits.UTXBF==1); //ждем окончания отправки предыдущих данных
	
	val = ((unsigned short)data) | (((unsigned short)bit9) << 8);

	U1TXREG = val;
}


///////////////////////////////////
char UsartRxByte(unsigned short *data)
//принимает байт, таймаут 13-26мсек
//возвращает 1, в случае ошибки, 0 если все успешно
{

	while(U1STAbits.URXDA==0);
		
	//проверяю что приняли данные без ошибок
	if(U1STAbits.FERR)                 // If a framing error occured
	{
		*data = U1RXREG; 
		//error_num=err_UsartRxFrameError;
		slave_regs[REG_ERROR_USART_FRAME]++;
		return(1);
	}
	if(U1STAbits.PERR)                 // If a framing error occured
	{
		*data = U1RXREG; 
		//error_num=err_UsartRxParityError;
		 slave_regs[REG_ERROR_USART_PARITY]++; 
		return(1);
	}

	*data = U1RXREG; 

	if(U1STAbits.OERR)                 //переполнение буфера
	{ 
		U1STAbits.OERR=0;
		slave_regs[REG_ERROR_USART_OVERRUN]++; 
		return(1);   
	}

	return (0);                     // Return the received data
}


#if 0
char UsartRxByte_withTimeout(unsigned short *data)  //используется только мастером
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
		//stat_usart_error_frame++;
		slave_regs[REG_ERROR_USART_FRAME]++;
		return(1);
	}
		if(U1STAbits.PERR)                 // If a parity error occured
		{
		*data = U1RXREG; 
		//stat_usart_error_parity++;
		slave_regs[REG_ERROR_USART_PARITY]++; 
		return(1);
	}

	*data = U1RXREG; 

	if(U1STAbits.OERR)                 //переполнение буфера
	{ 
		U1STAbits.OERR=0;
		//stat_usart_error_overrun++;
		slave_regs[REG_ERROR_USART_OVERRUN]++; 
		return(1);   
	}

	return (0);                     // Return the received data
}
#endif

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


void UsartInit(void)
{
	rs485_init();

	//ножки
	TRISFbits.TRISF2 = 1;    //RX
	TRISFbits.TRISF3 = 0;    //TX
	
	//модуль usart
#ifdef USART_19200
	U1MODEbits.BRGH=1;       //High Baud Rate Enable bit
	U1BRG=520;               //19193бит/сек= (40*1000*1000Гц)/(4*(520+1))  
#elif defined USART_115200
	/* USART speed 115200 */
	U1MODEbits.BRGH=1;       //High Baud Rate Enable bit
	U1BRG=86;                //115200бит/сек= (40*1000*1000Гц)/(4*(86+1))  
	//U1MODEbits.BRGH=0;     //High Baud Rate Enable bit
	//U1BRG=21;              //115200бит/сек: 113636бит/сек=(40*1000*1000Гц)/(16*(86+1))  
#else
	Error!
#endif
	
	U1MODEbits.UEN=0;       //TX RX - используем, а всякие CTS RTS - нет    
#ifdef USART_9bit
	U1MODEbits.PDSEL=3;     //9bit no parity
#else
	U1MODEbits.PDSEL=0;     //8bit четность выкл.
#endif
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

