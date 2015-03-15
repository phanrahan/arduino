/*
    SS – digital 10
    MOSI – digital 11
    MISO – digital 12
    SCK – digital 13
*/

#include "SPI.h" // necessary library
//int SS=10; // using digital pin 10 for SPI slave select

void setup()
{
  pinMode(SS, OUTPUT); // we use this for SS pin
 
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
}

/*
** the value associated with chan will be returned 
** during the next call to this function.
*/
int readADC(int chan)
{
  digitalWrite(SS, LOW);
  int hi = SPI.transfer( chan << 3 );
  int lo = SPI.transfer( 0 );
  digitalWrite(SS, HIGH);
  
  return (hi << 8) | lo;
}

void loop()
{
  int val = readADC(0);
  Serial.println(val);
  
  // iterate thru all channels
  /*
  for (int i=0; i<8; i++) {
    int chan = (i == 7) ? 0 : i+1
    int val = adc.readADC(chan);
    Serial.print(val);
    Serial.print("\t");
  }
  Serial.println("");
  */
}
