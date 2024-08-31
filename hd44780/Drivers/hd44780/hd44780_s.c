/*
 * hd44780_s.c
 *
 *  Created on: Jun 4, 2024
 *      Author: BUDZILLA
 */

#include "main.h"
#include "hd44780.h"

#include "hd44780.h"

#if (LCD_MULTI_DISPLAY == LCD_SINGLE)

  uint8_t _displayfunction;
  uint8_t _displaycontrol;
  uint8_t _displaymode;

  uint8_t _initialized;
  uint8_t dotsize = LCD_5x8DOTS;

  uint8_t _numlines;
  uint8_t _row_offsets[4];

  static void pulseEnable(void) {
#if (LCD_INTERFACE == INTERFACE_PAR)
    HAL_GPIO_WritePin(LCD_E0_GPIO_Port, LCD_E0_Pin, GPIO_PIN_SET);
    delay_us(100);
    HAL_GPIO_WritePin(LCD_E0_GPIO_Port, LCD_E0_Pin, GPIO_PIN_RESET);
    delay_us(100);
#endif
  }

#if (LCD_DATA_MODE == DATA_4BIT)
  static void write4bits(uint8_t val) {
#if (LCD_INTERFACE == INTERFACE_PAR)
    HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, val&0x10);
    HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, val&0x20);
    HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, val&0x40);
    HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, val&0x80);
#endif // LCD_INTERFACE
    pulseEnable();
  }
#else // LCD_DATA_MODE
  static void write8bits(uint8_t val) {
#if (LCD_INTERFACE == INTERFACE_PAR)
    HAL_GPIO_WritePin(LCD_D0_GPIO_Port, LCD_D0_Pin, val&0x01);
    HAL_GPIO_WritePin(LCD_D1_GPIO_Port, LCD_D1_Pin, val&0x02);
    HAL_GPIO_WritePin(LCD_D2_GPIO_Port, LCD_D2_Pin, val&0x04);
    HAL_GPIO_WritePin(LCD_D3_GPIO_Port, LCD_D3_Pin, val&0x08);
    HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, val&0x10);
    HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, val&0x20);
    HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, val&0x40);
    HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, val&0x80);
#endif // LCD_INTERFACE
    pulseEnable();
  }
#endif // LCD_DATA_MODE

  static void send(uint8_t mode, uint8_t val) {
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, mode);
#if (LCD_DATA_MODE == DATA_4BIT)
    write4bits(val);
    val = val<<4;
    write4bits(val);
#else
    write8bits(val);
#endif
  }

  static void lcd_setRowOffsets(uint8_t row0, uint8_t row1, uint8_t row2, uint8_t row3) {
    _row_offsets[0] = row0;
    _row_offsets[1] = row1;
    _row_offsets[2] = row2;
    _row_offsets[3] = row3;
  }



  void lcd_begin(uint8_t cols, uint8_t lines) {
#if (LCD_DATA_MODE == DATA_4BIT)
    _displayfunction = LCD_4BITMODE | LCD_1LINE | dotsize;
#else
    _displayfunction = LCD_8BITMODE | LCD_1LINE | dotsize;
#endif
    if (lines > 1) { _displayfunction |= LCD_2LINE; }
    _numlines = lines;
    lcd_setRowOffsets(0x00, 0x40, 0x00 + cols, 0x40 + cols);
    // for some 1 line displays you can select a 10 pixel high font
    if ((dotsize != LCD_5x8DOTS) && (lines == 1)) { _displayfunction |= LCD_5x10DOTS; }
    // according to datasheet, we need at least 40ms after power rises above 2.7V
    // before sending commands.
    delay_ms(50);
    // Now we pull both RS and R/W low to begin commands
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);
    //put the LCD into 4 bit or 8 bit mode
#if (LCD_DATA_MODE == DATA_4BIT)
    write4bits(0x30);   // we start in 8bit mode, try to set 4 bit mode
    delay_ms(5);     // wait min 4.1ms
    write4bits(0x30);   // second try
    delay_ms(5);     // wait min 4.1ms
    write4bits(0x30);   // third go!
    delay_us(150);
    write4bits(0x20);   // finally, set to 4-bit interface
#else
    lcd_command(LCD_FUNCTIONSET | _displayfunction);  // Send function set command sequence
    delay_ms(5);                                   // wait more than 4.1ms
    lcd_command(LCD_FUNCTIONSET | _displayfunction);  // second try
    delay_us(150);
    lcd_command(LCD_FUNCTIONSET | _displayfunction);  // third go
#endif
    // finally, set # lines, font size, etc.
    lcd_command(LCD_FUNCTIONSET | _displayfunction);
    // turn the display on with no cursor or blinking default
    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    lcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
    lcd_clear();
    // Initialize to default text direction (for romance languages)
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    // set the entry mode
    lcd_command(LCD_ENTRYMODESET | _displaymode);
#if (LCD_BIG_FONT == TRUE)
    lcd_createChars();
#endif
  }

  void lcd_clear(void) {
    lcd_command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
    delay_us(2000);  // this command takes a long time!
  }

  void lcd_home(void) {
    lcd_command(LCD_RETURNHOME);  // set cursor position to zero
    delay_us(2000);  // this command takes a long time!
  }

  void lcd_createChar(uint8_t chrAdr, uint8_t chr[]) {
    chrAdr &= 0x7; // we only have 8 charAdr 0-7
    lcd_command(LCD_SETCGRAMADDR | (chrAdr << 3));
    for (int i=0; i<8; i++) { lcd_write(chr[i]); }
  }

  void lcd_setCursor(uint8_t col, uint8_t row) {
    const size_t max_lines = sizeof(_row_offsets) / sizeof(*_row_offsets);
    if ( row >= max_lines ) { row = max_lines - 1; }    // we count rows starting w/0
    if ( row >= _numlines ) { row = _numlines - 1; }    // we count rows starting w/0
    lcd_command(LCD_SETDDRAMADDR | (col + _row_offsets[row]));
  }

  void lcd_write(uint8_t val) { send(1, val); }

  void lcd_command(uint8_t val) { send(0, val); }

  void lcd_print(const char *str) { while (*str) lcd_write(*str++); }

#endif


