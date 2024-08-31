#ifndef ___HUB75_H___
#define ___HUB75_H___

#define PANEL_H     32
#define PANEL_W     64
#define PANEL_SCAN  16

#define CANVAS_MODE  1

#define PIXEL_Y     PANEL_H * 1
#define PIXEL_X     PANEL_W * 2

#define BUFF_Y      PIXEL_Y
#define BUFF_X      PIXEL_X / 8

#define clBLACK     0X00
#define clRED       0X01
#define clGREEN     0X02
#define clYELLOW    0X03
#define clBLUE      0X04
#define clMAGENTA   0X05
#define clCYAN      0X06
#define clWHITE     0X07


typedef enum { RGB, RBG, GRB, GBR, BRG, BGR } rgbConfig_et;

typedef struct {
  uint8_t fontH;
  uint8_t fontW;
  uint8_t char1st;
  uint8_t charLast;
  const uint8_t *fontTable;
} font_st;


  extern void delay_init();
  extern void delay_ms(uint32_t ms);
  extern void delay_us(uint16_t us);

  void hub75_display(void);
  void hub75_setFonts(font_st sfont);
  void hub75_setRgbPin(uint8_t rgbPin);
  void hub75_setBackColor(uint8_t color);
  void hub75_setColor(uint8_t color);
  void hub75_setBrightness(uint8_t delayBright);
  void hub75_setDisplay(uint8_t isOn);

#if (CANVAS_MODE == 0)
  void hub75_begin(void);
  void hub75_fillColor(uint8_t color);
  void hub75_print(const char *str);
#else
  void hub75_display(void);
  void hub75_begin(void);
  void hub75_setCursor(uint8_t x, uint8_t y);
  void hub75_setPixel(uint8_t x, uint8_t y);
  void hub75_drawChar(const char chr);
  void hub75_fillColor(uint8_t color);
  void hub75_fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
  void hub75_print(const char *str);
#endif


#endif /* ___HUB75_H___ */
