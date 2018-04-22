/*
 * CFile1.c
 *
 * Created: 01.04.2014 20:34:20
 *  Author: lv
 */ 

#define STOP while (1) {}

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

#include "1wire.h"

#define OW_port PORTC
#define OW_ddr DDRC
#define OW_port_input PINC
#define OW_pin_no 3
#define OW_speed spdLow				// set performance config

#define TEST_pin_no 5
#define TEST2_pin_no 6
#define TEST3_pin_no 4

#define OW_pin OW_port_input & (1 << OW_pin_no)

#define OW_SET_WRITE OW_ddr |= (1 << OW_pin_no)
#define OW_SET_READ OW_ddr &= ~(1 << OW_pin_no)
#define OW_SET1 OW_port |= (1 << OW_pin_no)
#define OW_SET0 OW_port &= ~(1 << OW_pin_no)

#define TEST_SET1 OW_port |= (1 << TEST_pin_no)
#define TEST_SET0 OW_port &= ~(1 << TEST_pin_no)
#define TEST_STROBE TEST_SET1; _delay_us(1); TEST_SET0;

#define TEST2_SET1 OW_port |= (1 << TEST2_pin_no)
#define TEST2_SET0 OW_port &= ~(1 << TEST2_pin_no)
#define TEST2_STROBE TEST2_SET1; _delay_us(1); TEST2_SET0;

#define TEST3_SET1 OW_port |= (1 << TEST3_pin_no)
#define TEST3_SET0 OW_port &= ~(1 << TEST3_pin_no)
#define TEST3_STROBE TEST3_SET1; _delay_us(1); TEST3_SET0;

//#define OW_w_delay OW_write_delay[OW_speed]
//#define OW_r_delay OW_read_delay[OW_speed]

int OW_write_delay[3] = {56, 30, 18};	// 3 different performance configs, no overdrive
int OW_read_delay[3] = {15, 10, 5};	// 3 different performance configs, no overdrive
	
#define READ_ROM 0x33
#define SKIP_ROM 0xCC
#define MATCH_ROM 0x55
#define SEARCH_ROM 0xF0
#define READ_MEMORY 0xF0

enum {spdLow = 0, spdMed, spdHigh};	// 3 different performance configs, no overdrive

uint8_t OW_w_delay;
uint8_t OW_r_delay;
	
//--------------------------------------------------------------------
//-------------  Implementations  ------------------------------------
//--------------------------------------------------------------------
void OW_init() {

	uint8_t OW_speed = spdLow;				// set performance config

	OW_w_delay = OW_write_delay[OW_speed];
	OW_r_delay = OW_read_delay[OW_speed];
	
	OW_ddr |= 1 << TEST_pin_no;
	OW_ddr |= 1 << TEST2_pin_no;
	OW_ddr |= 1 << TEST3_pin_no;

}
//--------------------------------------------------------------------

uint8_t OW_check_presence() {

//	OW_SET_READ;

//	do {} while (!OW_pin);
//	_delay_ms(10);					// 120us*8 ~= 1ms, at least 1 byte transfer from slaves can be completed 

	OW_SET_WRITE;
	OW_SET0;
	_delay_us(480);					//
	OW_SET_READ;
	_delay_us(70);					//
	
	if (OW_pin) return 0;			// no devices detected
	else {
		do {} while (!OW_pin);
		return 1;					// device(s) present
	}
	
//	return (OW_pin? 0: 1);
}
//--------------------------------------------------------------------

void OW_writebit(char Bit_To_Send) {

	OW_SET_WRITE;
	OW_SET0;
	_delay_us(15);					//
	if (Bit_To_Send) OW_SET_READ;	// set "1" на Port.Pin
//	Delay_us(OW_w_delay);
	_delay_us(25);					//
TEST2_STROBE
	_delay_us(25);					//
	OW_SET_READ;					//final return to "1"
	_delay_us(1);					//
}
//--------------------------------------------------------------------

void OW_writeByte(char byte_to_send) {
	
	for (uint8_t i = 8; i > 0; byte_to_send >>= 1, i--) {
		OW_writebit(byte_to_send & 1);
	}
}
//--------------------------------------------------------------------

uint8_t OW_readbit() {

	OW_SET_WRITE;
	OW_SET0;
	_delay_us(15);
//	Delay_us(OW_r_delay);				// t_LOWR
	OW_SET_READ;						// return to "1"
//	Delay_us(OW_r_delay >= 15? 3: 15 - OW_r_delay);			//t_RDV = 15us
//	Delay_us(OW_r_delay >= 15? 3: 10);			//t_RDV = 15us
	_delay_us(10);			//t_RDV = 15us
TEST_STROBE
//	uint8_t B = (OW_pin? 1: 0);
	uint8_t B = OW_pin;
	B = (B? 1: 0);
//	char B = ((PINC & (1 << 3))? 1: 0);
//	Delay_us(3 * OW_w_delay - 15);			//approximate t_SLOT
	_delay_us(35);			//approximate t_SLOT
//	_delay_us(60);			//forming t_SLOT
	while (!OW_pin) {}					//waiting for release '0' (if was)
//	_delay_us(10);						//time guard after release '0' (if was)
	return B;
}
//--------------------------------------------------------------------

char OW_readByte() {

	OW_SET_READ;

	char Byte = 0;
	for (uint8_t i = 0; i < 8; i++) {
		Byte >>= 1;
		if (OW_readbit()) Byte |= 0x80;
	}
	return Byte;
}
//--------------------------------------------------------------------

void Delay_us(int n){
	while (n--) {
		_delay_us(1);  // one microsecond
	}
}
//--------------------------------------------------------------------

char CRC8(char DataByte, char crc)
{
	char poly = 0x18;
	
	for (uint8_t i = 0; i < 8; i++) {
		if ((crc ^ DataByte) & 1) crc = ((crc ^ poly) >> 1) | 0x80;
		else crc >>= 1;
		DataByte >>= 1;
	}
	
	return crc;
}
//--------------------------------------------------------------------

uint8_t Scan_OW_devices(bool newscan)
{
	extern uint8_t Done_Flag;
	extern char DeviceROM[64];
	uint8_t Retval;

	uint8_t BitA;
	uint8_t BitB;

	static uint8_t LastDiscrepancy;
 
	uint8_t DiscrepancyMarker;
	
//*/	if (!OW_check_presence()) return 0;
	
	if (newscan) {
		LastDiscrepancy = 0;
		Done_Flag = 0;
	}
	
	Retval = 0;
	
	if (Done_Flag) {
		Done_Flag = 0;
		return Retval;
	}

//TEST_STROBE
	if (!OW_check_presence()) {
		LastDiscrepancy = 0;
		return Retval;
	}
	_delay_ms(1);
	
	DiscrepancyMarker = 0;

	OW_writeByte(SEARCH_ROM);
	
	_delay_ms(1);

	for (uint8_t RomBitIndex = 1; RomBitIndex <= 64; RomBitIndex++) {

		BitA = OW_readbit();
		BitB = OW_readbit();
			
		if (BitA == 1 && BitB == 1) {	// no device respond
			LastDiscrepancy = 0;
			return Retval;
		}
		else if (BitA ^ BitB) { SetBitInDeviceROM(RomBitIndex, BitA);	// BitA != BitB, no discrepancy
		}		
		else {	// BitA == BitB == 0 => DISCREPANCY
			if (RomBitIndex == LastDiscrepancy) SetBitInDeviceROM(RomBitIndex, 1);
			else if (RomBitIndex > LastDiscrepancy) {
				SetBitInDeviceROM(RomBitIndex, 0);
				DiscrepancyMarker = RomBitIndex;
			}			
			else if (!GetBitFromDeviceROM(RomBitIndex)) // RomBitIndex < LastDiscrepancy
				DiscrepancyMarker = RomBitIndex;
		}
		
		OW_writebit(GetBitFromDeviceROM(RomBitIndex));

	}	// 64 bits scanned
//TEST2_STROBE
	
	LastDiscrepancy = DiscrepancyMarker;

	char CalcedCRC = 0x00;
	for (uint8_t i = 0; i < 8; i++) CalcedCRC = CRC8(DeviceROM[i], CalcedCRC);
	if (!CalcedCRC) {
		if (!LastDiscrepancy) Done_Flag = 1;
		Retval = 1;
	}
	
	return Retval;

/*
	if (!LastDiscrepancy) Done_Flag = 1;
	Retval = 1;
	return Retval;
*/
}
//--------------------------------------------------------------------

void SetBitInDeviceROM(uint8_t BitNo, uint8_t val) {
	extern char DeviceROM[64];

	uint8_t ByteNo = (BitNo - 1)/ 8;
	uint8_t Bit_In_Byte = (BitNo - 1) % 8;

	if (val) DeviceROM[ByteNo] |= (1 << Bit_In_Byte);
	else DeviceROM[ByteNo] &= ~(1 << Bit_In_Byte);
}
//--------------------------------------------------------------------

char GetBitFromDeviceROM(uint8_t BitNo) {
	extern char DeviceROM[64];

	uint8_t ByteNo = (BitNo - 1) / 8;
	uint8_t Bit_In_Byte = (BitNo - 1) % 8;
	
	return (DeviceROM[ByteNo] & (1 << Bit_In_Byte)) >> Bit_In_Byte;
}
//--------------------------------------------------------------------

/*
uint8_t OW_readbitTEST() {
static uint8_t BitNo;
uint8_t Dev1[8] = {0x01, 0x16, 0x0A, 0x54, 0x06, 0x00, 0x00, 0x6E};
uint8_t Dev2[8] = {0x02, 0x1C, 0xB8, 0x01, 0x00, 0x00, 0x00, 0xA2};
// ... 00001010 00010110 00000001 => etc
// ... 10111000 00011100 00000010 => etc

return
(((Dev1[BitNo++ / 16] & (1 << ((BitNo/2) % 8))) >> ((BitNo/2) % 8)) ^ (BitNo % 2)) & \
(((Dev2[BitNo   / 16] & (1 << ((BitNo/2) % 8))) >> ((BitNo/2) % 8)) ^ (BitNo % 2));
}
//--------------------------------------------------------------------
*/