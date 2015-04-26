// Demonstrate use of the DDS Library
// m0xpd
// http://m0xpd.blogspot.com
// March 2014

#include <Bounce2.h>
#include <Encoder.h>
#include <U8glib.h>
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

const int decrPin = 8;
Bounce decrbutton = Bounce();  // 10 ms debounce

const int incrPin = 9;
Bounce incrbutton = Bounce();  // 10 ms debounce


long freq = 5000000;
long dfreq = 1000;

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
  pinMode(incrPin, INPUT_PULLUP);
  pinMode(decrPin, INPUT_PULLUP);
  incrbutton.attach(incrPin);
  incrbutton.interval(10);
  decrbutton.attach(decrPin);
  decrbutton.interval(10);
  
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
  if( incrbutton.update() ) {
    if( incrbutton.read() ) {
      if (dfreq == 100) {
        dfreq = 1000;
      } 
      else if (dfreq == 1000) {
        dfreq = 10000;
      } 
      else if (dfreq == 10000) {
        dfreq = 100000;   
      }
      else if (dfreq == 100000) {
        dfreq = 100;   
      }
    }
  }
  
  if( decrbutton.update() ) {
    if( decrbutton.read() ) {
      if (dfreq == 100) {
        dfreq = 100000;
      } 
      else if (dfreq == 100000) {
        dfreq = 10000;
      } 
      else if (dfreq == 10000) {
        dfreq = 1000;   
      }
      else if (dfreq == 1000) {
        dfreq = 100;   
      }
    }
  }

  int pos = knob.read();
  if( pos != lastpos ) {
     freq += (pos - lastpos) * dfreq;
     lastpos = pos;
     setFrequency();
  } 
}
