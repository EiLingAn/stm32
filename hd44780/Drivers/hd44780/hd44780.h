/*
 * hd44780.h
 *
 *  Created on: Jun 4, 2024
 *      Author: BUDZILLA
 */

#include <inttypes.h>

#ifndef ___HD44780_H___
#define ___HD44780_H___

/* Define LCD_INTERFACE */
#define INTERFACE_PAR   1
#define INTERFACE_SPI   2
#define INTERFACE_I2C   3
/* Define LCD_MULTI_DISPLAY */
#define LCD_SINGLE      1
#define LCD_MULTI       2
/* Define LCD_DATA_MODE */
#define DATA_8BIT       1
#define DATA_4BIT       2

/* LCD setup */
#define LCD_MULTI_DISPLAY   LCD_MULTI
#define LCD_INTERFACE       INTERFACE_PAR
#define LCD_DISPL_NUM       2               // 1..4   (max 2 @ paralel)
#define LCD_DATA_MODE       DATA_4BIT       // 4bit @ multi
#define LCD_BIG_FONT        FALSE

// commands
#define LCD_CLEARDISPLAY    0x01
#define LCD_RETURNHOME      0x02
#define LCD_ENTRYMODESET    0x04
#define LCD_DISPLAYCONTROL  0x08
#define LCD_CURSORSHIFT     0x10
#define LCD_FUNCTIONSET     0x20
#define LCD_SETCGRAMADDR    0x40
#define LCD_SETDDRAMADDR    0x80
// flags for display entry mode
#define LCD_ENTRYRIGHT          0x00
#define LCD_ENTRYLEFT           0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00
// flags for display on/off control
#define LCD_DISPLAYON   0x04
#define LCD_DISPLAYOFF  0x00
#define LCD_CURSORON    0x02
#define LCD_CURSOROFF   0x00
#define LCD_BLINKON     0x01
#define LCD_BLINKOFF    0x00
// flags for display/cursor shift
#define LCD_DISPLAYMOVE   0x08
#define LCD_CURSORMOVE    0x00
#define LCD_MOVERIGHT     0x04
#define LCD_MOVELEFT      0x00
// flags for function set
#define LCD_8BITMODE  0x10
#define LCD_4BITMODE  0x00
#define LCD_2LINE     0x08
#define LCD_1LINE     0x00
#define LCD_5x10DOTS  0x04
#define LCD_5x8DOTS   0x00

/* Define others */
#define FALSE           0
#define TRUE            1
#define LCD_ALL         0xff


extern void delay_init(void);
extern void delay_us(uint16_t us);
extern void delay_ms(uint32_t ms);

#if (LCD_MULTI_DISPLAY == LCD_SINGLE)
  void lcd_begin(uint8_t , uint8_t );
  void lcd_clear();
  void lcd_home();
  void lcd_createChar(uint8_t, uint8_t[]);
  void lcd_setCursor(uint8_t, uint8_t);
  void lcd_write(uint8_t );
  void lcd_command(uint8_t );
  void lcd_print(const char *);
#if (LCD_BIG_FONT == TRUE)
  void lcd_createChars(void);
  void lcd_printBig(const char *, uint8_t , uint8_t );
#endif
#endif

#if (LCD_MULTI_DISPLAY == LCD_MULTI)
  void lcd_begin(uint8_t , uint8_t , uint8_t );
  void lcd_clear(uint8_t );
  void lcd_home(uint8_t );
  void lcd_createChar(uint8_t , uint8_t, uint8_t[]);
  void lcd_setCursor(uint8_t , uint8_t, uint8_t);
  void lcd_write(uint8_t , uint8_t );
  void lcd_command(uint8_t , uint8_t );
  void lcd_print(uint8_t , const char *);
#if (LCD_BIG_FONT == TRUE)
  void lcd_createChars(uint8_t );
  void lcd_printBig(uint8_t , const char *, uint8_t , uint8_t );
#endif
#endif


#endif /* ___HD44780_H___ */
