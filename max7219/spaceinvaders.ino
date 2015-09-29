/* SPACE INVADERS and LETTERS Animation for MAX7219 from maxim @ Funduino mini pro (Arduino mini pro clone), 
reduced and optimised for useing more then one 7219 in a row,
______________________________________

 Code History:
 --------------

The orginal code was written for the Wiring board by:
 * Nicholas Zambetti and Dave Mellis /Interaction Design Institute Ivrea /Dec 2004
 * http://www.potemkin.org/uploads/Wiring/MAX7219.txt

First modification by:
 * Marcus Hannerstig/  K3, malmï¿½ hï¿½gskola /2006
 * http://www.xlab.se | http://arduino.berlios.de

Second version is by:
 * tomek ness /FH-Potsdam / Feb 2007
 * http://design.fh-potsdam.de/ 

 * @acknowledgements: eric f. 


This version is by:
 * Juliano R. F. de Oliveira / November 2014
 * email: jrfoliveira@gmail.com
 * @acknowledgements: all previous authors and the below two sites who provide animations for aliens and letters.
 * Letters: Animation Tool for 8x8 LED Matrix http://www.diyode.com/mini8x8/
 * Aliens: Gemma Space Invaders https://learn.adafruit.com/trinket-slash-gemma-space-invader-pendant/animation
-----------------------------------

General notes: 


-if you are only using one max7219, then use the function maxSingle to control
 the little guy ---maxSingle(register (1-8), collum (0-255))

-if you are using more then one max7219, and they all should work the same, 
then use the function maxAll ---maxAll(register (1-8), collum (0-255))

-if you are using more than one max7219 and just want to change something
at one little guy, then use the function maxOne
---maxOne(Max you wane controll (1== the first one), register (1-8), 
collum (0-255))

/* During initiation, be sure to send every part to every max7219 and then
 upload it.
For example, if you have five max7219's, you have to send the scanLimit 5 times
before you load it-- other wise not every max7219 will get the data. the
function maxInUse  keeps track of this, just tell it how many max7219 you are
using.
*/

int dataIn = 2;
int load = 3;
int clock = 4;
int intensity = 0;

int maxInUse = 1;    //change this variable to set how many MAX7219's you'll use

int e = 0;           // just a varialble

                     // define max7219 registers
byte max7219_reg_noop        = 0x00;
byte max7219_reg_digit0      = 0x01;
byte max7219_reg_digit1      = 0x02;
byte max7219_reg_digit2      = 0x03;
byte max7219_reg_digit3      = 0x04;
byte max7219_reg_digit4      = 0x05;
byte max7219_reg_digit5      = 0x06;
byte max7219_reg_digit6      = 0x07;
byte max7219_reg_digit7      = 0x08;
byte max7219_reg_decodeMode  = 0x09;
byte max7219_reg_intensity   = 0x0a;
byte max7219_reg_scanLimit   = 0x0b;
byte max7219_reg_shutdown    = 0x0c;
byte max7219_reg_displayTest = 0x0f;

void putByte(byte data) {
  byte i = 8;
  byte mask;
  while(i > 0) {
    mask = 0x01 << (i - 1);      // get bitmask
    digitalWrite( clock, LOW);   // tick
    if (data & mask){            // choose bit
      digitalWrite(dataIn, HIGH);// send 1
    }else{
      digitalWrite(dataIn, LOW); // send 0
    }
    digitalWrite(clock, HIGH);   // tock
    --i;                         // move to lesser bit
  }
}

void maxSingle( byte reg, byte col) {    
//maxSingle is the "easy"  function to use for a     //single max7219

  digitalWrite(load, LOW);       // begin     
  putByte(reg);                  // specify register
  putByte(col);//((data & 0x01) * 256) + data >> 1); // put data   
  digitalWrite(load, LOW);       // and load da shit
  digitalWrite(load,HIGH); 
}

void maxAll (byte reg, byte col) {    // initialize  all  MAX7219's in the system
  int c = 0;
  digitalWrite(load, LOW);  // begin     
  for ( c =1; c<= maxInUse; c++) {
  putByte(reg);  // specify register
  putByte(col);//((data & 0x01) * 256) + data >> 1); // put data
    }
  digitalWrite(load, LOW);
  digitalWrite(load,HIGH);
}

void maxOne(byte maxNr, byte reg, byte col) {    
//maxOne is for adressing different MAX7219's, 
//whilele having a couple of them cascaded

  int c = 0;
  digitalWrite(load, LOW);  // begin     

  for ( c = maxInUse; c > maxNr; c--) {
    putByte(0);    // means no operation
    putByte(0);    // means no operation
  }

  putByte(reg);  // specify register
  putByte(col);//((data & 0x01) * 256) + data >> 1); // put data 

  for ( c =maxNr-1; c >= 1; c--) {
    putByte(0);    // means no operation
    putByte(0);    // means no operation
  }

  digitalWrite(load, LOW); // and load da shit
  digitalWrite(load,HIGH); 
}


void setup () {


  pinMode(dataIn, OUTPUT);
  pinMode(clock,  OUTPUT);
  pinMode(load,   OUTPUT);

  //Serial.begin(9600);
  //digitalWrite(13, HIGH);  

//initiation of the max 7219
  maxAll(max7219_reg_scanLimit, 0x07);      
  maxAll(max7219_reg_decodeMode, 0x00);  // using an led matrix (not digits)
  maxAll(max7219_reg_shutdown, 0x01);    // not in shutdown mode
  maxAll(max7219_reg_displayTest, 0x00); // no display test
   for (e=1; e<=8; e++) {    // empty registers, turn all LEDs off 
    maxAll(e,0);
  }
  maxAll(max7219_reg_intensity, 0x08 & 0x0f);    // the first 0x0f is the value you can set
                                                  // range: 0x00 to 0x0f
}  

int row=0, col=0;

#define rowMax 27
#define colMax 8

int valueMask[rowMax][colMax]={
//JULIANO - see Animation Tool for 8x8 LED Matrix http://www.diyode.com/mini8x8/
  {B00011100,B00001000,B00001000,B00001000,B11101000,B01001000,B01001000,B00110000},
  {B10000001,B10000001,B10000001,B10000001,B10000001,B10000001,B11000011,B01111110},
  {B10000000,B10000000,B10000000,B10000000,B10000000,B10000000,B10000000,B11111110},
  {B00111000,B00010000,B00010000,B00010000,B00010000,B00010000,B00010000,B00111000},
  {B00011000,B00100100,B01000010,B10000001,B11111111,B10000001,B10000001,B10000001},
  {B10000001,B11000001,B10100001,B10010001,B10001001,B10000101,B10000011,B10000001},
  {B00111100,B01100110,B10000001,B10000001,B10000001,B10000001,B01100110,B00111100},  
// Aliens - got the code from Gemma Space Invaders https://learn.adafruit.com/trinket-slash-gemma-space-invader-pendant/animation 
//  first alien .25s
  {B00011000,B00111100,B01111110,B11011011,B11111111,B00100100,B01011010,B10100101},
  {B00011000,B00111100,B01111110,B11011011,B11111111,B00100100,B01011010,B01000010},
  {B00011000,B00111100,B01111110,B11011011,B11111111,B00100100,B01011010,B10100101},
  {B00011000,B00111100,B01111110,B11011011,B11111111,B00100100,B01011010,B01000010},
//  second alien .25s
  {B00000000, B00111100, B01111110, B11011011, B11011011, B01111110, B00100100, B11000011},
  {B00111100, B01111110, B11011011, B11011011, B01111110, B00100100, B00100100, B00100100},
  {B00000000, B00111100, B01111110, B11011011, B11011011, B01111110, B00100100, B11000011},
  {B00111100, B01111110, B11011011, B11011011, B01111110, B00100100, B00100100, B00100100},
//  third alien .25s
  {B00100100, B00100100, B01111110, B11011011, B11111111, B11111111, B10100101, B00100100},
  {B00100100, B10100101, B11111111, B11011011, B11111111, B01111110, B00100100, B01000010},
  {B00100100, B00100100, B01111110, B11011011, B11111111, B11111111, B10100101, B00100100},
  {B00100100, B10100101, B11111111, B11011011, B11111111, B01111110, B00100100, B01000010},
// fourth alien .12s  
  {B00111100, B01111110, B00110011, B01111110, B00111100, B00000000, B00001000, B00000000},
  {B00111100, B01111110, B10011001, B01111110, B00111100, B00000000, B00001000, B00001000},
  {B00111100, B01111110, B11001100, B01111110, B00111100, B00000000, B00000000, B00001000},
  {B00111100, B01111110, B01100110, B01111110, B00111100, B00000000, B00000000, B00000000},
  {B00111100, B01111110, B00110011, B01111110, B00111100, B00000000, B00001000, B00000000},
  {B00111100, B01111110, B10011001, B01111110, B00111100, B00000000, B00001000, B00001000},
  {B00111100, B01111110, B11001100, B01111110, B00111100, B00000000, B00000000, B00001000},
  {B00111100, B01111110, B01100110, B01111110, B00111100, B00000000, B00000000, B00000000}
  
};

void loop () {

  // concatenated FOR loop to made the code in a more simple way
  for(row=0;row<rowMax;row++)
  {
    for(col=0;col<colMax;col++)
    {
      maxSingle(col+1,valueMask[row][col]);
    }
    
    // Delays between the frames (all 250ms, but for the last alien is 120ms)
    if(row<(rowMax-8))
    {
      delay(250);
    }
    else
    {
      delay(120);
    }
    
    //Blink Funduino LED just to see that the microcontroller is alive
    if(row % 2 == 0)
    {
      digitalWrite(13, LOW);
    }
    else
    {
      digitalWrite(13, HIGH);
    }
  }
  
}
