/*
  Read the MCP3008 using the library created Uros Petrevki

  Created by Pat Hanrahan
  Released into the public domain.
*/

#include <MCP3008.h>

// define pin connections
#define SS 10
#define SCLK 13
#define MOSI 11
#define MISO_PIN 12

// put pins inside MCP3008 constructor
MCP3008 adc(SCLK, MOSI, MISO, SS);

void setup() {
 Serial.begin(9600);
}

void loop() {
  int val = adc.readADC(0); // read Channel 0 from MCP3008 ADC
  Serial.println(val);
  
  // iterate thru all channels
  /*
  for (int i=0; i<8; i++) {
   int val = adc.readADC(i);
   Serial.print(val);
   Serial.print("\t");
   }
   Serial.println("");
  */
  
}
