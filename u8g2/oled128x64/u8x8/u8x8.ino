/*

  u8x8.ino

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

*/

#include <U8x8lib.h>

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

void setup(void) {
  u8x8.begin();  
}

void loop(void) {
  u8x8.clearDisplay();    
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0,20,"Hello World!");
  delay(100);
}

