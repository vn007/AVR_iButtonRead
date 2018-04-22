/*
 * LCD.h
 *
 * Created: 12.04.2014 4:27:17
 *  Author: lv
 */ 


#ifndef LCD_H_
#define LCD_H_

void set_mode(uint8_t mode);
void strobe();
bool dev_is_busy(void);
void lcd_send_cmd(uint8_t cmd, uint8_t arg, bool need_wait_busy);
void lcd_write_char(unsigned char char_to_write);
void lcd_write_hex(unsigned char hex_to_write);
void lcd_fill(uint8_t pos, uint8_t qty, const unsigned char pattern);
void lcd_send_string(uint8_t row, uint8_t col, char *STR);
void lcd_move_to(uint8_t row, uint8_t col);
void lcd_clear_display(void);
void lcd_init(void);
void lcd_create_cg(uint8_t charno, unsigned char *LNS);

#endif /* LCD_H_ */