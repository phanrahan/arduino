const int evanePin = D2;

void setup() {
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  int evane = analogRead(evanePin);
  Serial.println(evane);
  delay(500);
}