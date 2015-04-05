#include "DualMC33926MotorShield.h"

const int POWER = 2;

DualMC33926MotorShield md;

void setup()
{
  Serial.begin(9600);
  
  pinMode(POWER, INPUT);
  digitalWrite(POWER, HIGH);
  
  // md uses A0 and A1 to read motor current
  md.init();
}

void loop()
{
  if( digitalRead(POWER) ) {
      digitalWrite(4, HIGH);
      while( digitalRead(POWER) ) {
          int speed = analogRead(A2) - 512;
          md.setM1Speed(speed);
          Serial.print("speed ");
          Serial.println(speed);
          delay(500);
          unsigned ma = md.getM1CurrentMilliamps();
          Serial.print("current (mA) ");
          Serial.println(ma);
          delay(500);
      }
      digitalWrite(4, LOW);
  }
}
