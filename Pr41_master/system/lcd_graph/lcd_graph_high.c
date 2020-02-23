#include "main\p33FJ128GP706.h"

#include "system\lcd_graph\lcd_graph_low.h"
#include "system\lcd_graph\lcd_graph_font_table.h"

//----------константы--------------
	 
//---------переменные--------------
unsigned char current_line;
unsigned char current_column;
unsigned char current_lcd;


//----------функции----------------
void graph_symbol_out(unsigned char x);
void graph_set_position(unsigned char line,unsigned char column);

//--------------------------основная часть-------------------------------------------


///////////////////////////////////
void graph_symbol_out(unsigned char x)
{
	if(current_column>9)
	{
		//правый ЖК
		if(current_lcd==l) graph_set_position(current_line, current_column);
		graph_data_out(r,font_table[x][0]);
		graph_data_out(r,font_table[x][1]);
		graph_data_out(r,font_table[x][2]);
		graph_data_out(r,font_table[x][3]);
		graph_data_out(r,font_table[x][4]);
		graph_data_out(r,0x00);     //разрыв в 1 пиксел между символами
	}
	else
	{
		//левый ЖК
		graph_data_out(l,font_table[x][0]);
		graph_data_out(l,font_table[x][1]);
		graph_data_out(l,font_table[x][2]);
		graph_data_out(l,font_table[x][3]);
		graph_data_out(l,font_table[x][4]);
		graph_data_out(l,0x00);     //разрыв в 1 пиксел между символами
	}
	current_column++;    
	return;
}


///////////////////////////////////
void graph_set_position(unsigned char line,unsigned char column)
{
	if(column>9)
	{
		//правый ЖК
		graph_cmd_out(r,0xB8+line);     //page(символьная строка) = 0
		graph_cmd_out(r,column*6-60);   //столбец
		current_lcd=r;
	} 
	else
	{
		//левый ЖК
		graph_cmd_out(l,0xB8+line);     //page(символьная строка) = 0
		graph_cmd_out(l,20+column*6);   //столбец
		current_lcd=l;
	}
	
	current_line=line;
	current_column=column;
	return;
}

