/*
 * LCD.c
 *
 * Created: 31.01.2014 12:15:23
 *  Author: lv
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

#include "LCD.h"

#define LCD_N_COLS	16
#define LCD_N_ROWS	2

//unsigned char display_array[LCD_N_ROWS][LCD_N_COLS];

/*
//Unnecessary if stdbool.h is included:
typedef unsigned char bool;

#define TRUE 1
#define FALSE 0
*/

#define LCD_DATAPORT PORTD
#define LCD_DATAPIN PIND
#define LCD_DATAPORTDDR DDRD
#define LCD_CTRLPORT PORTC
#define LCD_CTRLPORTDDR DDRC

#define LCD_CTRLEPIN 2

#define SET_DATA(x) LCD_DATAPORT = x

#define LCD_DPSETOUT LCD_DATAPORTDDR = 0xFF
#define LCD_DPSETIN LCD_DATAPORTDDR = 0x00
#define LCD_CPSETOUT LCD_CTRLPORTDDR = 0x07  /// 0xFF	// ”“Œ◊Õ»“‹ œ»Õ€!

#define LCD_SET_E1 LCD_CTRLPORT |= (1<<LCD_CTRLEPIN)
#define LCD_SET_E0 LCD_CTRLPORT &= ~(1<<LCD_CTRLEPIN)

#define BUSY_FLAG			7

#define CMD_CLEAR_DISPLAY	0
#define CMD_RETURN_HOME		1
#define CMD_ENTRY_MODE_SET	2
#define CMD_DISPLAY_ON_OFF	3
#define CMD_C_D_SHIFT		4
#define CMD_FUNCTION_SET	5
#define CMD_SET_CGRAM_ADDR	6
#define CMD_SET_DDRAM_ADDR	7

// Args for CMD_ENTRY_MODE_SET
#define S_SHIFT_ENT_DISPL	0
#define I_D_DIR				1

// Args for CMD_DISPLAY_ON_OFF
#define ENTIRE_DISPLAY_ON	2
#define CURSOR_POSITION_ON	1
#define CURSOR_ON			0

// Args for CMD_C_D_SHIFT
#define SHIFT_DISPL			3 // set 1 for display shift, 0 for cursor shift
#define RIGHT_DIR			2 // set 1 for right, 0 for left

// Args for FUNCTION_SET
#define IF_8BIT				4 // set 1 for 8 bit, 0 for 4 bit
#define TWO_LINES			3 // set 1 for two lines, 0 for one line
#define BIG_FONT			2 // set 1 for 5x11, 0 for 5x8

enum {IRwrite = 0, Read_busy, Write_data, Read_data};
	
void set_mode(uint8_t mode) {
	LCD_CTRLPORT = (LCD_CTRLPORT & 0b11111100) | mode;
}
//-----------------------------------------------------------------

void strobe(){
	LCD_SET_E1;		// E -> 1
	_delay_us(1);
	LCD_SET_E0;		// E -> 0, strobe here
	_delay_us(1);
	LCD_SET_E1;
//	_delay_us(2);
}
//-----------------------------------------------------------------

bool dev_is_busy(void)
{
	set_mode(Read_busy);
	LCD_DPSETIN;
	strobe();
	return LCD_DATAPIN & (1<<BUSY_FLAG);
//return 0; // FOR DEBUG
}
//-----------------------------------------------------------------

void lcd_send_cmd(uint8_t cmd, uint8_t arg, bool need_wait_busy) // cmd is bit number!
{
	if (need_wait_busy) {
		if (dev_is_busy()){
			PORTC |= (1 << 4);	// For debug with oscilloscope, PORTC.4
		}
		while (dev_is_busy()) {};	// Waiting for busy ends
		PORTC &= ~(1 << 4);		// For debug with oscilloscope, PORTC.4
	}	
		
		
	set_mode(IRwrite);
	LCD_DPSETOUT;
	LCD_DATAPORT = (1<<cmd) | arg;
	strobe();

	if (need_wait_busy) {
		while (dev_is_busy()) {};	// Waiting for busy ends
	}	

	return;
}
//-----------------------------------------------------------------

void lcd_write_char(unsigned char char_to_write) {
//while (dev_is_busy()) {};	// Waiting for busy ends
//_delay_us(50);
set_mode(Write_data);
LCD_DPSETOUT;
SET_DATA(char_to_write);
//_delay_us(5);
strobe();
while (dev_is_busy()) {};	// Waiting for busy ends
}
//-----------------------------------------------------------------

void lcd_write_hex(unsigned char hex_to_write) {
	
	unsigned char H = (hex_to_write & 0xF0) >> 4;
	H = (H < 0x0A? H + 0x30: H + 0x37);
	lcd_write_char(H);
	
	H = hex_to_write & 0x0F;
	H = (H < 0x0A? H + 0x30: H + 0x37);
	lcd_write_char(H);
	
}
//-----------------------------------------------------------------

/* fill buffer and LCD with pattern */
void lcd_fill(uint8_t pos, uint8_t qty, const unsigned char pattern)
{
	unsigned char col;
	
	lcd_send_cmd(CMD_SET_DDRAM_ADDR, pos, 1);

	for (col=0; col < qty; col++) {
		lcd_write_char(pattern);
	}		
}
//-----------------------------------------------------------------

/* Send string to LCD*/
void lcd_send_string(uint8_t row, uint8_t col, char *STR)
{
	lcd_send_cmd(CMD_SET_DDRAM_ADDR, 0x40*(row - 1) + col, 1);

	uint8_t i=0;
	
	do {
		//C = STR[i];
		lcd_write_char(STR[i]);
		i++;
	} while (STR[i] != 0);
}
//-----------------------------------------------------------------

void lcd_move_to(uint8_t row, uint8_t col) {
	lcd_send_cmd(CMD_SET_DDRAM_ADDR, 0x40*(row - 1) + col, 1);
}
//-----------------------------------------------------------------

void lcd_clear_display(void) {
	lcd_send_cmd(CMD_CLEAR_DISPLAY, 0, 1);
}
//-----------------------------------------------------------------

void lcd_init(void)
{
//*/	_delay_ms(50);

	LCD_DPSETOUT;
	LCD_CPSETOUT;
	
	lcd_send_cmd(CMD_FUNCTION_SET, (1<<IF_8BIT) | (1<<TWO_LINES) | (1<<BIG_FONT), 0);		//CMD_FUNCTION_SET - bit 5
//	_delay_us(38);
	_delay_ms(5);

	lcd_send_cmd(CMD_FUNCTION_SET, (1<<IF_8BIT) | (1<<TWO_LINES) | (1<<BIG_FONT), 0);		//CMD_FUNCTION_SET - bit 5
	_delay_us(120);

	lcd_send_cmd(CMD_FUNCTION_SET, (1<<IF_8BIT) | (1<<TWO_LINES) | (1<<BIG_FONT), 0);		//CMD_FUNCTION_SET - bit 5
	_delay_us(38);


	lcd_send_cmd(CMD_DISPLAY_ON_OFF, (0<<ENTIRE_DISPLAY_ON), 1);	//CMD_DISPLAY_ON_OFF is bit 3
	lcd_send_cmd(CMD_DISPLAY_ON_OFF, (1<<ENTIRE_DISPLAY_ON), 1);	//CMD_DISPLAY_ON_OFF is bit 3

	lcd_send_cmd(CMD_CLEAR_DISPLAY, 0, 1);

	lcd_send_cmd(CMD_ENTRY_MODE_SET, (0<<S_SHIFT_ENT_DISPL) | (1<<I_D_DIR), 1);	//CMD_ENTRY_MODE_SET - bit 2
																			//S_SHIFT_ENT_DISPL is 0 -> cursor moves, not display


//	lcd_send_cmd(CMD_C_D_SHIFT, (0<<SHIFT_DISPL) | (1<<RIGHT_DIR), 1);		//CMD_C_D_SHIFT - bit 4
																		//SHIFT_DISPL is 0 -> cursor is shifting
/*																		
	unsigned char cgCH[8];
	
	//"ﬂ"
	cgCH[0] = 0b00001111; //0x0F;
	cgCH[1] = 0b00010001; //0x11;
	cgCH[2] = 0b00010001; //0x11;
	cgCH[3] = 0b00001111; //0x0F;
	cgCH[4] = 0b00000101; //0x05;
	cgCH[5] = 0b00001001; //0x09;
	cgCH[6] = 0b00010001; //0x11;
	cgCH[7] = 0b00000000; //0x00;
	lcd_create_cg(0x10, cgCH);

	//"ÿ"
	cgCH[0] = 0b00010101; //0x15;
	cgCH[1] = 0b00010101; //0x15;
	cgCH[2] = 0b00010101; //0x15;
	cgCH[3] = 0b00010101; //0x15;
	cgCH[4] = 0b00010101; //0x15;
	cgCH[5] = 0b00010101; //0x15;
	cgCH[6] = 0b00011111; //0x1F;
	cgCH[7] = 0b00000000; //0x00;
	lcd_create_cg(0x11, cgCH);

	//"Ÿ"
	cgCH[0] = 0b00010101; //0x15;
	cgCH[1] = 0b00010101; //0x15;
	cgCH[2] = 0b00010101; //0x15;
	cgCH[3] = 0b00010101; //0x15;
	cgCH[4] = 0b00010101; //0x15;
	cgCH[5] = 0b00010101; //0x15;
	cgCH[6] = 0b00011111; //0x1F;
	cgCH[7] = 0b00000001; //0x01;
	lcd_create_cg(0x12, cgCH);
	lcd_create_cg(0x13, cgCH);
	lcd_create_cg(0x14, cgCH);
	lcd_create_cg(0x15, cgCH);
	lcd_create_cg(0x16, cgCH);
	lcd_create_cg(0x17, cgCH);
*/
}
//-----------------------------------------------------------------

void lcd_create_cg(uint8_t charno, unsigned char *LNS) {
	
	lcd_send_cmd(CMD_SET_CGRAM_ADDR, 8*charno, 1);
	
	for (int i=0; i<8; i++) {
		lcd_write_char(*(LNS+i));
	}
}
//-----------------------------------------------------------------
