#include "p33FJ128GP706.h"

#include "main\errors.h" 
#include "main\oscillator.h"


//#define CLOCK_EXTERNAL_80MHZ_NOPLL //у мен€ мастер
//#define CLOCK_EXTERNAL_80MHZ_PLL
//#define CLOCK_EXTERNAL_10MHZ_PLL     //у бати на работе, у мен€ слейв
#define CLOCK_EXTERNAL_20MHZ_PLL   //у бати на работе 2
//#define CLOCK_HS_2MHZ_PLL
//#define CLOCK_HS_20MHZ_PLL
//#define CLOCK_HS_20MHZ_NOPLL
//#define CLOCK_INTERNAL_FRC_80MHZ_PLL
//#define CLOCK_INTERNAL_FRC_7MHZ_NOPLL

#if defined CLOCK_EXTERNAL_80MHZ_NOPLL
	//внешн€€ частота без PLL
	_FOSC(OSCIOFNC_OFF & POSCMD_EC & FCKSM_CSECMD)  //OSC2 is clock output, External clock,Clock switching:enabled+clock monitor:disabled
	_FOSCSEL(FNOSC_PRI & IESO_OFF)                  //Primary (XT, HS, EC) oscillator no PLL, Two-speed Oscillator Startup:disabled

#elif defined CLOCK_EXTERNAL_80MHZ_PLL
	//внешн€€ частота 80 мгц +  PLL=1
	_FOSC(OSCIOFNC_OFF & POSCMD_EC & FCKSM_CSECMD)  //OSC2 is clock output, External clockClock switching:enabled+clock monitor:disabled
	_FOSCSEL(FNOSC_FRCPLL & IESO_OFF)               //initial:Fast RC oscillator w/ divide and PLL, Two-speed Oscillator Startup:disabled

#elif defined CLOCK_EXTERNAL_10MHZ_PLL
	//внешн€€ частота, которую через PLL превращаем в 80ћ√÷
	_FOSC(OSCIOFNC_OFF & POSCMD_EC & FCKSM_CSECMD)  //OSC2 is clock output, External clock,Clock switching:enabled+clock monitor:disabled
	_FOSCSEL(FNOSC_FRCPLL & IESO_OFF)               //initial:Fast RC oscillator w/ divide and PLL, Two-speed Oscillator Startup:disabled

#elif defined CLOCK_EXTERNAL_20MHZ_PLL
	//внешн€€ частота, которую через PLL превращаем в 80ћ√÷
	_FOSC(OSCIOFNC_OFF & POSCMD_EC & FCKSM_CSECMD)  //OSC2 is clock output, primary:External clock,Clock switching:enabled+clock monitor:disabled
	_FOSCSEL(FNOSC_FRCPLL & IESO_OFF)               ///initial:Fast RC oscillator w/ divide and PLL, Two-speed Oscillator Startup:disabled

#elif defined CLOCK_HS_2MHZ_PLL
	//2ћ√÷ c PLL
	_FOSC(OSCIOFNC_OFF & POSCMD_HS & FCKSM_CSECMD)  //OSC2 is clock O/P, primary:HS oscillator,Clock switching:enabled+clock monitor:disabled
	_FOSCSEL(FNOSC_FRCPLL & IESO_OFF)               //initial:Fast RC oscillator w/ divide and PLL, Two-speed Oscillator Startup:disabled

#elif defined CLOCK_HS_20MHZ_PLL
	//20ћ√÷ c PLL
	_FOSC(OSCIOFNC_OFF & POSCMD_HS & FCKSM_CSECMD)  //OSC2 is clock O/P, primary:HS oscillator,Clock switching:enabled+clock monitor:disabled
	_FOSCSEL(FNOSC_FRCPLL & IESO_OFF)               //initial:Fast RC oscillator w/ divide and PLL, Two-speed Oscillator Startup:disabled

#elif defined CLOCK_HS_20MHZ_NOPLL
	//20ћ√ц HS без PLL
	_FOSC(OSCIOFNC_OFF & POSCMD_HS & FCKSM_CSECMD)  //OSC2 is clock O/P, HS oscillator,Clock switching:disabled+clock monitor:disabled
	_FOSCSEL(FNOSC_PRI & IESO_OFF)                  //initial:=primary,Two-speed Oscillator Startup:disabled

#elif defined CLOCK_INTERNAL_FRC_80MHZ_PLL
	//80ћгц из FRC + PLL
	_FOSC(OSCIOFNC_OFF & POSCMD_NONE & FCKSM_CSECMD) //OSC2 is clock O/P, Primary oscillator disabled,Clock switching:enabled+clock monitor:disabled
	_FOSCSEL(FNOSC_FRCPLL & IESO_OFF)                //initial:Fast RC oscillator w/ divide and PLL,Two-speed Oscillator Startup:disabled

#elif defined CLOCK_INTERNAL_FRC_7MHZ_NOPLL
	//7,37ћ√ц FRC(встроенных) без PLL
	_FOSC(POSCMD_NONE)
	_FOSCSEL(FNOSC_FRC & IESO_OFF)
#endif


_FWDT(FWDTEN_OFF)               //Watchdog Timer: disabled       
_FPOR(FPWRT_PWR64)              // Power-on Reset Value: 64ms

//---------константы---------------
#define OSCCONH_UNLOCK_CODE1    0x78    //First byte in OSCCONH unlock sequence
#define OSCCONH_UNLOCK_CODE2    0x9A    //Second byte in OSCCONH unlock sequence
#define OSCCONL_UNLOCK_CODE1    0x46    //First byte in OSCCONL unlock sequence
#define OSCCONL_UNLOCK_CODE2    0x57    //Second byte in OSCCONL unlock sequence



//--------------------------основна€ часть-------------------------------------------

void oscillator_unlock_write_OSCCONH(unsigned char val)
{
	int current_cpu_ipl;                            /* Declare temp variable for storing CPU IPL */   
	char unlock1, unlock2, *ptr;

	unlock1 = OSCCONH_UNLOCK_CODE1;
	unlock2 = OSCCONH_UNLOCK_CODE2;
	ptr = (char *)&OSCCON + 1;
 
	SET_AND_SAVE_CPU_IPL(current_cpu_ipl, 7);       /* Disable interrupts for unlock sequence below */
	
	asm volatile ("mov.b %1,[%0] \n"
	              "mov.b %2,[%0] \n"
	              "mov.b %3,[%0] \n" : /* no outputs */ : "r"(ptr), "r"(unlock1), "r"(unlock2), "r"(val)); 
	RESTORE_CPU_IPL(current_cpu_ipl);               /* Restore CPU IPL value after executing unlock sequence */ 
}


void oscillator_unlock_write_OSCCONL(unsigned char val)
{
	int current_cpu_ipl;                            /* Declare temp variable for storing CPU IPL */   
	char unlock1, unlock2, *ptr;

	unlock1 = OSCCONL_UNLOCK_CODE1;                       /* Load OSCCONL unlock sequence parameters */
	unlock2 = OSCCONL_UNLOCK_CODE2;
	ptr = (char *)&OSCCON;
	
 
	SET_AND_SAVE_CPU_IPL(current_cpu_ipl, 7);       /* Disable interrupts for unlock sequence below */
	
	/* Perform OSCCONL unlock sequence and request an Oscillator switch by setting OSWEN bit */
	asm volatile ("mov.b %1,[%0] \n"
	              "mov.b %2,[%0] \n"
	              "mov.b %3,[%0] \n" : /* no outputs */ : "r"(ptr), "r"(unlock1), "r"(unlock2),"r"(val));
	RESTORE_CPU_IPL(current_cpu_ipl);               /* Restore CPU IPL value after executing unlock sequence */ 
}


///////////////////////////////////
void oscillator_init(void)
{
	
#if defined CLOCK_EXTERNAL_80MHZ_NOPLL
	//ничего не делаем

#elif defined CLOCK_EXTERNAL_80MHZ_PLL
	//конфигурируем Fosc=((80мгц/PLLPRE)*PLLDIV)/PLLPOST=80ћ√ц
	oscillator_unlock_write_OSCCONH(0x03); //Primary oscillator (XT, HS, EC) with PLL 
	CLKDIVbits.PLLPRE=0;    //PLLPRE=2;
	CLKDIVbits.PLLPOST=0;   //PLLPOST=2;   
	PLLFBDbits.PLLDIV=2;    //PLLDIV=4.
	oscillator_unlock_write_OSCCONL(OSCCON | 0x01); //OSCCONbits.OSWEN=1 = initiate the oscillator switch
	while (OSCCONbits.OSWEN);  //Wait until OSWEN has been cleared by hardware
	
#elif defined CLOCK_EXTERNAL_10MHZ_PLL
	//конфигурируем Fosc=((10мгц/PLLPRE)*PLLDIV)/PLLPOST=80ћ√ц  
	oscillator_unlock_write_OSCCONH(0x03); //Primary oscillator (XT, HS, EC) with PLL 
	CLKDIVbits.PLLPRE=0;    //PLLPRE=2;
	CLKDIVbits.PLLPOST=0;   //PLLPOST=2;   
	PLLFBDbits.PLLDIV=30;   //PLLDIV=32.
	oscillator_unlock_write_OSCCONL(OSCCON | 0x01); //OSCCONbits.OSWEN=1 = initiate the oscillator switch
	while (OSCCONbits.OSWEN);  //Wait until OSWEN has been cleared by hardware

#elif defined CLOCK_EXTERNAL_20MHZ_PLL
	//конфигурируем Fosc=((20мгц/PLLPRE)*PLLDIV)/PLLPOST=80ћ√ц
	oscillator_unlock_write_OSCCONH(0x03); //Primary oscillator (XT, HS, EC) with PLL 
	CLKDIVbits.PLLPRE=0;    //PLLPRE=2;
	CLKDIVbits.PLLPOST=0;   //PLLPOST=2;   
	PLLFBDbits.PLLDIV=14;   //PLLDIV=16.
	oscillator_unlock_write_OSCCONL(OSCCON | 0x01); //OSCCONbits.OSWEN=1 = initiate the oscillator switch
	while (OSCCONbits.OSWEN);  //Wait until OSWEN has been cleared by hardware
	
#elif defined CLOCK_HS_2MHZ_PLL
	//конфигурируем Fosc=((2мгц/PLLPRE)*PLLDIV)/PLLPOST=80ћ√ц
	oscillator_unlock_write_OSCCONH(0x03); //Primary oscillator (XT, HS, EC) with PLL 
	CLKDIVbits.PLLPRE=0;    //PLLPRE=2;
	CLKDIVbits.PLLPOST=0;   //PLLPOST=2;   
	PLLFBDbits.PLLDIV= 158; //правильное значение 158=>80ћгц; 198=>100ћ√ц; 210 работало, 220 уже нет.
	oscillator_unlock_write_OSCCONL(OSCCON | 0x01); //OSCCONbits.OSWEN=1 = initiate the oscillator switch
	while (OSCCONbits.OSWEN);  //Wait until OSWEN has been cleared by hardware

#elif defined CLOCK_HS_20MHZ_PLL
	//конфигурируем Fosc=((20мгц/PLLPRE)*PLLDIV)/PLLPOST=80ћ√ц
	oscillator_unlock_write_OSCCONH(0x03); //Primary oscillator (XT, HS, EC) with PLL 
	CLKDIVbits.PLLPRE=0;    //PLLPRE=2;
	CLKDIVbits.PLLPOST=0;   //PLLPOST=2;   
	PLLFBDbits.PLLDIV=14;   //PLLDIV=16.
	oscillator_unlock_write_OSCCONL(OSCCON | 0x01); //OSCCONbits.OSWEN=1 = initiate the oscillator switch
	while (OSCCONbits.OSWEN);  //Wait until OSWEN has been cleared by hardware
	
#elif defined CLOCK_HS_20MHZ_NOPLL
	//ничего не делаем, все работает на частоте 20MHZ, usart и жк-экран отвал€тс€ без дополнительного конфигурировани€
	
#elif defined CLOCK_INTERNAL_FRC_80MHZ_PLL
		//конфигурируем Fosc=((7,37мгц/PLLPRE)*PLLDIV)/PLLPOST=80ћ√ц
	oscillator_unlock_write_OSCCONH(0x01); //Fast RC oscillator (FRC) with PLL
	CLKDIVbits.PLLPRE=0;    //PLLPRE=2;
	CLKDIVbits.PLLPOST=0;   //PLLPOST=2;   
	PLLFBDbits.PLLDIV=42; //PLLDIV=44 => 80,4ћ√÷; PLLDIV=41 => 79,5 ћ√÷; вообще работало при PLLDIV=60 => 115ћ√ц, дальше на разгон€л   
	oscillator_unlock_write_OSCCONL(OSCCON | 0x01); //OSCCONbits.OSWEN=1 = initiate the oscillator switch
	while (OSCCONbits.OSWEN);  //Wait until OSWEN has been cleared by hardware

#elif defined CLOCK_INTERNAL_FRC_7MHZ_NOPLL
	//ничего не делаем, все работает на частоте 7MHZ, usart и жк-экран отвал€тс€ без дополнительного конфигурировани€

#endif

	return;
}



