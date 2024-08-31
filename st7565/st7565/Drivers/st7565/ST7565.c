/*
 $Id:$

 ST7565 LCD library!

 Copyright (C) 2010 Limor Fried, Adafruit Industries

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 // some of this code was written by <cstone@pobox.com> originally; it is in the public domain.
 */

#include "main.h"
#include "ST7565.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "glcdfont.h"

#define STARTBYTES 0

extern SPI_HandleTypeDef hspi2;

uint8_t is_reversed = 0;

// a handy reference to where the pages are on the screen
const uint8_t pagemap[] = { 7, 6, 5, 4, 3, 2, 1, 0 };

// a 5x7 font table
extern const uint8_t font[];

// the memory buffer for the LCD
uint8_t glcd_buffer[1024] = { 0 };

#define enablePartialUpdate

#ifdef enablePartialUpdate
  static uint8_t xUpdateMin, xUpdateMax, yUpdateMin, yUpdateMax;
#endif

static void glcd_updateBoundingBox(uint8_t xmin, uint8_t ymin, uint8_t xmax, uint8_t ymax) {
#ifdef enablePartialUpdate
  if (xmin < xUpdateMin)
    xUpdateMin = xmin;
  if (xmax > xUpdateMax)
    xUpdateMax = xmax;
  if (ymin < yUpdateMin)
    yUpdateMin = ymin;
  if (ymax > yUpdateMax)
    yUpdateMax = ymax;
#endif
}

static void glcd_init(void) {
  GLCD_CS(1);
  GLCD_RST(0);
  delay_ms(500);
  GLCD_RST(1);

  glcd_command(CMD_SET_BIAS_7);                // LCD bias select
  glcd_command(CMD_SET_ADC_NORMAL);            // ADC select
  glcd_command(CMD_SET_COM_NORMAL);            // SHL select
  glcd_command(CMD_SET_DISP_START_LINE);       // Initial display line
  glcd_command(CMD_SET_POWER_CONTROL | 0x4); // turn on voltage converter (VC=1, VR=0, VF=0)
  delay_ms(50);                                          // wait for 50% rising
  glcd_command(CMD_SET_POWER_CONTROL | 0x6); // turn on voltage regulator (VC=1, VR=1, VF=0)
  delay_ms(50);                                          // wait >=50ms
  glcd_command(CMD_SET_POWER_CONTROL | 0x7); // turn on voltage follower (VC=1, VR=1, VF=1)
  delay_ms(10);                                          // wait
  glcd_command(CMD_SET_RESISTOR_RATIO | 0x6); // set lcd operating voltage (regulator resistor, ref voltage resistor)
  // initial display line, set page address, set column address, write display data
  // set up a bounding box for screen updates
  glcd_updateBoundingBox(0, 0, LCDWIDTH - 1, LCDHEIGHT - 1);
}

void glcd_command(uint8_t c) {
  GLCD_DC(0);
  HAL_SPI_Transmit(&hspi2, &c, 1, 100);
}

void glcd_data(uint8_t c) {
  GLCD_DC(1);
  HAL_SPI_Transmit(&hspi2, &c, 1, 100);
}

void glcd_drawbitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color) {
  uint8_t i, j;
  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++) {
      if (bitmap[i + (j / 8) * w] & (1 << (j % 8))) { glcd_setpixel(x + i, y + j, color); }
    }
  }
  glcd_updateBoundingBox(x, y, x + w, y + h);
}

void glcd_drawstring(uint8_t x, uint8_t line, char *c) {
  while (c[0] != 0) {
    glcd_drawchar(x, line, c[0]);
    c++;
    x += 6; // 6 pixels wide
    if (x + 6 >= LCDWIDTH) {
      x = 0;    // ran out of this line
      line++;
    }
    if (line >= (LCDHEIGHT / 8))
      return;        // ran out of space :(
  }
}

void glcd_drawstring_P(uint8_t x, uint8_t line, const char *str) {
  char c;
  while (1) {
    c = *str++;
    if (!c)
      return;
    glcd_drawchar(x, line, c);
    x += 6; // 6 pixels wide
    if (x + 6 >= LCDWIDTH) {
      x = 0;    // ran out of this line
      line++;
    }
    if (line >= (LCDHEIGHT / 8))
      return;        // ran out of space :(
  }
}

void glcd_drawchar(uint8_t x, uint8_t line, char c) {
  uint8_t i;
  for (i = 0; i < 5; i++) {
    glcd_buffer[x + (line * 128)] = font[((unsigned char) c * 5) + i];
    x++;
  }

  glcd_updateBoundingBox(x - 5, line * 8, x - 1, line * 8 + 8);
}

// bresenham's algorithm - thx wikpedia
void glcd_drawline(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
    uint8_t color) {
  uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  // much faster to put the test here, since we've already sorted the points
  glcd_updateBoundingBox(x0, y0, x1, y1);

  uint8_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int8_t err = dx / 2;
  int8_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0 <= x1; x0++) {
    if (steep) {
      glcd_setpixel(y0, x0, color);
    } else {
      glcd_setpixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

// filled rectangle
void glcd_fillrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color) {
  uint8_t i, j;

  // stupidest version - just pixels - but fast with internal buffer!
  for (i = x; i < x + w; i++) {
    for (j = y; j < y + h; j++) {
      glcd_setpixel(i, j, color);
    }
  }

  glcd_updateBoundingBox(x, y, x + w, y + h);
}

// draw a rectangle
void glcd_drawrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color) {
  uint8_t i;
  // stupidest version - just pixels - but fast with internal buffer!
  for (i = x; i < x + w; i++) {
    glcd_setpixel(i, y, color);
    glcd_setpixel(i, y + h - 1, color);
  }
  for (i = y; i < y + h; i++) {
    glcd_setpixel(x, i, color);
    glcd_setpixel(x + w - 1, i, color);
  }

  glcd_updateBoundingBox(x, y, x + w, y + h);
}

// draw a circle outline
void glcd_drawcircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color) {
  glcd_updateBoundingBox(x0 - r, y0 - r, x0 + r, y0 + r);

  int8_t f = 1 - r;
  int8_t ddF_x = 1;
  int8_t ddF_y = -2 * r;
  int8_t x = 0;
  int8_t y = r;

  glcd_setpixel(x0, y0 + r, color);
  glcd_setpixel(x0, y0 - r, color);
  glcd_setpixel(x0 + r, y0, color);
  glcd_setpixel(x0 - r, y0, color);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    glcd_setpixel(x0 + x, y0 + y, color);
    glcd_setpixel(x0 - x, y0 + y, color);
    glcd_setpixel(x0 + x, y0 - y, color);
    glcd_setpixel(x0 - x, y0 - y, color);

    glcd_setpixel(x0 + y, y0 + x, color);
    glcd_setpixel(x0 - y, y0 + x, color);
    glcd_setpixel(x0 + y, y0 - x, color);
    glcd_setpixel(x0 - y, y0 - x, color);
  }
}

void glcd_fillcircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color) {
  glcd_updateBoundingBox(x0 - r, y0 - r, x0 + r, y0 + r);

  int8_t f = 1 - r;
  int8_t ddF_x = 1;
  int8_t ddF_y = -2 * r;
  int8_t x = 0;
  int8_t y = r;
  uint8_t i;

  for (i = y0 - r; i <= y0 + r; i++) {
    glcd_setpixel(x0, i, color);
  }

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    for (i = y0 - y; i <= y0 + y; i++) {
      glcd_setpixel(x0 + x, i, color);
      glcd_setpixel(x0 - x, i, color);
    }
    for (i = y0 - x; i <= y0 + x; i++) {
      glcd_setpixel(x0 + y, i, color);
      glcd_setpixel(x0 - y, i, color);
    }
  }
}

// the most basic function, set a single pixel
void glcd_setpixel(uint8_t x, uint8_t y, uint8_t color) {
  if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
    return;

  // x is which column
  if (color)
    glcd_buffer[x + (y / 8) * 128] |= (1 << (7 - (y % 8)));
  else
    glcd_buffer[x + (y / 8) * 128] &= ~(1 << (7 - (y % 8)));

  glcd_updateBoundingBox(x, y, x, y);
}

void glcd_set_brightness(uint8_t val) {
  glcd_command(CMD_SET_VOLUME_FIRST);
  glcd_command(CMD_SET_VOLUME_SECOND | (val & 0x3f));
}

// the most basic function, get a single pixel
uint8_t glcd_getpixel(uint8_t x, uint8_t y) {
  if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
    return 0;

  return (glcd_buffer[x + (y / 8) * 128] >> (7 - (y % 8))) & 0x1;
}

void glcd_begin(uint8_t contrast) {
  delay_init();
  glcd_init();
  glcd_command(CMD_DISPLAY_ON);
  glcd_command(CMD_SET_ALLPTS_NORMAL);
  glcd_set_brightness(contrast);
}

void glcd_display(void) {
  uint8_t col, maxcol, p;

  for (p = 0; p < 8; p++) {
#ifdef enablePartialUpdate
    if (yUpdateMin >= ((p + 1) * 8)) {
      continue;
    }  // nope, skip it!
    if (yUpdateMax < p * 8) {
      break;
    }
#endif
    delay_ms(1);
    glcd_command(CMD_SET_PAGE | pagemap[p]);
    delay_ms(1);  //DelayuS(100);
#ifdef enablePartialUpdate
    col = xUpdateMin;
    maxcol = xUpdateMax;
#else
    // start at the beginning of the row
    col = 0;
    maxcol = LCDWIDTH;
#endif

    glcd_command(CMD_SET_COLUMN_LOWER | ((col + STARTBYTES) & 0xf));
    delay_ms(1);
    glcd_command(
        CMD_SET_COLUMN_UPPER | (((col + STARTBYTES) >> 4) & 0x0F));
    delay_ms(1);
    glcd_command(CMD_RMW);
    delay_ms(1);  //DelayuS(100);

    for (; col < maxcol; col++) {
      glcd_data(glcd_buffer[(128 * p) + col]);
    }
  }
#ifdef enablePartialUpdate
  xUpdateMin = LCDWIDTH;  // - 1;
  xUpdateMax = 0;
  yUpdateMin = LCDHEIGHT;  //-1;
  yUpdateMax = 0;
#endif
}

// clear everything
void glcd_clear(void) {
  memset(glcd_buffer, 0, 1024);
  glcd_updateBoundingBox(0, 0, LCDWIDTH - 1, LCDHEIGHT - 1);
}

// this doesnt touch the buffer, just clears the display RAM - might be handy
void glcd_clear_display(void) {
  uint8_t p, c;

  for (p = 0; p < 8; p++) {
    glcd_command(CMD_SET_PAGE | p);
    for (c = 0; c < 128; c++) {
      glcd_command(CMD_SET_COLUMN_LOWER | (c & 0xf));
      glcd_command(CMD_SET_COLUMN_UPPER | ((c >> 4) & 0xf));
      glcd_data(0x0);
    }
  }
}
