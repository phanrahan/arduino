/*
 *   DS2423Counter
 *
 *   This example demonstrates the use of the DS2423 library and the Arduino
 *   OneWire library to read a counter value from a Dallas Semiconductor DS2423.
 *
 *   by Joe Bechter
 *
 *   (C) 2012, bechter.com
 *
 *   All files, software, schematics and designs are provided as-is with no warranty.
 *   All files, software, schematics and designs are for experimental/hobby use.
 *   Under no circumstances should any part be used for critical systems where safety,
 *   life or property depends upon it. You are responsible for all use.
 *   You are free to use, modify, derive or otherwise extend for your own non-commercial purposes provided
 *       1. No part of this software or design may be used to cause injury or death to humans or animals.
 *       2. Use is non-commercial.
 *       3. Credit is given to the author (i.e. portions © bechter.com), and provide a link to the original source.
 *
 */

#include <Arduino.h>
#include <OneWire.h>
#include <DS2423.h>

// define the Arduino digital I/O pin to be used for the 1-Wire network here
const uint8_t ONE_WIRE_PIN = 10;

// 1-Wire address of the DS2423 counter in the rain gauge
// uint8_t DS2423_address[] = { 0x1D, 0xD0, 0xE6, 0x0D, 0x00, 0x00, 0x00, 0xDC };
// 1-Wire address of the DS2423 counter in the anemometer
uint8_t DS2423_address[] = { 0x1D, 0x59, 0xEA, 0x0D, 0x00, 0x00, 0x00, 0x81 };

OneWire ow(ONE_WIRE_PIN);
DS2423 ds2423(&ow, DS2423_address);

void setup() {
    Serial.begin(115200);
    ds2423.begin(DS2423_COUNTER_B);
}

void loop() {
    ds2423.update();
    if (ds2423.isError()) {
        Serial.println("Error reading counter");
    } else {
        Serial.print("Count = ");
        Serial.println(ds2423.getCount(DS2423_COUNTER_B));
    }
    delay(1000);
}

