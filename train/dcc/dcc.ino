#include <NmraDcc.h>

#define DCC_PIN 2

NmraDcc Dcc;

void notifyDccAccTurnoutOutput(uint16_t Addr, uint8_t Direction, uint8_t OutputPower) {

  Serial.print("Addr: ");Serial.print(Addr);
  Serial.print("Direction: ");Serial.print(Direction);
  Serial.print("OutputPower: ");Serial.print(OutputPower);
  Serial.println("-----------------------------");
  
}


void setup() {
Serial.begin(115200);
pinMode(RED_LED, OUTPUT);
pinMode(GREEN_LED, OUTPUT);
 Dcc.pin(digitalPinToInterrupt(DCC_PIN),DCC_PIN, false);
Dcc.init( MAN_ID_DIY, 10, CV29_ACCESSORY_DECODER | CV29_OUTPUT_ADDRESS_MODE, 0 );

}

void loop() {
  Dcc.process();


}
