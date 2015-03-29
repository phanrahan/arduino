// Demonstrate use of the DDS Library
// m0xpd
// http://m0xpd.blogspot.com
// March 2014

#include "U8glib.h"
#include <Encoder.h>
#include <DDS.h>

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST);

//======================================
// AD9850 Module....
// set pin numbers:
const int W_CLK = 2;
const int FQ_UD = 3; 
const int DATA = 4;
const int RESET = 5;

// Change these pin numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder knob(6, 7);
int lastpos;

long freq = 5000000;
long dfreq = 100000;

// Instantiate the DDS...
DDS dds(W_CLK, FQ_UD, DATA, RESET);

void setFrequency()
{
  dds.setFrequency(freq);  
  
  u8g.firstPage();  
  do {
      u8g.setPrintPos( 10, 22 );
      u8g.print(freq);
  } while( u8g.nextPage() ); 
}


void setup() {
  lastpos = knob.read();
  
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
 
  dds.init();  
  dds.trim(124997000);   // (Optional) trim if your xtal is not at 125MHz...
    
  setFrequency();
}

void loop() {
  int pos = knob.read();
  
  if( pos != lastpos ) {
     freq += (pos - lastpos) * dfreq;
     lastpos = pos;
     setFrequency();
  } 
}
