#ifndef _NMB_HIGH_INCLUDED
#define _NMB_HIGH_INCLUDED

extern int nmb_mode;
extern int nmb_error;

int nmb_process_data(unsigned char *data_in, unsigned char size_in, unsigned char *data_out, unsigned char *size_out);

#endif //_NMB_HIGH_INCLUDED
