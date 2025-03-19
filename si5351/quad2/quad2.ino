// Test program to evaluate Si5351 quadrature output using Etherkit 5351 Library
// Example based on ZL2CTM  Jan. 2019 blog post
// Si5351 Quadrature Clock Output down to 3MHz
// Credit to Brian Harper M1CEM and Miguel Bartié PY2OHH


//Step 1. Edit si5351.h file. Change the SI5351_PLL_VCO_MIN to 380000000, i.e.,
//#define SI5351_PLL_VCO_MIN              380000000

// Example code follows to set CLK0 and CLK2 in quadrature at frequency freq


#include "si5351.h"
#include "Wire.h"

Si5351 si5351;


volatile long freq = 3500000;
volatile int Even_Divisor = 0;
volatile int oldEven_Divisor = 0;
unsigned long pfreq;

void EvenDivisor()
{
    if (freq < 6850000)
  {
    Even_Divisor = 126;
  }
  if ((freq >= 6850000) && (freq < 9500000))
  {
    Even_Divisor = 88;
  }
  if ((freq >= 9500000) && (freq < 13600000))
  {
    Even_Divisor = 64;
  }
  if ((freq >= 13600000) && (freq < 17500000))
  {
    Even_Divisor = 44;
  }
  if ((freq >= 17500000) && (freq < 25000000))
  {
    Even_Divisor = 34;
  }
  if ((freq >= 25000000) && (freq < 36000000))
  {
    Even_Divisor = 24;
  }
  if ((freq >= 36000000) && (freq < 45000000)) {
    Even_Divisor = 18;
  }
  if ((freq >= 45000000) && (freq < 60000000)) {
    Even_Divisor = 14;
  }
  if ((freq >= 60000000) && (freq < 80000000)) {
    Even_Divisor = 10;
  }
  if ((freq >= 80000000) && (freq < 100000000)) {
    Even_Divisor = 8;
  }
  if ((freq >= 100000000) && (freq < 146600000)) {
    Even_Divisor = 6;
  }
  if ((freq >= 150000000) && (freq < 220000000)) {
    Even_Divisor = 4;
  }
}


void SendFrequency()
{
  EvenDivisor();
  si5351.set_freq_manual(freq * SI5351_FREQ_MULT, Even_Divisor * freq * SI5351_FREQ_MULT, SI5351_CLK0);
  si5351.set_freq_manual(freq * SI5351_FREQ_MULT, Even_Divisor * freq * SI5351_FREQ_MULT, SI5351_CLK2);
  si5351.set_phase(SI5351_CLK0, 0);
  si5351.set_phase(SI5351_CLK2, Even_Divisor);
 
if(Even_Divisor != oldEven_Divisor)
  {
    si5351.pll_reset(SI5351_PLLA);
    oldEven_Divisor = Even_Divisor;
  }
  Serial.print("Even Divisor  ");
  Serial.println(Even_Divisor);
  Serial.print("New Freq  ");
  Serial.println(freq);
  Serial.print("Sending  ");
  pfreq =(freq * SI5351_FREQ_MULT);
  Serial.println(pfreq);
}


void setup()
{
  // Start serial and initialize the Si5351
  Serial.begin(9600);
  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
 
SendFrequency();


freq = freq+100;
delay(2000);
}
