int anemometer = D3;
int count = 0;

void setup() {
  Serial.begin(9600);
  pinMode(anemometer, INPUT_PULLUP);
}

static void wait_for_falling_edge(int gpio) {
  while (digitalRead(gpio) != 1) {} // wait until gpio is high
  while (digitalRead(gpio) == 1) {} // wait until gpio is low
}

void loop() {
  wait_for_falling_edge(anemometer);
  count++;
  Serial.println(count);
}
