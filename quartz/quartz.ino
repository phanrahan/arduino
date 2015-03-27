// Clock Tick Demonstration
//
// By Matt Mets, completed in 2008
//
// This code is released into the public domain.  Attribution is appreciated.
//
// This is a demonstration on how to control a cheapo clock mechanism with an Arduino.
// The clock mechanism works by using an electromagnet to pull a little fixed magnet,
// similar to how a DC motor works.  To control this with the Arduino, we need to hook a
// wire up to each side of the electromagnet (disconnect the exisiting clock circuity if
// possible).  Then, hook each of the wires to pins on the Arduino.  I chose pins 2 and 3
// for my circuit.  It is also a good idea to put a resistor (I chose 500 ohms) in series
// (between one of the wires and an Arduino pin), which will limit the amount of current
// that is applied.  Once the wires are hooked up, you take turns turning on one or the
// other pin momentarily.  Each time you do this, the clock 'ticks' and moves forward one
// second.  I have provided a doTick() routine to do this automatically, so it just needs
// to be called each time you want the clock to tick.
//
 
 
////// Board Setup /////////////////////////////////////////////////////////////////////////
extern unsigned long timer0_overflow_count;
 
int clockA = 8;
int clockB = 9;
 
int tickPin = clockA;    // This keeps track of which clock pin should be fired next.
 
 
// Initialize the IO ports
void setup()
{
  pinMode(clockA, OUTPUT);
  pinMode(clockB, OUTPUT);
 
  digitalWrite(clockA, LOW);
  digitalWrite(clockB, LOW);
}
 
 
// Move the second hand forward one position (one second on the clock face).
void doTick() {
 
  // Energize the electromagnet in the correct direction.
  digitalWrite(tickPin, HIGH);
  delay(40 );
  digitalWrite(tickPin, LOW);  
 
  // Switch the direction so it will fire in the opposite way next time.
  if (tickPin == clockA)
  {
    tickPin = clockB;
  } else {
    tickPin = clockA;
  }
}
 
 
// Main loop
void loop()
{
  unsigned long startTime = millis();
  unsigned long temp;
 
  // Pretend to be a regular clock, and tick once a second.
  while (true)
  {
    startTime += 1000;
 
    // Wait until a second has passed.  Note that this will do ugly things when millis()
    // runs over, so we only have about 9 hours before this version will stop working.
    while (startTime - millis() > 0) {}
 
    doTick();
  }
}
