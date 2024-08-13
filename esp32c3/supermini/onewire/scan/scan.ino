/*
  Arduino One Wire Search
  =======================
  
  Processing code for Ardunio to search for 1-wire devices and outputs address in hex to serial.
  
  - Adapted by Gordon Turner from code by hacktronics:
    http://www.hacktronics.com/Tutorials/arduino-1-wire-address-finder.html
 
  - 1-Wire temperature sensor attached to digital pin 3.
  
  - Requires 1-wire Ardunio Library and Dallas Temperature Control Arduino Library.
  - Included in Libraries folder or online:
      http://www.pjrc.com/teensy/arduino_libraries/OneWire.zip
      http://milesburton.com/Dallas_Temperature_Control_Library#Latest
 
  - Sample serial output:

  Searching for 1-wire devices.
  Found 1-Wire device with address: 0x28, 0x85, 0x95, 0x3A, 0x04, 0x00, 0x00, 0xB7
  No more devices found.

 
*/

#include <OneWire.h>

// 1-Wire data wire is attached to ditigal pin 3.
#define ONE_WIRE_BUS 10

// Intialize an 1-Wire instance.
OneWire oneWire(ONE_WIRE_BUS);

void setup(void) 
{
  // Open serial communications and wait for port to open.
  Serial.begin(115200);
  
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
  
  Serial.println("Searching for 1-wire devices.");
  
  while( oneWire.search(addr) ) 
  {
    // TODO: Test with more then one 1-wire device.
  
    Serial.print("Found 1-Wire device with address: ");
  
    for( i = 0; i < 8; i++) 
    {
      Serial.print("0x");
      
      if (addr[i] < 16) 
      {
        Serial.print('0');
      }
      
      Serial.print(addr[i], HEX);
      
      if (i < 7) 
      {
        Serial.print(", ");
      }
      else
      {
        Serial.println();
      }
    }
    
    if ( OneWire::crc8( addr, 7) != addr[7]) 
    {
        Serial.println("Error: CRC is invalid.");
        return;
    }
  }
  
  Serial.println("No more devices found.");
  
  oneWire.reset_search();
  return;
}


void loop()
{
  // Empty
}


