#include "p33FJ128GP706.h"
#include "system\nmb\nmb_timer.h"

void __attribute__((interrupt,auto_psv)) _T1Interrupt(void)
{
	nmb_timer_callback();
	IFS0bits.T1IF = 0;
	return;
}

void __attribute__((interrupt,auto_psv)) _T2Interrupt(void)
{
	nmb_timer2_callback();
	IFS0bits.T2IF = 0;
	return;
}
