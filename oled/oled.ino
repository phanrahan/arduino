// Demonstrate use of the DDS Library
// m0xpd
// http://m0xpd.blogspot.com
// March 2014

#include <U8glib.h>


U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST);


void setup() {
    // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
  u8g.setFont(u8g_font_unifont);
  u8g.setRot180();
}

void loop() {
  int i = 0;
  while(1) {
      u8g.firstPage();
      do {
         u8g.setPrintPos( 10, 22 );
         u8g.print(i);
      } while( u8g.nextPage() );

      i += 1;
      delay(1000);
  }
}
