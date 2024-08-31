/*
 * appMain.c
 *
 *  Created on: Jun 10, 2024
 *      Author: BUDZILLA
 */

#include "main.h"
#include "st7565.h"
#include "graphics.h"

#include "fonts/font_tahoma.h"

#include <stdio.h>

void setup(void) {
  // Initialise the screen, turning it on
  glcd_init();
  // Clear the screen's internal memory
  glcd_blank();
  // Set the screen's brightness, if required.
  glcd_contrast(3, 31);
}

void loop(void) {
  char buff[100];
  for (uint8_t i=0; i<10; i++) {
    sprintf(buff, "Example string %d", i);
    // Draw some text with spacing 1, at location (10,10)
    draw_text(buff, 10, 10, Tahoma10, 1);
    // Draw a rectangle from (1,1) to (50,50)
    draw_rectangle(1, 1, 50, 50, 1);
    // Draw a circle, centred at (32,32) with radius 10
    draw_circle(32, 32, 10, 1);

    draw_text("TAHOMA32", 0, 30, Tahoma32, 1);

    glcd_refresh();
    delay_ms(1000);
  }
}

