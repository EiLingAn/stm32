#include "main.h"
// Setup for ST7565R in SPI mode
/** The chip select pin */

#define SPI_SOFTWARE_INTERFACE

#define GLCD_SDA(bit)     HAL_GPIO_WritePin(GLCD_SDA_GPIO_Port, GLCD_SDA_Pin, bit)
#define GLCD_SCL(bit)     HAL_GPIO_WritePin(GLCD_SCL_GPIO_Port, GLCD_SCL_Pin, bit)
#define GLCD_DC(bit)      HAL_GPIO_WritePin(GLCD_A0_GPIO_Port, GLCD_A0_Pin, bit)
#define GLCD_CS(bit)      HAL_GPIO_WritePin(GLCD_CS1_GPIO_Port, GLCD_CS1_Pin, bit)
#define GLCD_RST(bit)     HAL_GPIO_WritePin(GLCD_RST_GPIO_Port, GLCD_RST_Pin, bit)

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

/** Define this if your screen is incorrectly shifted by 4 pixels */
#define ST7565_REVERSE

/** By default we only write pages that have changed.  Undefine this
    if you want less/faster code at the expense of more SPI operations. */
//#define ST7565_DIRTY_PAGES
