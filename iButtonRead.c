/*
 * iButtonRead.c
 *
 * Created: 01.04.2014 3:40:16
 *  Author: lv
 */ 

#define STOP while (1) {}

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

#include "1wire.h"
#include "LCD.h"

#define READ_ROM 0x33
#define SKIP_ROM 0xCC
#define MATCH_ROM 0x55
#define SEARCH_ROM 0xF0
#define READ_MEMORY 0xF0

//-----------------------------------------------------------------
/******-----------------**************/
/******--- MAIN --------**************/
/******-----------------**************/

uint8_t Done_Flag;
char DeviceROM[64];

int main(void)
{

//testCRC();
//STOP
	lcd_init();
//	testStoreBits();
	while(1) {
		lcd_clear_display();
		lcd_send_string(1, 0, "SCANING DEVS...\0");
	
		OW_init();

//	while (!OW_check_presence()) {}

//OW_check_presence();

//	_delay_ms(1);
		char Device[10][8];
		uint8_t DevNo = 0;

		Done_Flag = 0;
		while (!Done_Flag) {
			if (Scan_OW_devices(DevNo? false: true)) {
				for (uint8_t i = 0; i < 8; i++) {
					Device[DevNo][i] = DeviceROM[i];
				}
				DevNo++;
			}
			else DevNo = 0;		
		}	

		lcd_send_string(1, 0, "Found    devs\0");
		lcd_move_to(1, 6);
		lcd_write_hex(DevNo);
			
		_delay_ms(500);
		
		lcd_send_string(1, 0, "Device          \0");
		for (uint8_t i = 0; i < DevNo; i++) {
			lcd_move_to(1, 7);
			lcd_write_hex(i + 1);
				
			lcd_move_to(2, 0);

			for (uint8_t j = 0; j < 8; j++)	{
					lcd_write_hex(Device[i][j]);		//device serial number
			}
				_delay_ms(1000);
		}

	}		

	//		char *STR = {0x3E,0x3F,0x26,0x74,0};
	//		lcd_send_string(1, 0,  STR);



	//		lcd_send_cmd(CMD_SET_CGRAM_ADDR, 0, 1);

//	lcd_send_string(1, 0,  "\xB0\x3F\x3E\x3D\0");
	//		lcd_send_string(2, 0, "абвгдежзийклмноп\0");
	//		lcd_fill(7, 6, 0x24);

/*	lcd_move_to(2, 10);
	lcd_write_char(0x00);
	lcd_write_char(0x01);
	lcd_write_char(0x02);
*/


//	if (OW_check_presence()) lcd_send_string(1, 0, "1-wire is here!\0");
//	else lcd_send_string(1, 0, "No devices :(\0");

//	lcd_move_to(1, 0);
//	lcd_write_hex(OW_check_presence());

//STOP
/*
    while(1)
    {
		//
//		uint8_t PCE = OW_check_presence();
//		if (PCE)
		if (OW_check_presence())
		{
			lcd_send_string(1, 0, "1-wire is here! \0");
//	lcd_clear_display();
			OW_writeByte(READ_ROM);
//			lcd_send_string(2, 0, "0123456789ABCDEF\0");
//			lcd_send_string(2, 0, "****************\0");
//			lcd_move_to(2, 0);

			char iKey[8];
			char CalcedCRC;
			for (short i = 0; i < 7; i++)
			{
				iKey[i] = OW_readByte();
//				CalcedCRC = CRC8(iKey[i], (i? 0: 1));
			}

			CalcedCRC = 0x00;
			for (short i = 0; i < 7; i++)
			{
				CalcedCRC = CRC8(iKey[i], CalcedCRC);
			}
			iKey[7] = OW_readByte();		//CRC received

			lcd_move_to(2, 0);
			lcd_write_hex(iKey[0]);			//family code

			for (short i = 6; i >=1; i--)
			{
				lcd_write_hex(iKey[i]);		//device serial number
			}
			
//			char CalcedCRC = Crc8(iKey, 7);
//STOP
			for (int i = 0; i < 3; i++) {
				lcd_move_to(2, 14);
				lcd_write_hex(iKey[7]);			//CRC
				_delay_ms(500);
				lcd_move_to(1, 14);
				lcd_write_hex(CalcedCRC);
				_delay_ms(1000);
			}			
		}		
		
		else {
			lcd_send_string(1, 0, "no devices :(   \0");
			lcd_send_string(2, 0, "                \0");
		}

    }
*/
} // main()


/*				lcd_write_hex(0xFA);
				OW_readByte();
				lcd_write_hex(0xDE);
				OW_readByte();
				lcd_write_hex(0xBC);
				lcd_write_hex(0xA0);
				lcd_write_hex(0x73);
				lcd_write_hex(0x88);
				lcd_write_hex(0x55);
				lcd_write_hex(0x99);
*/
//--------------------------------------------------------------------

/*
void testCRC() {
//		char iKey[8] = {0x02, 0x1C, 0xB8, 0x01, 0x00, 0x00, 0x00, 0xA2};
//		char iKey[8] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};
		char iKey[8] = {0x01, 0x16, 0x0A, 0x54, 0x06, 0x00, 0x00, 0x6E};
			char CalcedCRC = 0x00;
			for (short i = 0; i < 8; i++) CalcedCRC = CRC8(iKey[i], CalcedCRC);
}
//--------------------------------------------------------------------
*/

/*
void testStoreBits() {
char Z[17] = {'A', 'B', 'C', 'D', '1', '2', '3', 'F', 'E', 'D', 'C', '9', '8', '7', '6', '5', '\0'};
char X[17];
char Y[17];
uint8_t i;
uint8_t BitToStore;
uint8_t BT;
uint8_t bt;

for (i = 1; i < 128; i++) {
	
	BT = (i - 1) / 8;
	bt = (i - 1) % 8;
	
	BitToStore = (Z[BT] & (1 << bt)) >> bt;
	SetBitInDeviceROM(X, i, BitToStore);
}

for (i = 1; i < 128; i++) {
	
	BT = (i - 1) / 8;
	bt = (i - 1) % 8;
	
	if (GetBitFromDeviceROM(X, i)) Y[BT] |= 1 << bt;
	else Y[BT] &= ~(1 << bt);
}

lcd_send_string(1, 0, Z);
lcd_send_string(2, 0, Y);
STOP
}
//--------------------------------------------------------------------
*/