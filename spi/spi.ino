#include "SPI.h" 

int ss=10; 

const int DELAY=100; 
 
void setup()
{
  Serial.begin(9600);
  pinMode(ss, OUTPUT); 
  SPI.begin(); 
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV16); // 1Mhz
}
 
int setValue(int value)
{
  digitalWrite(ss, LOW);
  int result = SPI.transfer(value);
  digitalWrite(ss, HIGH);
  return result;
}
 
void loop()
{
  for (int i=0; i<256; i++) {
    int result = setValue(i);
    Serial.print(result);
    Serial.println();
    delay(DELAY);
  }
}
