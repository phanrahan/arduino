#include <OneWire.h>

// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!
// https://github.com/milesburton/Arduino-Temperature-Control-Library

OneWire  ds(10);  // on pin 10 (a 4.7K resistor is necessary)

void setup(void) {
  Serial.begin(115200);
  Serial.println("Scanning for onewire devices");
}

void loop(void) {
  byte i;
  byte present = 0;
  byte data[9];
  byte addr[8];
  float celsius, fahrenheit;
  
  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ds.reset_search();
    delay(2000);
    return;
  }
  
  Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  Serial.println();
 
  // the first ROM byte indicates chip family
  switch (addr[0]) {
    case 0x10:
      Serial.println("  DS18S20 Temperature");  // or old DS1820
      break;
    case 0x28:
      Serial.println("  DS18B20 Temperature");
      break;
    case 0x22:
      Serial.println("  DS1822 Temperature");
      break;
    case 0x1D:
      Serial.println("  DS2423 Dual Counter");
      break;
    case 0x26:
      Serial.println("  DS2438 Battery Monitor");
      break;
    case 0x20:
      Serial.println("  DS2450 Quad ADC");
      break;
    default:
      Serial.println("  Unrecognizable device.");
      return;
  }
}
