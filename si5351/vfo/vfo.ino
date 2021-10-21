//#include <Bounce2.h>
#include <Encoder.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <si5351.h>
//#include <SPI.h>

#define TFT_CS     10
#define TFT_RST    9  
#define TFT_DC     8

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// Change these pin numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder knob(6, 7);
int lastpos;

//const int decrPin = 4;
//Bounce decrbutton = Bounce();  // 10 ms debounce

//const int incrPin = 5;
//Bounce incrbutton = Bounce();  // 10 ms debounce

Si5351 si5351;

//int calvalue = 14;// the cal value for the TCXO
long freq = 5000000;
long dfreq = 100000;

void setFrequency()
{
  si5351.set_freq(freq, SI5351_CLK0);
  
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.print(freq);
}

void setup() {
  /*
  pinMode(incrPin, INPUT_PULLUP);
  pinMode(decrPin, INPUT_PULLUP);
  incrbutton.attach(incrPin);
  incrbutton.interval(10);
  decrbutton.attach(decrPin);
  decrbutton.interval(10);
  */
  
  lastpos = knob.read();

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);
  tft.setTextSize(3);

  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA);
  //si5351.set_correction(calvalue);
  si5351.output_enable(SI5351_CLK0, 0);
    
  setFrequency();
}

void loop() {
  /*
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
        dfreq = 1000000;   
      }
    }
  }
  
  if( decrbutton.update() ) {
    if( decrbutton.read() ) {
      if (dfreq == 1000000) {
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
  */
 
  int pos = knob.read();
  if( pos != lastpos ) {
     freq += (pos - lastpos) * dfreq;
     lastpos = pos;
     setFrequency();
  } 
}
