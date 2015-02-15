/*
 Reading Grizzly iGaging Digital Scales V2.1

 Created 19 January 2012
 Updated 03 April 2013
 by Yuriy Krushelnytskiy
 http://www.yuriystoys.com
 
 This example code is is licensed under a Creative Commons Attribution-ShareAlike 3.0 Unported License. 
 */
 void tickTock();

int const clockPin = 2;

int const xDataPin = 3;
int const yDataPin = 4;
int const zDataPin = 5;

//variables that will store the readout
volatile long xCoord;
volatile long yCoord;
volatile long zCoord;

void setup() {         

  //clock pin should be set as output  
  pinMode(clockPin, OUTPUT); 
  
  //data pins should be set as inputs
  pinMode(xDataPin, INPUT);  
  pinMode(yDataPin, INPUT);  
  pinMode(zDataPin, INPUT);  
  
  //initialize serial port
  Serial.begin(9600);
}



void loop() {
  
  xCoord = 0;
  yCoord = 0;
  zCoord = 0;
  
  int bitOffset;
  
  //read the first 20 bits
  for(bitOffset = 0; bitOffset<21; bitOffset++)
  {
    tickTock();
    
    //read the pin state and shif it into the appropriate variables
    xCoord |= ((long)digitalRead(xDataPin)<<bitOffset);
    yCoord |= ((long)digitalRead(yDataPin)<<bitOffset);
    zCoord |= ((long)digitalRead(zDataPin)<<bitOffset);
    
  }
  
  tickTock();

  //read the last bit (signified the sign)
  //if it's high, fill 11 leftmost bits with "1"s
  if(digitalRead(xDataPin)==HIGH)
        xCoord |= ((long)0x7ff << 21);
        
  if(digitalRead(yDataPin)==HIGH)
        yCoord |= ((long)0x7ff << 21);
        
  if(digitalRead(zDataPin)==HIGH) 
        zCoord |= ((long)0x7ff << 21);
  
  //print the position to the serial port (for debuging purposes)
  Serial.print("X");
  Serial.print((long)xCoord);
  Serial.print(";"); 
  
  Serial.print("Y");
  Serial.print((long)yCoord); 
  Serial.print(";"); 
  
  Serial.print("Z");
  Serial.print((long)zCoord); 
  Serial.print(";"); 
  
  //give the scales time to become ready again
  delay(50);
}

inline void tickTock()
{
      //tick
    digitalWrite(clockPin, HIGH);
	
	//If the scale output is floating all over the place comment lines 99-102 and uncomment line 106.
	
	//Alternative 1: Use "software" delay (works better on UNO)
	//give the scales a few microseconds to think about it
    for(byte i = 0; i<20; i++)
    {
      __asm__("nop\n\t");
    }
	
    //Alternative 2: Use proper delay
    //give the scales a few microseconds to think about it
    //delayMicroseconds(2);
  
    //tock
    digitalWrite(clockPin, LOW);
  

} 
