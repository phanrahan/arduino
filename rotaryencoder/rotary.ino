/* Bounce-Free Rotary Encoder

   David Johnson-Davies - www.technoblogy.com - 28th October 2017
   ATtiny85 @ 1 MHz (internal oscillator; BOD disabled)
   
   CC BY 4.0
   Licensed under a Creative Commons Attribution 4.0 International license: 
   http://creativecommons.org/licenses/by/4.0/
*/

const int DataOut = 0;
const int ClockPin = 1;
const int LED = 2;
const int EncoderA = 3;
const int EncoderB = 4;

// Serial seven segment display **********************************************

const int Clear_Display = 0x76;
const int Cursor_Control = 0x79;
const int Brightness_Control = 0x7A;

void SendByte(char data) {
  shiftOut(DataOut, ClockPin, MSBFIRST, data);
}

// Clear display
void ClearDisplay () {
  SendByte(Clear_Display);
  SendByte(Brightness_Control);
  SendByte(255);
}

// Display byte
void Display (int number) {
  boolean dig = false;
  int j;
  SendByte(Cursor_Control); SendByte(0);
  j = 1000;
  for (int d=0; d<4 ; d++) {
    int i = (number/j) % 10;
    if (!i && !dig && j>1) SendByte(' ');
    else { SendByte(i); dig = true; }
    j=j/10;
  }
}

// Rotary encoder **********************************************

volatile int a0;
volatile int c0;
volatile int Count = 0;

// Called when encoder value changes
void ChangeValue (bool Up) {
  digitalWrite(LED, Up);
  Count = max(min((Count + (Up ? 1 : -1)), 1000), 0);
}

// Pin change interrupt service routine
ISR (PCINT0_vect) {
  int a = PINB>>EncoderA & 1;
  int b = PINB>>EncoderB & 1;
  if (a != a0) {              // A changed
    a0 = a;
    if (b != c0) {
      c0 = b;
      ChangeValue(a == b);
    }
  }
}

// Setup demo **********************************************

void setup() {
  pinMode(ClockPin, OUTPUT);
  pinMode(DataOut, OUTPUT);
  digitalWrite(ClockPin, LOW);
  digitalWrite(DataOut, LOW);
  pinMode(LED, OUTPUT);
  ClearDisplay();
  pinMode(EncoderA, INPUT_PULLUP);
  pinMode(EncoderB, INPUT_PULLUP);
  PCMSK = 1<<EncoderA;        // Configure pin change interrupt on A
  GIMSK = 1<<PCIE;            // Enable interrupt
  GIFR = 1<<PCIF;             // Clear interrupt flag
}

void loop() {
  Display(Count);
  delay(100);
}
