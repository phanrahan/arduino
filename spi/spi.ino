#include "SPI.h" 

int ss=10; 

const int DELAY=200; 
 
void setup()
{
  pinMode(ss, OUTPUT); 
  SPI.begin(); 
  SPI.setBitOrder(MSBFIRST);
}
 
void setValue(int value)
{
  digitalWrite(ss, LOW);
  SPI.transfer(value);
  digitalWrite(ss, HIGH);
}
 
void loop()
{
  for (int i=0; i<256; i++) {
    setValue(i);
    delay(DELAY);
  }
}
