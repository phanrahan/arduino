#include <Wire.h>

#define intPin   2
#define intNum   0
#define ds3231   0x68
#define ctrlReg  0x0E
// bits 4 & 3 in ctrlByte set to 0 = 1Hz
#define ctrlByte 0b00000000 

volatile unsigned int c = 0;

void setup()
{
  pinMode(intPin, INPUT);
  //Use built-in pull-up resistor on D2
  digitalWrite(intPin, HIGH);
  attachInterrupt(intNum, handleInterrupt, FALLING);
  Wire.begin();
  Serial.begin(115200);
  Wire.beginTransmission(ds3231); 
  Wire.write(ctrlReg); 
  Wire.write(ctrlByte);
  Wire.endTransmission();
}

void handleInterrupt() {
  Serial.println(++c);
}

void loop() { ; }
