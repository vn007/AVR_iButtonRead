/*
 * _1wire.h
 *
 * Created: 12.04.2014 4:06:16
 *  Author: lv
 */ 


#ifndef _1WIRE_H_
#define _1WIRE_H_

void OW_init();
uint8_t OW_check_presence();
void OW_writebit(char Bit_To_Send);
void OW_writeByte(char byte_to_send);
uint8_t OW_readbit();
char OW_readByte();
void Delay_us(int n);
char CRC8(char DataByte, char crc);
uint8_t Scan_OW_devices(bool newscan);
void SetBitInDeviceROM(uint8_t BitNo, uint8_t val);
char GetBitFromDeviceROM(uint8_t BitNo);
uint8_t OW_readbitTEST();

#endif /* 1WIRE_H_ */