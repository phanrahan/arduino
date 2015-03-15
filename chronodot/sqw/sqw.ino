#include <Wire.h>

#define ds3231   0x68
#define ctrlReg  0x0E
// bits 4 & 3 in ctrlByte set to 0 = 1Hz
#define ctrlByte 0b00000000 

void setup()
{
  Wire.begin();
  Wire.beginTransmission(ds3231); 
  Wire.write(ctrlReg); 
  Wire.write(ctrlByte);
  Wire.endTransmission();
}

void loop() { 
  ; 
}

