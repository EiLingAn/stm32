/*
 * bigFonts.c
 *
 *  Created on: May 15, 2024
 *      Author: BUDZILLA
 */

#include "hd44780.h"

#include <stdbool.h>
#include <string.h>

#if (LCD_BIG_FONT == TRUE)

  static uint8_t LT[8]  = { 0b00111, 0b01111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 };
  static uint8_t UB[8]  = { 0b11111, 0b11111, 0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000 };
  static uint8_t RT[8]  = { 0b11100, 0b11110, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 };
  static uint8_t LL[8]  = { 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b01111, 0b00111 };
  static uint8_t LB[8]  = { 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111 };
  static uint8_t LR[8]  = { 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11110, 0b11100 };
  static uint8_t UMB[8] = { 0b11111, 0b11111, 0b11111, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111 };
  static uint8_t LMB[8] = { 0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111 };

  uint8_t chr0[] = { 3, 8, 1, 2, 3, 4, 5 };
  uint8_t chr1[] = { 2, 1, 2, 254, 255 };
  uint8_t chr2[] = { 3, 6, 6, 2, 3, 7, 7 };
  uint8_t chr3[] = { 3, 6, 6, 2, 7, 7, 5 };
  uint8_t chr4[] = { 3, 3, 4, 2, 254, 254, 255 };
  uint8_t chr5[] = { 3, 255, 6, 6, 7, 7, 5 };
  uint8_t chr6[] = { 3, 8, 6, 6, 3, 7, 5 };
  uint8_t chr7[] = { 3, 1, 1, 2, 254, 8, 254 };
  uint8_t chr8[] = { 3, 8, 6, 2, 3, 7, 5 };
  uint8_t chr9[] = { 3, 8, 6, 2, 254, 254, 255 };
  uint8_t chrA[] = { 3, 8, 6, 2, 255, 254, 255 };
  uint8_t chrB[] = { 3, 255, 6, 5, 255, 7, 2 };
  uint8_t chrC[] = { 3, 8, 1, 1, 3, 4, 4 };
  uint8_t chrD[] = { 3, 255, 1, 2, 255, 4, 5 };
  uint8_t chrE[] = { 3, 255, 6, 6, 255, 7, 7 };
  uint8_t chrF[] = { 3, 255, 6, 6, 255, 254, 254 };
  uint8_t chrG[] = { 3, 8, 1, 1, 3, 4, 2 };
  uint8_t chrH[] = { 3, 255, 4, 255, 255, 254, 255 };
  uint8_t chrI[] = { 3, 1, 255, 1, 4, 255, 4 };
  uint8_t chrJ[] = { 3, 254, 254, 255, 4, 4, 5 };
  uint8_t chrK[] = { 3, 255, 4, 5, 255, 254, 2 };
  uint8_t chrL[] = { 3, 255, 254, 254, 255, 4, 4 };
  uint8_t chrM[] = { 4, 8, 3, 5, 2, 255, 254, 254, 255 };
  uint8_t chrN[] = { 4, 8, 2, 254, 255, 255, 254, 3, 5 };
  uint8_t chrO[] = { 3, 255, 6, 2, 255, 254, 254 };
  uint8_t chrP[] = { 3, 255, 6, 2, 255, 254, 254 };
  uint8_t chrQ[] = { 4, 8, 1, 2, 254, 3, 4, 255, 4 };
  uint8_t chrR[] = { 3, 255, 6, 2, 255, 254, 2 };
  uint8_t chrS[] = { 3, 8, 6, 6, 7, 7, 5 };
  uint8_t chrT[] = { 3, 1, 255, 1, 254, 255, 254 };
  uint8_t chrU[] = { 3, 255, 254, 255, 3, 4, 5 };
  uint8_t chrV[] = { 4, 3, 254, 254, 5, 254, 2, 8, 254 };
  uint8_t chrW[] = { 4, 255, 254, 254, 255, 3, 8, 2, 5 };
  uint8_t chrX[] = { 3, 3, 4, 5, 8, 254, 2 };
  uint8_t chrY[] = { 3, 3, 4, 5, 254, 255, 254 };
  uint8_t chrZ[] = { 3, 1, 6, 5, 8, 7, 4 };
  uint8_t cSpc[] = { 1, 254, 254};


#if (LCD_MULTI_DISPLAY == LCD_SINGLE)

  uint8_t char_x = 0;
  uint8_t char_y = 0;

  void lcd_createChars(void) {
    lcd_createChar(1, UB);
    lcd_createChar(2, RT);
    lcd_createChar(3, LL);
    lcd_createChar(4, LB);
    lcd_createChar(5, LR);
    lcd_createChar(6, UMB);
    lcd_createChar(7, LMB);
    lcd_createChar(8, LT);
  }

  void lcd_printBig(const char *chrs, uint8_t x, uint8_t y) {
    uint8_t *chr;

    for (uint8_t i = 0; i<strlen(chrs); i++) {
      if (chrs[i] == '0' || chrs[i] == 'O' || chrs[i] == 'o') { chr = chr0; }
      else if (chrs[i] == '1') { chr = chr1; }
      else if (chrs[i] == '2') { chr = chr2; }
      else if (chrs[i] == '3') { chr = chr3; }
      else if (chrs[i] == '4') { chr = chr4; }
      else if (chrs[i] == '5') { chr = chr5; }
      else if (chrs[i] == '6') { chr = chr6; }
      else if (chrs[i] == '7') { chr = chr7; }
      else if (chrs[i] == '8') { chr = chr8; }
      else if (chrs[i] == '9') { chr = chr9; }
      else if ((chrs[i] == 'a') || (chrs[i] == 'A')) { chr = chrA; }
      else if ((chrs[i] == 'b') || (chrs[i] == 'B')) { chr = chrB; }
      else if ((chrs[i] == 'c') || (chrs[i] == 'C')) { chr = chrC; }
      else if ((chrs[i] == 'd') || (chrs[i] == 'D')) { chr = chrD; }
      else if ((chrs[i] == 'e') || (chrs[i] == 'E')) { chr = chrE; }
      else if ((chrs[i] == 'f') || (chrs[i] == 'F')) { chr = chrF; }
      else if ((chrs[i] == 'g') || (chrs[i] == 'G')) { chr = chrG; }
      else if ((chrs[i] == 'h') || (chrs[i] == 'H')) { chr = chrH; }
      else if ((chrs[i] == 'i') || (chrs[i] == 'I')) { chr = chrI; }
      else if ((chrs[i] == 'j') || (chrs[i] == 'J')) { chr = chrJ; }
      else if ((chrs[i] == 'k') || (chrs[i] == 'K')) { chr = chrK; }
      else if ((chrs[i] == 'l') || (chrs[i] == 'L')) { chr = chrL; }
      else if ((chrs[i] == 'm') || (chrs[i] == 'M')) { chr = chrM; }
      else if ((chrs[i] == 'n') || (chrs[i] == 'N')) { chr = chrN; }
      else if ((chrs[i] == 'o') || (chrs[i] == 'O')) { chr = chrO; }
      else if ((chrs[i] == 'p') || (chrs[i] == 'P')) { chr = chrP; }
      else if ((chrs[i] == 'q') || (chrs[i] == 'Q')) { chr = chrQ; }
      else if ((chrs[i] == 'r') || (chrs[i] == 'R')) { chr = chrR; }
      else if ((chrs[i] == 's') || (chrs[i] == 'S')) { chr = chrS; }
      else if ((chrs[i] == 't') || (chrs[i] == 'T')) { chr = chrT; }
      else if ((chrs[i] == 'u') || (chrs[i] == 'U')) { chr = chrU; }
      else if ((chrs[i] == 'v') || (chrs[i] == 'V')) { chr = chrV; }
      else if ((chrs[i] == 'w') || (chrs[i] == 'W')) { chr = chrW; }
      else if ((chrs[i] == 'x') || (chrs[i] == 'X')) { chr = chrX; }
      else if ((chrs[i] == 'y') || (chrs[i] == 'Y')) { chr = chrY; }
      else if ((chrs[i] == 'z') || (chrs[i] == 'Z')) { chr = chrZ; }
      else if (chrs[i] == ' ') { chr = cSpc; }

      lcd_setCursor(x, y);
      for (uint8_t i=0; i<chr[0]*2; i++) {
        lcd_write(chr[i+1]);
        if (i==chr[0]-1) lcd_setCursor(x, y + 1);
      }
      char_x = char_x + chr[0];
      x = x + chr[0];
    }
  }

#endif

#if (LCD_MULTI_DISPLAY == LCD_MULTI)

  uint8_t char_x = 0;
  uint8_t char_y = 0;

  void lcd_createChars(uint8_t lcdNum) {
    lcd_createChar(lcdNum, 1, UB);
    lcd_createChar(lcdNum, 2, RT);
    lcd_createChar(lcdNum, 3, LL);
    lcd_createChar(lcdNum, 4, LB);
    lcd_createChar(lcdNum, 5, LR);
    lcd_createChar(lcdNum, 6, UMB);
    lcd_createChar(lcdNum, 7, LMB);
    lcd_createChar(lcdNum, 8, LT);
  }

  void lcd_printBig(uint8_t lcdNum, const char *chrs, uint8_t x, uint8_t y) {
    uint8_t *chr;

    for (uint8_t i = 0; i<strlen(chrs); i++) {
      if (chrs[i] == '0' || chrs[i] == 'O' || chrs[i] == 'o') { chr = chr0; }
      else if (chrs[i] == '1') { chr = chr1; }
      else if (chrs[i] == '2') { chr = chr2; }
      else if (chrs[i] == '3') { chr = chr3; }
      else if (chrs[i] == '4') { chr = chr4; }
      else if (chrs[i] == '5') { chr = chr5; }
      else if (chrs[i] == '6') { chr = chr6; }
      else if (chrs[i] == '7') { chr = chr7; }
      else if (chrs[i] == '8') { chr = chr8; }
      else if (chrs[i] == '9') { chr = chr9; }
      else if ((chrs[i] == 'a') || (chrs[i] == 'A')) { chr = chrA; }
      else if ((chrs[i] == 'b') || (chrs[i] == 'B')) { chr = chrB; }
      else if ((chrs[i] == 'c') || (chrs[i] == 'C')) { chr = chrC; }
      else if ((chrs[i] == 'd') || (chrs[i] == 'D')) { chr = chrD; }
      else if ((chrs[i] == 'e') || (chrs[i] == 'E')) { chr = chrE; }
      else if ((chrs[i] == 'f') || (chrs[i] == 'F')) { chr = chrF; }
      else if ((chrs[i] == 'g') || (chrs[i] == 'G')) { chr = chrG; }
      else if ((chrs[i] == 'h') || (chrs[i] == 'H')) { chr = chrH; }
      else if ((chrs[i] == 'i') || (chrs[i] == 'I')) { chr = chrI; }
      else if ((chrs[i] == 'j') || (chrs[i] == 'J')) { chr = chrJ; }
      else if ((chrs[i] == 'k') || (chrs[i] == 'K')) { chr = chrK; }
      else if ((chrs[i] == 'l') || (chrs[i] == 'L')) { chr = chrL; }
      else if ((chrs[i] == 'm') || (chrs[i] == 'M')) { chr = chrM; }
      else if ((chrs[i] == 'n') || (chrs[i] == 'N')) { chr = chrN; }
      else if ((chrs[i] == 'p') || (chrs[i] == 'P')) { chr = chrP; }
      else if ((chrs[i] == 'q') || (chrs[i] == 'Q')) { chr = chrQ; }
      else if ((chrs[i] == 'r') || (chrs[i] == 'R')) { chr = chrR; }
      else if ((chrs[i] == 's') || (chrs[i] == 'S')) { chr = chrS; }
      else if ((chrs[i] == 't') || (chrs[i] == 'T')) { chr = chrT; }
      else if ((chrs[i] == 'u') || (chrs[i] == 'U')) { chr = chrU; }
      else if ((chrs[i] == 'v') || (chrs[i] == 'V')) { chr = chrV; }
      else if ((chrs[i] == 'w') || (chrs[i] == 'W')) { chr = chrW; }
      else if ((chrs[i] == 'x') || (chrs[i] == 'X')) { chr = chrX; }
      else if ((chrs[i] == 'y') || (chrs[i] == 'Y')) { chr = chrY; }
      else if ((chrs[i] == 'z') || (chrs[i] == 'Z')) { chr = chrZ; }
      else if (chrs[i] == ' ') { chr = cSpc; }

      uint8_t len = chr[0]*2;
      lcd_setCursor(lcdNum, x, y);
      for (uint8_t i=0; i<len; i++) {
        lcd_write(lcdNum, chr[i+1]);
        if (i==chr[0]-1) lcd_setCursor(lcdNum, x, y + 1);
      }
      char_x = char_x + chr[0];
      x = x + chr[0];
    }
  }

#endif


#endif
