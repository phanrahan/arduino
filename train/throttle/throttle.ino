#include "DualMC33926MotorShield.h"

DualMC33926MotorShield md;

void setup()
{
  Serial.begin(9600);
  // md uses A0 and A1 to read motor current
  md.init();
}

void loop()
{
  int speed = analogRead(A2) - 512;
  md.setM1Speed(speed);
  Serial.println(speed);
  delay(500);
}
