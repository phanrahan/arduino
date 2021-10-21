// (*) All in the spirit of open-source and open-hardware
// Janost 2017 Sweden

// The Micro-TS Key fob Touch Synthesizer
// https://janostman.wordpress.com/the-microts-diy-synth/

// Copyright 2017 DSP Synthesizers Sweden. 
// 
// Author: Jan Ostman 
// 
// This program is free software: you can redistribute it and/or modify 
// it under the terms of the GNU General Public License as published by 
// the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version. 
// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
// GNU General Public License for more details. 

//Set Fuses to E2 DF FF for Internal 16MHz Clock

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// Standard Arduino Pins
#define digitalPinToPortReg(P) \
(((P) >= 0 && (P) <= 7) ? &PORTD : (((P) >= 8 && (P) <= 13) ? &PORTB : &PORTC))
#define digitalPinToDDRReg(P) \
(((P) >= 0 && (P) <= 7) ? &DDRD : (((P) >= 8 && (P) <= 13) ? &DDRB : &DDRC))
#define digitalPinToPINReg(P) \
(((P) >= 0 && (P) <= 7) ? &PIND : (((P) >= 8 && (P) <= 13) ? &PINB : &PINC))
#define digitalPinToBit(P) \
(((P) >= 0 && (P) <= 7) ? (P) : (((P) >= 8 && (P) <= 13) ? (P) - 8 : (P) - 14))

#define digitalReadFast(P) bitRead(*digitalPinToPINReg(P), digitalPinToBit(P))
                  
#define digitalWriteFast(P, V) bitWrite(*digitalPinToPortReg(P), digitalPinToBit(P), (V))

const unsigned char PS_2 = (1 << ADPS0);;
const unsigned char PS_4 = (1 << ADPS1);
const unsigned char PS_8 = (1 << ADPS1) | (1 << ADPS0);
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);



//1,0594630944
uint16_t NOTES[12]={8192>>1,8679>>1,9195>>1,9742>>1,10321>>1,10935>>1,11585>>1,12274>>1,13004>>1,13777>>1,14596>>1,15464>>1};

const uint8_t ATTrates[16]={
1,2,3,4,5,8,12,20,32,37,43,51,64,85,128,255};

const uint8_t RELrates[16]={
1,2,3,4,5,8,12,20,32,37,43,51,64,85,128,255};


const uint8_t sinetable[256] PROGMEM = {
  0,0,0,0,1,1,1,2,2,3,4,5,5,6,7,9,10,11,12,14,15,16,18,20,21,23,25,27,29,31,
  33,35,37,39,42,44,46,49,51,54,56,59,62,64,67,70,73,76,78,81,84,87,90,93,96,99,102,105,108,111,115,118,121,124,
  127,130,133,136,139,143,146,149,152,155,158,161,164,167,170,173,176,178,181,184,187,190,192,195,198,200,203,205,208,210,212,215,217,219,221,223,225,227,229,231,233,234,236,238,239,240,
  242,243,244,245,247,248,249,249,250,251,252,252,253,253,253,254,254,254,254,254,254,254,253,253,253,252,252,251,250,249,249,248,247,245,244,243,242,240,239,238,236,234,233,231,229,227,225,223,
  221,219,217,215,212,210,208,205,203,200,198,195,192,190,187,184,181,178,176,173,170,167,164,161,158,155,152,149,146,143,139,136,133,130,127,124,121,118,115,111,108,105,102,99,96,93,90,87,84,81,78,
  76,73,70,67,64,62,59,56,54,51,49,46,44,42,39,37,35,33,31,29,27,25,23,21,20,18,16,15,14,12,11,10,9,7,6,5,5,4,3,2,2,1,1,1,0,0,0
};


uint16_t envtick=549;

uint8_t capsensebase = 3; //Set capacitive touch sensitivity
uint8_t currentkey = 0;
uint8_t key;
uint8_t oldkeydown;
uint16_t COARSEPITCH;
volatile uint8_t DOUBLE=0;

int16_t volume=0;
uint8_t ENVsmoothing;
uint8_t envcnt=10;

uint8_t GATEIO=0; //0=Gate is output, 1=Gate is input



//-------- Synth parameters --------------
volatile uint8_t VCA=255;         //VCA level 0-255
volatile uint8_t ATTACK=1;        // ENV Attack rate 0-255
volatile uint8_t RELEASE=1;       // ENV Release rate 0-255
volatile uint8_t ENVELOPE=0;      // ENV Shape
volatile uint8_t TRIG=0;          //MIDItrig 1=note ON
volatile uint16_t PDmod;          //Resonant Peak index
volatile uint8_t ENVamt;          //Resonant Peak envelope modulation amount
volatile uint8_t PHASEamt;        //Resonant Peak bias
//-----------------------------------------



uint8_t otone1;
uint8_t otone2;
uint16_t phacc;
uint16_t pdacc;
uint8_t otone12;
uint8_t otone22;
uint16_t phacc2;
uint16_t pdacc2;

uint16_t FREQ=0;         //DCO pitch

int16_t DCO;
int16_t ENV;

uint8_t k=0;
uint8_t z;
int8_t MUX=0;


ISR(TIMER0_COMPA_vect) {
  
    //-------------------- DCO block ------------------------------------------
        
    phacc+=FREQ;
    if (phacc & 0x8000) {
      phacc &= 0x7FFF;
      otone1+=2; 
      pdacc+=PDmod;
    }
    if (!otone1) pdacc=0;
    otone2=(pdacc>>3)&255;
    uint8_t env=(255-otone1);
    DCO=((pgm_read_byte(&sinetable[otone2])+pgm_read_byte(&sinetable[(otone2+(127-env))&255])));

    phacc2+=FREQ+DOUBLE;
    if (phacc2 & 0x8000) {
      phacc2 &= 0x7FFF;
      otone12+=2; 
      pdacc2+=PDmod;
    }
    if (!otone12) pdacc2=0;
    otone22=(pdacc2>>3)&255;
    env=(255-otone12);
    DCO+=((pgm_read_byte(&sinetable[otone22])+pgm_read_byte(&sinetable[(otone22+(127-env))&255])));

    //---------------------------------------------------------------------------
    
    

    //------------------ VCA block ------------------------------------
    if ((ATTACK==255)&&(TRIG==1)) VCA=255;
    if (!(envcnt--)) {
      envcnt=20;
     if (VCA<volume) VCA++;
     if (VCA>volume) VCA--;
    }
    #define M(MX, MX1, MX2) \
      asm volatile ( \
        "clr r26 \n\t"\
        "mulsu %B1, %A2 \n\t"\
        "movw %A0, r0 \n\t"\
        "mul %A1, %A2 \n\t"\
        "add %A0, r1 \n\t"\
        "adc %B0, r26 \n\t"\
        "clr r1 \n\t"\
        : \
        "=&r" (MX) \
        : \
        "a" (MX1), \
        "a" (MX2) \
        :\
        "r26"\
      )
      
      DCO>>=1;
      M(ENV, (int16_t)DCO, VCA);
      
      OCR2A = ENV>>1;    
 
    
   //-----------------------------------------------------------------

 if (!(envtick--)) {
  envtick=582; 
   //--------------------- ENV block ---------------------------------
    
    if ((TRIG==1)&&(volume<255)) {
      volume+=ATTACK;
      if (volume>255) volume=255;
    }
    if ((TRIG==0)&&(volume>0)) {
      volume-=RELEASE;
      if (volume<0) volume=0;
    }
    
    //----------------------------------------------------------------- 

    //--------- Resonant Peak ENV modulation ----------------------------------------

    PDmod=((volume*ENVamt)>>8)+PHASEamt;
    if (PDmod>255) PDmod=255;
    
    //-----------------------------------------------------------------



  }
}









void setup() {

    OSCCAL=255;

    //Test if Gate should be input or output
    pinMode(13, INPUT_PULLUP); //Gate output
    if (!digitalRead(13)) { //If gate is low on startup
      pinMode(13, INPUT); //Gate is input
      pinMode(9, INPUT);  //CV is input
      GATEIO=1; //Use Gate as input
    }
    else {
      pinMode(13, OUTPUT); //Gate is output
      pinMode(9, OUTPUT);  //CV is output
      GATEIO=0; //Use Gate as output
    }

    pinMode(11, OUTPUT); //Audio output
    

    // Set up Timer 1 to do PWM for the CV 1v/oct output.

    cli();


  TCCR1A =
      (1 << COM1A1)  |
      // Fast PWM mode.
      (1 << WGM11);
  TCCR1B =
      // Fast PWM mode.
      (1 << WGM12) | (1 << WGM13) |
      // No clock prescaling (fastest possible
      // freq).
      (1 << CS11);
  OCR1A = 0;
  TIMSK1=0;
  OCR1AH = 2; //CV PWM
  OCR1AL = 128;
  
  ICR1H = 5; //CV Frequency
  ICR1L = 0;

    //set timer0 interrupt at 35481Hz
    TCCR0A = 0;// set entire TCCR0A register to 0
    TCCR0B = 0;// same for TCCR0B
    TCNT0  = 0;//initialize counter value to 0
    // set compare match register for 35481Hz increments
    OCR0A = 56;// = 35481Hz
    // turn on CTC mode
    TCCR0A |= (1 << WGM01);
    // Set CS01 and CS00 bits for prescaler 8
    TCCR0B |= (0 << CS02) | (1 << CS01) | (0 << CS00);  //8 prescaler 
    // enable timer compare interrupt
    TIMSK0 |= (1 << OCIE0A); 
   
    sei();

    // Set up Timer 2 to do pulse width on Pin 11

    // Use internal clock
    ASSR &= ~(_BV(EXCLK) | _BV(AS2));

    // Set fast PWM mode
    TCCR2A |= _BV(WGM21) | _BV(WGM20);
    TCCR2B &= ~_BV(WGM22);

    
    // Do non-inverting PWM on pin OC2A.
    // On the Arduino this is pin 11.
    TCCR2A = (TCCR2A | _BV(COM2A1)) & ~_BV(COM2A0);
    TCCR2A &= ~(_BV(COM2B1) | _BV(COM2B0));
    // No prescaler (p.158)
    TCCR2B = (TCCR2B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);

    // Set initial pulse width to the first sample.
    OCR2A = 128;

    // set up the ADC
    uint16_t dummy=analogRead(0);
    ADCSRA &= ~PS_128;  // remove bits set by Arduino library
  
    // you can choose a prescaler from above.
    // PS_16, PS_32, PS_64 or PS_128
    ADCSRA |= PS_128;    // set our own prescaler to 128

    ADMUX = 64;
    sbi(ADCSRA, ADSC);       

}

//---------------- Get the base frequency for the MIDI note ---------------
uint16_t MIDI2FREQ(uint8_t note) {
  uint8_t key=note%12;
  if (note<36) return (NOTES[key]>>(1+(35-note)/12));
  if (note>47) return (NOTES[key]<<((note-36)/12));
  return NOTES[key];
}
//-------------------------------------------------------------------------



//---------- Capacitive Touch sensing -----------------------------

uint8_t capsensePORTD(uint8_t mask) {
   PORTD = 0; //Ground the PCB surface
   DDRD  = 0xFF;
   asm("nop"); 
   cli();
   DDRD &= ~(mask); //Turn selected pin to input
   PORTD |= mask;   //With pullup
   uint8_t cycles = 0;
   if      (PIND & mask) { cycles =  0;}
   else if (PIND & mask) { cycles =  1;}
   else if (PIND & mask) { cycles =  2;}
   else if (PIND & mask) { cycles =  3;}
   else if (PIND & mask) { cycles =  4;}
   else if (PIND & mask) { cycles =  5;}
   else if (PIND & mask) { cycles =  6;}
   else if (PIND & mask) { cycles =  7;}
   else if (PIND & mask) { cycles =  8;}
   else if (PIND & mask) { cycles =  9;}
   else if (PIND & mask) { cycles = 10;}
   else if (PIND & mask) { cycles = 11;}
   else if (PIND & mask) { cycles = 12;}
   else if (PIND & mask) { cycles = 13;}
   else if (PIND & mask) { cycles = 14;}
   else if (PIND & mask) { cycles = 15;}
   else if (PIND & mask) { cycles = 16;}
   sei();
   PORTD = 0; //Ground the PCB surface
   return cycles; //Return measured cycles
}

uint8_t capsensePORTB(uint8_t mask) {
   if ((mask&2)||(mask&8)||(mask&32)) return 0; //Dont measure our Audio and CV pins
   PORTB &= 0x20; //Ground the PCB surface
   DDRB  |= 0xDD;
   asm("nop"); 
   cli();
   DDRB &= ~(mask); //Turn selected pin to input
   PORTB |= mask;   //With pullup
   uint8_t cycles = 0;
   if      (PINB & mask) { cycles =  0;}
   else if (PINB & mask) { cycles =  1;}
   else if (PINB & mask) { cycles =  2;}
   else if (PINB & mask) { cycles =  3;}
   else if (PINB & mask) { cycles =  4;}
   else if (PINB & mask) { cycles =  5;}
   else if (PINB & mask) { cycles =  6;}
   else if (PINB & mask) { cycles =  7;}
   else if (PINB & mask) { cycles =  8;}
   else if (PINB & mask) { cycles =  9;}
   else if (PINB & mask) { cycles = 10;}
   else if (PINB & mask) { cycles = 11;}
   else if (PINB & mask) { cycles = 12;}
   else if (PINB & mask) { cycles = 13;}
   else if (PINB & mask) { cycles = 14;}
   else if (PINB & mask) { cycles = 15;}
   else if (PINB & mask) { cycles = 16;}
   sei();
   PORTB &= 0x20; //Ground the PCB surface
   return cycles; //Return measured cycles
}

//-----------------------------------------------------------------

void loop() {  
  //------------------ Key scanner -----------------------------
  uint8_t keydown=0;

  for (currentkey=0;currentkey<13;currentkey++) {
    if ((currentkey==0)&&(capsensePORTD(1)>capsensebase)) {
      key=currentkey;
      keydown=1;
    }
    if ((currentkey==1)&&(capsensePORTD(2)>capsensebase)) {
      key=currentkey;
      keydown=1;
    }
    if ((currentkey==2)&&(capsensePORTD(4)>capsensebase)) {
      key=currentkey;
      keydown=1;
    }
    if ((currentkey==3)&&(capsensePORTD(8)>capsensebase)) {
      key=currentkey;
      keydown=1;
    }
    if ((currentkey==4)&&(capsensePORTD(16)>capsensebase)) {
      key=currentkey;
      keydown=1;
    }
    if ((currentkey==5)&&(capsensePORTB(64)>capsensebase)) {
      key=currentkey;
      keydown=1;
    }
    if ((currentkey==6)&&(capsensePORTB(128)>capsensebase)) {
      key=currentkey;
      keydown=1;
    }
    if ((currentkey==7)&&(capsensePORTD(32)>capsensebase)) {
      key=currentkey;
      keydown=1;
    }
    if ((currentkey==8)&&(capsensePORTB(1)>capsensebase)) {
      key=currentkey;
      keydown=1;
    }
    if ((currentkey==9)&&(capsensePORTD(128)>capsensebase)) {
      key=currentkey;
      keydown=1;
    }
    if ((currentkey==10)&&(capsensePORTD(64)>capsensebase)) {
      key=currentkey;
      keydown=1;
    }  
    if ((currentkey==11)&&(capsensePORTB(4)>capsensebase)) {
      key=12;
      keydown=1;
    }
    if ((currentkey==12)&&(capsensePORTB(16)>capsensebase)) {
      key=11;
      keydown=1;
    }
  }
  if (!GATEIO) FREQ=MIDI2FREQ(key+12+(COARSEPITCH/21.3125));
  if (GATEIO) FREQ=MIDI2FREQ(((digitalRead(9)&1)*12)+key+12+(COARSEPITCH/21.3125));
  uint16_t CVout=COARSEPITCH+(key*21.33);
  OCR1AH = (CVout>>8);
  OCR1AL = CVout&255;
  if (!GATEIO) digitalWrite(13,keydown);
  if (GATEIO) TRIG=digitalRead(13)&1;
  if (!GATEIO) TRIG=keydown;
  //--------------- ADC block -------------------------------------
      if (!(ADCSRA & 64)) {
        if (MUX==5) COARSEPITCH=(ADCL+(ADCH<<8));
        if (MUX==4) DOUBLE=(ADCL+(ADCH<<8))>>2;
        if (MUX==3) PHASEamt=(((ADCL+(ADCH<<8)))>>3);
        if (MUX==2) ENVamt=((ADCL+(ADCH<<8))>>3);
        if (MUX==1) ATTACK=ATTrates[15-((ADCL+(ADCH<<8))>>6)];
        if (MUX==0) RELEASE=RELrates[15-((ADCL+(ADCH<<8))>>6)];
        MUX++;
        if (MUX>5) MUX=0;
        ADMUX = 64 | MUX; //Select MUX
        sbi(ADCSRA, ADSC); //start next conversation
      }
   //--------------------------------------------------------------------
}

