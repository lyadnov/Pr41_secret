#include "p33FJ128GP706.h"
#include "system\nmb\nmb_ext.h"

void __attribute__((interrupt,auto_psv)) _T1Interrupt(void)
{
	nmb_timer_callback();
	IFS0bits.T1IF=0;
	return;
}
