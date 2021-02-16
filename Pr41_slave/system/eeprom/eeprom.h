#ifndef _EEPROM_INCLUDED
#define _EEPROM_INCLUDED

//---------константы---------------
#define FLASH_BLOCK_SIZE             0x400              //в словах: 2слова=3байта   
#define ADDR_FLASH_END               0x015800

#define EEPROM_SIZE                  192                //в байтах 
#define ADDR_EEPROM_START (ADDR_FLASH_END-(EEPROM_SIZE/3)*2)
#define ADDR_BLOCK_WITH_EEPROM_START (ADDR_FLASH_END-FLASH_BLOCK_SIZE)

//адреса в EEPROM
#define ADDR_EEPROM_VERSION          0x00
#define ADDR_EEPROM_SENSOR1_VAL      0x01                //истинное значение сенсора 1, 2 байта
#define ADDR_EEPROM_SENSOR2_VAL      (ADDR_EEPROM_SENSOR1_VAL + 2)
#define ADDR_EEPROM_SENSOR3_VAL      (ADDR_EEPROM_SENSOR1_VAL + 4)
#define ADDR_EEPROM_SENSOR4_VAL      (ADDR_EEPROM_SENSOR1_VAL + 6)
#define ADDR_EEPROM_SENSOR5_VAL      (ADDR_EEPROM_SENSOR1_VAL + 8)
#define ADDR_EEPROM_SENSOR6_VAL      (ADDR_EEPROM_SENSOR1_VAL + 10)
#define ADDR_EEPROM_SENSOR7_VAL      (ADDR_EEPROM_SENSOR1_VAL + 12)
#define ADDR_EEPROM_SENSOR1_ADC      (ADDR_EEPROM_SENSOR1_VAL + 14)  //ADC значение, соответствующее истинному для сенсора 1, 2 байта
#define ADDR_EEPROM_SENSOR2_ADC      (ADDR_EEPROM_SENSOR1_ADC + 2)
#define ADDR_EEPROM_SENSOR3_ADC      (ADDR_EEPROM_SENSOR1_ADC + 4)
#define ADDR_EEPROM_SENSOR4_ADC      (ADDR_EEPROM_SENSOR1_ADC + 6)
#define ADDR_EEPROM_SENSOR5_ADC      (ADDR_EEPROM_SENSOR1_ADC + 8)
#define ADDR_EEPROM_SENSOR6_ADC      (ADDR_EEPROM_SENSOR1_ADC + 10)
#define ADDR_EEPROM_SENSOR7_ADC      (ADDR_EEPROM_SENSOR1_ADC + 12)


//---------переменные--------------

//----------функции----------------
unsigned char eeprom_read(unsigned char addr);
void eeprom_read_buf(unsigned char addr,unsigned char *buf,unsigned char size);
void eeprom_write_byte(unsigned char addr,unsigned char data);
void eeprom_write_word(unsigned char addr,unsigned short data);
void eeprom_write_buf(unsigned char addr,unsigned char *buf,unsigned char size);

#endif

