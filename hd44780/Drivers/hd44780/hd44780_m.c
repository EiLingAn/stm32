/*
 * hd44780_m.c
 *
 *  Created on: Jun 4, 2024
 *      Author: BUDZILLA
 */

#include "main.h"
#include "hd44780.h"

#if (LCD_MULTI_DISPLAY == LCD_MULTI)

#if (LCD_INTERFACE == INTERFACE_SPI)
  extern SPI_HandleTypeDef hspi2;
#elif (LCD_INTERFACE == INTERFACE_I2C)
  extern I2C_HandleTypeDef hi2c1;
#endif

  uint8_t _displayfunction;
  uint8_t _displaycontrol;
  uint8_t _displaymode;

  uint8_t _initialized;
  uint8_t dotsize = LCD_5x8DOTS;

  uint8_t _numlines;
  uint8_t _row_offsets[4];

  uint8_t spiData[LCD_DISPL_NUM];

#if (LCD_INTERFACE == INTERFACE_SPI)
  static void spiSend(uint8_t lcdNum, uint8_t val) {
    if (lcdNum==LCD_ALL) {
      for (uint8_t i=0; i<LCD_DISPL_NUM; i++) spiData[i] = val;
    } else { spiData[lcdNum] = val; }

    HAL_SPI_Transmit(&hspi2, spiData, LCD_DISPL_NUM, 100);
    HAL_GPIO_WritePin(x595_LE_GPIO_Port, x595_LE_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(x595_LE_GPIO_Port, x595_LE_Pin, GPIO_PIN_RESET);
  }
#endif

#if (LCD_INTERFACE == INTERFACE_PAR)
  static void pulseEnable(uint8_t lcdNum) {
    if (lcdNum == LCD_ALL) {
      HAL_GPIO_WritePin(LCD_E0_GPIO_Port, LCD_E0_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(LCD_E1_GPIO_Port, LCD_E1_Pin, GPIO_PIN_SET);
      delay_us(80);
      HAL_GPIO_WritePin(LCD_E0_GPIO_Port, LCD_E0_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LCD_E1_GPIO_Port, LCD_E1_Pin, GPIO_PIN_RESET);
      delay_us(80);
    } else if (lcdNum == 0) {
      HAL_GPIO_WritePin(LCD_E0_GPIO_Port, LCD_E0_Pin, GPIO_PIN_SET);
      delay_us(80);
      HAL_GPIO_WritePin(LCD_E0_GPIO_Port, LCD_E0_Pin, GPIO_PIN_RESET);
      delay_us(80);
    } else {
      HAL_GPIO_WritePin(LCD_E1_GPIO_Port, LCD_E1_Pin, GPIO_PIN_SET);
      delay_us(80);
      HAL_GPIO_WritePin(LCD_E1_GPIO_Port, LCD_E1_Pin, GPIO_PIN_RESET);
      delay_us(80);
    }
#elif (LCD_INTERFACE == INTERFACE_SPI)
    static void pulseEnable(uint8_t lcdNum, uint8_t data) {
      data |= (1<<LCD_EN);
      spiSend(lcdNum, data);
      delay_us(80);
      data &= ~(1<<LCD_EN);
      spiSend(lcdNum, data);
      delay_us(80);
#elif (LCD_INTERFACE == INTERFACE_I2C)
#endif
  }

#if (LCD_INTERFACE == INTERFACE_PAR)
  static void write4bits(uint8_t lcdNum, uint8_t val) {
    HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, val&0x10);
    HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, val&0x20);
    HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, val&0x40);
    HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, val&0x80);
    pulseEnable(lcdNum);
  }
#endif

  static void send(uint8_t lcdNum, uint8_t mode, uint8_t val) {
#if (LCD_INTERFACE == INTERFACE_PAR)
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, mode);
    write4bits(lcdNum, val);
    val = val<<4;
    write4bits(lcdNum, val);
#elif (LCD_INTERFACE == INTERFACE_SPI)
    uint8_t dByte = val;
    dByte &= 0xf0;
    if (mode == 1) dByte |= (1<<LCD_RS);
    pulseEnable(lcdNum, dByte);
    dByte = (val<<4) & 0xf0;
    if (mode == 1) dByte |= (1<<LCD_RS);
    pulseEnable(lcdNum, dByte);
#elif (LCD_INTERFACE == INTERFACE_I2C)
#endif
  }

  static void lcd_setRowOffsets(uint8_t row0, uint8_t row1, uint8_t row2, uint8_t row3) {
    _row_offsets[0] = row0;
    _row_offsets[1] = row1;
    _row_offsets[2] = row2;
    _row_offsets[3] = row3;
  }


  void lcd_begin(uint8_t lcdNum, uint8_t cols, uint8_t lines) {
    _displayfunction = LCD_4BITMODE | LCD_1LINE | dotsize;
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
#if (LCD_INTERFACE == INTERFACE_PAR)
    write4bits(lcdNum, 0x30);   // we start in 8bit mode, try to set 4 bit mode
    delay_ms(5);     // wait min 4.1ms
    write4bits(lcdNum, 0x30);   // second try
    delay_ms(5);     // wait min 4.1ms
    write4bits(lcdNum, 0x30);   // third go!
    delay_us(150);
    write4bits(lcdNum, 0x20);   // finally, set to 4-bit interface
#elif (LCD_INTERFACE == INTERFACE_SPI)
    pulseEnable(lcdNum, 0x30);   // we start in 8bit mode, try to set 4 bit mode
    delay_ms(5);     // wait min 4.1ms
    pulseEnable(lcdNum, 0x30);   // second try
    delay_ms(5);     // wait min 4.1ms
    pulseEnable(lcdNum, 0x30);   // third go!
    delay_us(150);
    pulseEnable(lcdNum, 0x20);   // finally, set to 4-bit interface
#endif
    // finally, set # lines, font size, etc.
    lcd_command(lcdNum, LCD_FUNCTIONSET | _displayfunction);
    // turn the display on with no cursor or blinking default
    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    lcd_command(lcdNum, LCD_DISPLAYCONTROL | _displaycontrol);
    lcd_clear(lcdNum);
    // Initialize to default text direction (for romance languages)
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    // set the entry mode
    lcd_command(lcdNum, LCD_ENTRYMODESET | _displaymode);
#if (LCD_BIG_FONT == TRUE)
    lcd_createChars(lcdNum);
#endif
  }

  void lcd_clear(uint8_t lcdNum) {
    lcd_command(lcdNum, LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
    delay_us(2000);  // this command takes a long time!
  }

  void lcd_home(uint8_t lcdNum) {
    lcd_command(lcdNum, LCD_RETURNHOME);  // set cursor position to zero
    delay_us(2000);  // this command takes a long time!
  }

  void lcd_createChar(uint8_t lcdNum, uint8_t chrAdr, uint8_t chr[]) {
//    location &= 0x7;    // we only have 8 locations 0-7
//    lcd_command(lcdNum, LCD_SETCGRAMADDR | (location << 3));
//    for (int i=0; i<8; i++) { lcd_write(lcdNum, charmap[i]); }
    chrAdr &= 0x7; // we only have 8 charAdr 0-7
    lcd_command(lcdNum, LCD_SETCGRAMADDR | (chrAdr << 3));
    for (int i=0; i<8; i++) { lcd_write(lcdNum, chr[i]); }
  }

  void lcd_setCursor(uint8_t lcdNum, uint8_t col, uint8_t row) {
    const size_t max_lines = sizeof(_row_offsets) / sizeof(*_row_offsets);
    if ( row >= max_lines ) { row = max_lines - 1; }    // we count rows starting w/0
    if ( row >= _numlines ) { row = _numlines - 1; }    // we count rows starting w/0
    lcd_command(lcdNum, LCD_SETDDRAMADDR | (col + _row_offsets[row]));
  }

  void lcd_write(uint8_t lcdNum, uint8_t val) { send(lcdNum, 1, val); }

  void lcd_command(uint8_t lcdNum, uint8_t val) { send(lcdNum, 0, val); }

  void lcd_print(uint8_t lcdNum, const char *str) {
    while (*str) lcd_write(lcdNum, *str++);
  }

#endif
