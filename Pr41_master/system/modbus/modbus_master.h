#ifndef _MODBUS_MASTER_INCLUDED
#define _MODBUS_MASTER_INCLUDED

//---------константы---------------
#define NULL 0

//---------типы--------------------
typedef unsigned char byte;						/* create byte type */
/* master structure */
typedef struct {
	int slave; 											/* number of the slave to call*/
	int function; 										/* modbus function to emit*/
	int address;										/* slave address */
	int length;											/* data length */
} Mbm_frame;


//---------переменные--------------


//----------функции----------------
int modbus_master_read_reg(unsigned char slave_addr, unsigned short reg_addr, unsigned short num, unsigned short *buf_in);
int modbus_master_write_reg(unsigned char slave_addr, unsigned short reg_addr, unsigned short val);
int modbus_master_write_reg_broadcast(unsigned short reg_addr, unsigned short val);


#endif //_MODBUS__MASTER_INCLUDED
