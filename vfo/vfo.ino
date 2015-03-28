// Demonstrate use of the DDS Library
// m0xpd
// http://m0xpd.blogspot.com
// March 2014

#include "U8glib.h"
#include <DDS.h>

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST);

//======================================
// AD9850 Module....
// set pin numbers:
const int W_CLK = 2;
const int FQ_UD = 3; 
const int DATA = 4;
const int RESET = 5;

const int encoder0PinA = 6;
const int encoder0PinB = 7;

int ALast;
int BLast;

double freq = 5000000;
double dfreq = 100000;

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
  DDRB|=0x21;        
  PORTB |= 0x21;
  
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
  
  pinMode (encoder0PinA,INPUT);
  pinMode (encoder0PinB,INPUT);
  digitalWrite (encoder0PinA, HIGH);
  digitalWrite (encoder0PinB, HIGH);
  ALast = digitalRead(encoder0PinA);
  BLast = digitalRead(encoder0PinB);
    
  setFrequency();
}

void loop() {
  int A = digitalRead(encoder0PinA);
  int B = digitalRead(encoder0PinB);
  if (ALast != A || BLast != B) {
     if (ALast ^ B) {
       freq -= dfreq;
     } 
     else {
       freq += dfreq;
     }
     ALast = A;
     BLast = B;
     setFrequency();
  } 
}
