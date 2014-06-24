/*
 Example 34.1 - SPI bus demo using a Microchip MCP4162 digital potentiometer 
    [http://bit.ly/iwDmnd]
    http://tronixstuff.com/tutorials > chapter 34 | CC by-sa-nc | John Boxall

    SS – digital 10
    MOSI – digital 11
    MISO – digital 12
    SCK – digital 13

*/

#include "SPI.h"
int del=1000;

void setup()
{
  pinMode(SS, OUTPUT);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
}

void setValue(int value)
{
  digitalWrite(SS, LOW);
  SPI.transfer( (value & 0x100) ? 1 : 0 );
  SPI.transfer( value & 0xff ); // send value (0~255)
  digitalWrite(SS, HIGH);
}

void loop()
{
  for (int a=0; a<=256; a+=1)
  {
    setValue(a);
    delay(del);
  }
}
