#include <U8g2lib.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

void setup() {
    u8g2.begin();
}

void loop() {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0,10,"Hello");
    u8g2.drawStr(0,25,"Nano V3 0.91'' OLED");
    u8g2.sendBuffer();
    delay(1000);
}
