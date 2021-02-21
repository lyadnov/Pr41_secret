#ifndef _NMB_TIMER_INCLUDED
#define _NMB_TIMER_INCLUDED

void nmb_timer2_stop(void);
void nmb_timer2_callback(void);
void nmb_timer2_start(void);

void nmb_timer_stop(void);
void nmb_timer_callback(void);
int nmb_timer_is_active(void);
void nmb_timer_start(int new_mode);

#endif //_NMB_TIMER_INCLUDED
