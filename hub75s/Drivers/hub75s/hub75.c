#include "main.h"
#include "hub75.h"

#include "fonts/system6x8.h"

#define FALSE   0
#define TRUE    1


extern TIM_HandleTypeDef htim2;

#if (CANVAS_MODE == 0)
  #include <stdio.h>
  char buff75[20];
#else
  uint8_t rgbBuff[3][BUFF_Y][BUFF_X];
#endif

uint8_t delayBrightness = 100,
        xPos = 0, yPos = 0,
        pixelColor = clWHITE,
        backColor = clBLACK,
        rgbConfig = RGB,
        dispOn = TRUE;

font_st sFont;

static const uint8_t color_index[] = {
  0, 1, 2, 3, 4, 5, 6, 7,   // RGB 0
  0, 1, 4, 5, 2, 3, 6, 7,   // RBG 8
  0, 2, 1, 3, 4, 6, 5, 7,   // GRB 16
  0, 4, 1, 5, 2, 6, 3, 7,   // GBR 24
  0, 4, 2, 6, 1, 5, 3, 7,   // BGR 32
  0, 2, 4, 6, 1, 3, 5, 7    // BRG 40
};

static void selectLine(uint8_t line) {
  HAL_GPIO_WritePin(A0_GPIO_Port, A0_Pin, line&0x01);
  HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, line&0x02);
#if (PANEL_SCAN == 8)
  HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, line&0x04);
#elif (PANEL_SCAN == 16)
  HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, line&0x04);
  HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, line&0x08);
#elif (PANEL_SCAN == 32)
  HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, line&0x04);
  HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, line&0x08);
  HAL_GPIO_WritePin(A4_GPIO_Port, A4_Pin, line&0x10);
#endif
}

static void sendLineData(uint8_t line) {
#if (CANVAS_MODE == 0)
  uint8_t fw = sFont.fontW+1;
  uint8_t top = 2*PANEL_SCAN;
  uint8_t bot = PANEL_SCAN;

  uint8_t x = PIXEL_X-1;
  while (x--) {
//  for (uint8_t x=0; x<PIXEL_X; x++) {
    uint8_t y = x%fw;
    uint8_t w = x/fw;

    uint8_t chrDat = sFont.fontTable[(buff75[w]-sFont.char1st)*fw + y];

    uint8_t clPxl = 0;

    if ((chrDat >> ((top-line-1)/2)) & 0x01) { clPxl = pixelColor; }     else { clPxl = backColor; }
    if ((chrDat >> ((bot-line-1)/2)) & 0x01) { clPxl |= pixelColor<<4; } else { clPxl |= backColor<<4; }

    HAL_GPIO_WritePin(R0_GPIO_Port, R0_Pin, clPxl&0x01);
    HAL_GPIO_WritePin(G0_GPIO_Port, G0_Pin, clPxl&0x02);
    HAL_GPIO_WritePin(B0_GPIO_Port, B0_Pin, clPxl&0x04);
    HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, clPxl&0x10);
    HAL_GPIO_WritePin(G1_GPIO_Port, G1_Pin, clPxl&0x20);
    HAL_GPIO_WritePin(B1_GPIO_Port, B1_Pin, clPxl&0x40);

    HAL_GPIO_WritePin(CK_GPIO_Port, CK_Pin, 1);
    HAL_GPIO_WritePin(CK_GPIO_Port, CK_Pin, 0);
  }
#else
  for (uint8_t x=0; x<BUFF_X; x++) {
    for (uint8_t i=0; i<8; i++) {
      uint8_t mask = 0x80>>i;
      HAL_GPIO_WritePin(R0_GPIO_Port, R0_Pin, rgbBuff[0][line][x] & mask);
      HAL_GPIO_WritePin(G0_GPIO_Port, G0_Pin, rgbBuff[1][line][x] & mask);
      HAL_GPIO_WritePin(B0_GPIO_Port, B0_Pin, rgbBuff[2][line][x] & mask);
      HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, rgbBuff[0][line+PANEL_SCAN][x] & mask);
      HAL_GPIO_WritePin(G1_GPIO_Port, G1_Pin, rgbBuff[1][line+PANEL_SCAN][x] & mask);
      HAL_GPIO_WritePin(B1_GPIO_Port, B1_Pin, rgbBuff[2][line+PANEL_SCAN][x] & mask);

      HAL_GPIO_WritePin(CK_GPIO_Port, CK_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(CK_GPIO_Port, CK_Pin, GPIO_PIN_RESET);
    }
  }
#endif
}

static void displayReset(void) {
  hub75_setBrightness(255);
  hub75_setRgbPin(RGB);
  hub75_setColor(clWHITE);
  hub75_setBackColor(clBLACK);
  hub75_setDisplay(TRUE);

  hub75_display();
}

void hub75_setFonts(font_st sfont) { sFont = sfont; }

void hub75_setRgbPin(uint8_t rgbPin) { rgbConfig = rgbPin; }

void hub75_setBackColor(uint8_t color) { backColor = color_index[(8*rgbConfig) + color]; }

void hub75_setColor(uint8_t color) { pixelColor = color_index[(8*rgbConfig) + color]; }

void hub75_setBrightness(uint8_t delayBright) { delayBrightness = delayBright; }

void hub75_setDisplay(uint8_t isOn) { dispOn = isOn; }

void hub75_display(void) {
  HAL_TIM_Base_Stop_IT(&htim2);
  if (dispOn == TRUE) {
    for (uint8_t y=0; y<PANEL_SCAN; y++) {
      selectLine(y);
      sendLineData(y);
      HAL_GPIO_WritePin(LE_GPIO_Port, LE_Pin, 1);
      HAL_GPIO_WritePin(LE_GPIO_Port, LE_Pin, 0);
      HAL_GPIO_WritePin(OE_GPIO_Port, OE_Pin, 0);
      delay_us(delayBrightness);
      HAL_GPIO_WritePin(OE_GPIO_Port, OE_Pin, 1);
    }
  } else { for (uint8_t y=0; y<2*PANEL_SCAN; y++) delay_us(delayBrightness); }
  HAL_TIM_Base_Start_IT(&htim2);
}

#if (CANVAS_MODE == 0)
  void hub75_begin(void) {
    hub75_setFonts(system5x8);
    displayReset();
  }

  void hub75_fillColor(uint8_t color) {
    hub75_setBackColor(color);
    sprintf(buff75, "               ");
  }

  void hub75_print(const char *str) { sprintf(buff75, "%s", str); }

#else
  static void drawPixel(uint8_t x, uint8_t y) {
    uint8_t xBit = 0x80>>x%8, xByte = x/8;
    if (pixelColor & 0x01) { rgbBuff[0][y][xByte] |= xBit; } else { rgbBuff[0][y][xByte] &= ~xBit; }
    if (pixelColor & 0x02) { rgbBuff[1][y][xByte] |= xBit; } else { rgbBuff[1][y][xByte] &= ~xBit; }
    if (pixelColor & 0x04) { rgbBuff[2][y][xByte] |= xBit; } else { rgbBuff[2][y][xByte] &= ~xBit; }
  }

  static void drawChar(const char chr) {
  uint16_t x, y, char_offset;
  uint8_t i, j;
  unsigned long long data;

  if(chr <= sFont.charLast) {
      j = sFont.fontH/8;      // bytes per char
      if(sFont.fontH%8 != 0)j++;
      char_offset = (chr-sFont.char1st)*(j*sFont.fontW+1);
      if((xPos+sFont.fontTable[char_offset]-1) >= PIXEL_X) return;
      for (x=1; x<=sFont.fontTable[char_offset]; x++) {
        data = 0;
        for(i=0; i<j; i++) {
          data |= (sFont.fontTable[char_offset+j*(x-1)+i+1]&0xff)<<(i*8);
        }
        if(x!=1 || data!=0) { // skip if starts with blank line
          for(y=0; y<(sFont.fontH+1); y++) {
            if((data>>y)&0x01) drawPixel(xPos, yPos);
            yPos++;
          }
          xPos++;
          yPos = yPos-(sFont.fontH+1);
        }
      }
      xPos++;                 // blank line at the end of char
    }
  }

  void hub75_begin(void) {
    hub75_setFonts(system6x8);
    hub75_setCursor(0, 0);
    displayReset();
  }

  void hub75_setCursor(uint8_t x, uint8_t y) { xPos = x, yPos = y; }

  void hub75_setPixel(uint8_t x, uint8_t y) { drawPixel(x, y); }

  void hub75_fillColor(uint8_t color) {
    for (uint8_t y=0; y<BUFF_Y; y++) {
      for (uint8_t x=0; x<BUFF_X; x++) {
        if (color&0x01) { rgbBuff[0][y][x] = 0xff; } else { rgbBuff[0][y][x] = 0x00; }
        if (color&0x02) { rgbBuff[1][y][x] = 0xff; } else { rgbBuff[1][y][x] = 0x00; }
        if (color&0x04) { rgbBuff[2][y][x] = 0xff; } else { rgbBuff[2][y][x] = 0x00; }
      }
    }
  }

  void hub75_fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color) {
    xPos = x; yPos = y;

    hub75_setColor(color);

    for (uint8_t y=yPos; y<h; y++) {
      for (uint8_t x=xPos; x<w; x++) drawPixel(x, y);
    }
  }

  void hub75_print(const char *str) { while (*str) { drawChar(*str++); } }

#endif
