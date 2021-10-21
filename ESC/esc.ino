/*
        Arduino Brushless Motor Control
     by Dejan, https://howtomechatronics.com
*/

#include <Servo.h>

Servo ESC;     // create servo object to control the ESC

const int minthrottle=1064; // microseconds
const int maxthrottle=1864;

// to calibrate
//   set pot to the max
//   power up esc
//   wait for one beep
//   set pot to the min
//   wait for two beeps

void setup() {
  ESC.attach(9,minthrottle,maxthrottle); 
}

void loop() {
  int potValue = analogRead(A0); 
  // scale it to use it with the servo library (value between 0 and 180)
  potValue = map(potValue, 0, 1023, 0, 180);   
  ESC.write(potValue); 
}
