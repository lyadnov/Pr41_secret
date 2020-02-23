#ifndef _MODBUS_INCLUDED
#define _MODBUS_INCLUDED


//---------константы---------------
#define USART_MASTER_RX_TIMEOUT        500     //3,2мс=500*6.4мкс(=1такт TMR8 при делителе 1:256) параметр нужен при работе мастера с компом
#define USART_MODBUS_ERROR_PAUSE       47      //300мкс в случае любой ошибки ждем тишины в линии этот интервал времени
#define USART_MODBUS_PAUSE             62      //400мкc выставляем ножку TX в 1 и ждем пока сигнал на линии установиться + пауза между MODBUS-пакетами
#define NUMBER_OF_RESENDS              1       //количество попыток получить данные мастеру от слейва

#define DEFAULT_SLAVE_ADDR              1
#define DEFAULT_START_PAUSE             475    //пауза перед стартом измерений в мс
#define START_PAUSE_MIN_VALUE           1
#define START_PAUSE_MAX_VALUE           999

#define DEFAULT_MEASURE_INTERVAL        10     //интервал между точками, должно быть 1мс..500мс, а у меня 1мс..400мс

#define DEFAULT_K                       14000  //значение по-умолчанию калибровочного коэффициента для датчиков давления
#define K_MIN_VALUE                     12000
#define K_MAX_VALUE                     16000

#define DEFAULT_R                       5500   //значение по-умолчанию калибровочного коэффицента для датчика сопротивления воды
#define R_MIN_VALUE                     4000
#define R_MAX_VALUE                     7000


#define MODBUS_BROADCAST_ADDR           0x00   //по стандарту нулевой адрес=широковещательный

#define NUMBER_OF_MEASUREMENTS          100
#define NUMBER_OF_SENSORS               10     //последний 10ый сенсор измеряет не давление а сопротивление воды.
#define REALTIME_REG_NUM                0x64   //количество регистров значение которых вычисляется в реальном времени (отладочные\текущие значения\параметры)
#define RECORDED_REG_NUM                ((NUMBER_OF_SENSORS-1)*NUMBER_OF_MEASUREMENTS) //количество регистров в которых хранятся записанные АЦП-значения
#define TOTAL_REG_NUM                   (REALTIME_REG_NUM+RECORDED_REG_NUM)

//------адреса регистров modbus-----------
#define REG_MAGIC                       0x00 //тут всегда 0x1234
//управление
#define REG_CONTROL_MEASURE_START       0x01 //при записи в этот регистр 1цы начинаются измерения, по всем слейвам
	#define  CMD_MEASURE_START            0x01
#define REG_CONTROL_SAVE                0x02 //при записи в этот регистр 1-4, сохраняются соответствующие параметры в EEPROM
	#define  CMD_SAVE_K                   0x01
	#define  CMD_SAVE_SLAVE_ADDR          0x02
	#define  CMD_SAVE_START_PAUSE         0x03
	#define  CMD_SAVE_MEASURE_INTERVAL    0x04
	#define  CMD_SAVE_PMODE               0x05
//конфигурация
#define REG_CONFIG_SLAVE_ADDR           0x03
#define REG_CONFIG_START_PAUSE          0x04 //пауза перед стартом измерений в мсек
#define REG_CONFIG_MEASURE_INTERVAL     0x05 //интервал между измерениями, т.е мерием 100 точек между каждой точкой выдерживаем данный интервал времени
#define REG_CONFIG_K1                   0x06            //калибровочный коэффициент для 1го датчика
#define REG_CONFIG_K2                   (REG_CONFIG_K1+1) 
#define REG_CONFIG_K3                   (REG_CONFIG_K1+2) 
#define REG_CONFIG_K4                   (REG_CONFIG_K1+3) 
#define REG_CONFIG_K5                   (REG_CONFIG_K1+4) 
#define REG_CONFIG_K6                   (REG_CONFIG_K1+5) 
#define REG_CONFIG_K7                   (REG_CONFIG_K1+6) 
#define REG_CONFIG_K8                   (REG_CONFIG_K1+7) 
#define REG_CONFIG_K9                   (REG_CONFIG_K1+8) 
#define REG_CONFIG_K10                  (REG_CONFIG_K1+9) //калибровочный коэффициент для 10го датчика

//статистика Modbus
#define REG_STAT_TOTAL_FRAME_NUM        0x10
#define REG_STAT_SLAVE_ADDR_MISMATCH    0x11
#define REG_STAT_SLAVE_FUNC_MISMATCH    0x12
#define REG_STAT_CRC_ERROR              0x13
#define REG_STAT_CRC_OK                 0x14
#define REG_STAT_FULL_DUMP_NUM          0x15   //каждый раз когда запускаю запись измеренных значений, счетчик увеличивается

//статистика USART
#define REG_ERROR_USART_FRAME           0x20
#define REG_ERROR_USART_PARITY          0x21
#define REG_ERROR_USART_OVERRUN         0x22
//текущие значения с датчиков
#define REG_SENSOR1_CURRENT_VALUE       0x30 //датчик давления 1
#define REG_SENSOR2_CURRENT_VALUE       0x31
#define REG_SENSOR3_CURRENT_VALUE       0x32
#define REG_SENSOR4_CURRENT_VALUE       0x33
#define REG_SENSOR5_CURRENT_VALUE       0x34
#define REG_SENSOR6_CURRENT_VALUE       0x35
#define REG_SENSOR7_CURRENT_VALUE       0x36
#define REG_SENSOR8_CURRENT_VALUE       0x37
#define REG_SENSOR9_CURRENT_VALUE       0x38 //датчик давления 9
#define REG_SENSOR10_CURRENT_VALUE      0x39 //датчик сопротивления воды
#define REG_WATER_LEVEL_CURRENT_VALUE   0x3A //датчик уровня воды

//pmode
#define REG_CONFIG_PMODE                0x3B
#define REG_CONFIG_P2_INTERVAL1         0x3C
#define REG_CONFIG_P2_INTERVAL2         (REG_CONFIG_P2_INTERVAL1 + 1)
#define REG_CONFIG_P2_INTERVAL3         (REG_CONFIG_P2_INTERVAL1 + 2)
#define REG_CONFIG_P2_INTERVAL4         (REG_CONFIG_P2_INTERVAL1 + 3)
#define REG_CONFIG_P2_INTERVAL5         (REG_CONFIG_P2_INTERVAL1 + 4)
#define REG_CONFIG_P2_INTERVAL6         (REG_CONFIG_P2_INTERVAL1 + 5)
#define REG_CONFIG_P2_INTERVAL7         (REG_CONFIG_P2_INTERVAL1 + 6)
#define REG_CONFIG_P2_INTERVAL8         (REG_CONFIG_P2_INTERVAL1 + 7)
#define REG_CONFIG_P2_INTERVAL9         (REG_CONFIG_P2_INTERVAL1 + 8)
#define REG_CONFIG_P2_INTERVAL10        (REG_CONFIG_P2_INTERVAL1 + 9)
#define REG_CONFIG_P2_INTERVAL11        (REG_CONFIG_P2_INTERVAL1 + 10)
#define REG_CONFIG_P2_INTERVAL12        (REG_CONFIG_P2_INTERVAL1 + 11)
#define REG_CONFIG_P2_INTERVAL13        (REG_CONFIG_P2_INTERVAL1 + 12)
#define REG_CONFIG_P2_INTERVAL14        (REG_CONFIG_P2_INTERVAL1 + 13)
#define REG_CONFIG_P2_INTERVAL15        (REG_CONFIG_P2_INTERVAL1 + 14)
#define REG_CONFIG_P2_INTERVAL16        (REG_CONFIG_P2_INTERVAL1 + 15)


//цепочка измерений 100 точек на каждом датчике
#define REG_SENSOR1_RECORD_VALUE        REALTIME_REG_NUM
#define REG_SENSOR2_RECORD_VALUE        (REG_SENSOR1_RECORD_VALUE + NUMBER_OF_MEASUREMENTS)
#define REG_SENSOR3_RECORD_VALUE        (REG_SENSOR2_RECORD_VALUE + NUMBER_OF_MEASUREMENTS)
#define REG_SENSOR4_RECORD_VALUE        (REG_SENSOR3_RECORD_VALUE + NUMBER_OF_MEASUREMENTS)
#define REG_SENSOR5_RECORD_VALUE        (REG_SENSOR4_RECORD_VALUE + NUMBER_OF_MEASUREMENTS)
#define REG_SENSOR6_RECORD_VALUE        (REG_SENSOR5_RECORD_VALUE + NUMBER_OF_MEASUREMENTS)
#define REG_SENSOR7_RECORD_VALUE        (REG_SENSOR6_RECORD_VALUE + NUMBER_OF_MEASUREMENTS)
#define REG_SENSOR8_RECORD_VALUE        (REG_SENSOR7_RECORD_VALUE + NUMBER_OF_MEASUREMENTS)
#define REG_SENSOR9_RECORD_VALUE        (REG_SENSOR8_RECORD_VALUE + NUMBER_OF_MEASUREMENTS)



//---------типы--------------------

//---------переменные--------------
extern unsigned char cfg_slave_addr;
extern unsigned short cfg_start_pause;
extern unsigned short cfg_measure_interval;
extern unsigned short cfg_k[];
extern unsigned int slave_regs[];


//----------функции----------------
void modbus_init(void);
void modbus_eeprom_write_default_values(void);
void modbus_main(void);

void modbus_read_reg_func(unsigned int addr, unsigned int num);
void modbus_write_reg_func(unsigned int addr,unsigned int value);
void modbus_start_full_measurement();

#endif //_MODBUS_INCLUDED
