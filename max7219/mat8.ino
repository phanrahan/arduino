//
// Julian Ilett
//
// https://www.youtube.com/watch?v=tFDiMw5QKZQ
//
#define MAX7219_DIN 5
#define MAX7219_CS  6
#define MAX7219_CLK 7

void initialise()
{
  digitalWrite(MAX7219_CS, HIGH);
  pinMode(MAX7219_DIN, OUTPUT);
  pinMode(MAX7219_CS, OUTPUT);
  pinMode(MAX7219_CLK, OUTPUT);
}

void output(byte address, byte data)
{
  digitalWrite(MAX7219_CS, LOW);
  shiftOut(MAX7219_DIN, MAX7219_CLK, MSBFIRST, address);
  shiftOut(MAX7219_DIN, MAX7219_CLK, MSBFIRST, data);
  digitalWrite(MAX7219_CS, HIGH);
}

void setup() {
  // put your setup code here, to run once:
  initialise();
  output(0x0f, 0x00); //display test register - test mode off
  output(0x0c, 0x01); //shutdown register - normal operation
  output(0x0b, 0x07); //scan limit register - display digits 0 thru 7
  output(0x0a, 0x0f); //intensity register - max brightness
  output(0x09, 0xff); //decode mode register - CodeB decode all digits

  output(0x08, 0x0c); //digit 7 (leftmost digit) data
  output(0x07, 0x0b);
  output(0x06, 0x0d);
  output(0x05, 0x0e);
  output(0x04, 0x08);
  output(0x03, 0x07);
  output(0x02, 0x06);
  output(0x01, 0x05); //digit 0 (rightmost digit) data
}

void loop() {
  // put your main code here, to run repeatedly:
}
