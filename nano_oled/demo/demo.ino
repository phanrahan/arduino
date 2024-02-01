#include <Arduino.h>
#include <U8g2lib.h>
#ifdef UX8xX8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef UX8xX8_HAVE_HW_I2C
#include <wire.h>
#endif

U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, A5,A4, U8X8_PIN_NONE);

void setup() {
    // put your setup code here, to run once:
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
