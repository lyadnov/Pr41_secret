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
#define ADDR_EEPROM_SLAVE_ADDR       0x05                //адрес слейва 1 байт
#define ADDR_EEPROM_START_PAUSE      0x06                //начальная пауза перед измерениями на всех датчиках 2 байта
#define ADDR_EEPROM_MEASURE_INTERVAL 0x08                //интервал между точками, 2 байта
#define ADDR_EEPROM_K1               0x0A                //К1, 2 байта
#define ADDR_EEPROM_K2               (ADDR_EEPROM_K1+2)
#define ADDR_EEPROM_K3               (ADDR_EEPROM_K1+4)
#define ADDR_EEPROM_K4               (ADDR_EEPROM_K1+6)
#define ADDR_EEPROM_K5               (ADDR_EEPROM_K1+8)
#define ADDR_EEPROM_K6               (ADDR_EEPROM_K1+10)
#define ADDR_EEPROM_K7               (ADDR_EEPROM_K1+12)
#define ADDR_EEPROM_K8               (ADDR_EEPROM_K1+14)
#define ADDR_EEPROM_K9               (ADDR_EEPROM_K1+16)
#define ADDR_EEPROM_K10              (ADDR_EEPROM_K1+18) //К10
#define ADDR_EEPROM_P2_INTERVAL1     0x1E                //1 байт
#define ADDR_EEPROM_P2_INTERVAL2     (ADDR_EEPROM_P2_INTERVAL1 + 1)
#define ADDR_EEPROM_P2_INTERVAL3     (ADDR_EEPROM_P2_INTERVAL1 + 2)
#define ADDR_EEPROM_P2_INTERVAL4     (ADDR_EEPROM_P2_INTERVAL1 + 3)
#define ADDR_EEPROM_P2_INTERVAL5     (ADDR_EEPROM_P2_INTERVAL1 + 4)
#define ADDR_EEPROM_P2_INTERVAL6     (ADDR_EEPROM_P2_INTERVAL1 + 5)
#define ADDR_EEPROM_P2_INTERVAL7     (ADDR_EEPROM_P2_INTERVAL1 + 6)
#define ADDR_EEPROM_P2_INTERVAL8     (ADDR_EEPROM_P2_INTERVAL1 + 7)
#define ADDR_EEPROM_P2_INTERVAL9     (ADDR_EEPROM_P2_INTERVAL1 + 8)
#define ADDR_EEPROM_P2_INTERVAL10    (ADDR_EEPROM_P2_INTERVAL1 + 9)
#define ADDR_EEPROM_P2_INTERVAL11    (ADDR_EEPROM_P2_INTERVAL1 + 10)
#define ADDR_EEPROM_P2_INTERVAL12    (ADDR_EEPROM_P2_INTERVAL1 + 11)
#define ADDR_EEPROM_P2_INTERVAL13    (ADDR_EEPROM_P2_INTERVAL1 + 12)
#define ADDR_EEPROM_P2_INTERVAL14    (ADDR_EEPROM_P2_INTERVAL1 + 13)
#define ADDR_EEPROM_P2_INTERVAL15    (ADDR_EEPROM_P2_INTERVAL1 + 14)
#define ADDR_EEPROM_P2_INTERVAL16    (ADDR_EEPROM_P2_INTERVAL1 + 15)


//---------переменные--------------

//----------функции----------------
unsigned char eeprom_read(unsigned char addr);
void eeprom_read_buf(unsigned char addr,unsigned char *buf,unsigned char size);
void eeprom_write_byte(unsigned char addr,unsigned char data);
void eeprom_write_word(unsigned char addr,unsigned short data);
void eeprom_write_buf(unsigned char addr,unsigned char *buf,unsigned char size);

#endif

