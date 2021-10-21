/* Stepper Unipolar Advanced
 * -------------------------
 */
#include <WProgram.h>

int motorPins[] = {8, 9, 10, 11};
int count = 0;
int count2 = 0;
int delayTime = 200;
int val = 0;

void setup() {
  for (count = 0; count < 4; count++) {
    pinMode(motorPins[count], OUTPUT);
  }
}

void moveForward() {
  if ((count2 == 0) || (count2 == 1)) {
    count2 = 16;
  }
  count2>>=1;
  for (count = 3; count >= 0; count--) {
    digitalWrite(motorPins[count], count2>>count&0x01);
  }
  delay(delayTime);
}

void moveBackward() {
  if ((count2 == 0) || (count2 == 1)) {
    count2 = 16;
  }
  count2>>=1;
  for (count = 3; count >= 0; count--) {
    digitalWrite(motorPins[3 - count], count2>>count&0x01);
  }
  delay(delayTime);
}

void loop() {
  int i;
  for( i = 0; i < 96; i++ )
    moveForward();
  for( i = 0; i < 96; i++ )
    moveBackward();
}
