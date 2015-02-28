// beacon
// 
// Demonstrate use of the DDS Library
// m0xpd
// http://m0xpd.blogspot.com
// March 2014

// include the DDS Library:
#include <Wire.h>
#include <SeeedOLED.h>
#include <DDS.h>

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
  
  SeeedOled.clearDisplay();  
  SeeedOled.setTextXY(2,5);
  SeeedOled.putNumber(freq); 
}


void setup() {
  Wire.begin();
  SeeedOled.init();  //initialze SEEED OLED display
  DDRB|=0x21;        
  PORTB |= 0x21;
  SeeedOled.setNormalDisplay();      //Set display to normal mode (i.e non-inverse mode)
  SeeedOled.setPageMode();           //Set addressing mode to Page Mode
 
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
