/*

  u8g2.ino

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)
   
*/

#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

void setup(void) {
  u8g2.begin();  
}

void loop(void) {
  u8g2.clearBuffer();    
  //u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.setFont(u8g2_font_logisoso16_tr);
  u8g2.drawStr(0,20,"Hello World!");
  u8g2.sendBuffer();
  delay(1000);
}

