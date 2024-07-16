// define led according to pin diagram
//
// https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/
int led = D10;

void setup() {
  pinMode(led, OUTPUT);
}

void loop() {
  digitalWrite(led, HIGH);   // turn the LED on 
  delay(1000);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off
  delay(1000);               // wait for a second
}
