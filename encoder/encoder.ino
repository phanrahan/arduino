#include <Encoder.h>

#define A_PIN       6
#define B_PIN       7
Encoder knob(A_PIN, B_PIN);

int lastposition = 0;

#define BUTTON_PIN  8
int lastpress = 1;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop()
{
  int position = knob.read();
  if( position != lastposition ) {
     Serial.println(position);
     lastposition = position;
  }

  int press = digitalRead(BUTTON_PIN);
  if( press != lastpress ) {
     Serial.println(press);
     lastpress = press;
  }
}
