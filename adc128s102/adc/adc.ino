#include <config.h>
#include <coolant_control.h>
#include <defaults.h>
#include <eeprom.h>
#include <gcode.h>
#include <limits.h>
#include <motion_control.h>
#include <nuts_bolts.h>
#include <pin_map.h>
#include <planner.h>
#include <print.h>
#include <protocol.h>
#include <report.h>
#include <serial.h>
#include <settings.h>
#include <spindle_control.h>
#include <stepper.h>

/*
  Created by Pat Hanrahan
  Released into the public domain.
*/

#include "SPI.h"

void setup() {
 pinMode(SS, OUTPUT);
 SPI.begin();
 SPI.setBitOrder(MSBFIRST);
 Serial.begin(9600);
}

int readADC(int chan)
{
  digitalWrite(SS, LOW);
  SPI.transfer( 0x01 );
  int hi = SPI.transfer( (chan & 0x7) << 4 );
  int lo = SPI.transfer( 0 );
  digitalWrite(SS, HIGH);

  return (hi << 8) | lo;
}

void loop() 
{
  int val = readADC(0); 
  Serial.println(val);
}
