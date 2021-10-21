//
// Author: Hans Summers, 2015
// Website: http://www.hanssummers.com
//
// A very very simple Si5351a demonstration
// using the Si5351a module kit http://www.hanssummers.com/synth
// Please also refer to SiLabs AN619 which describes all the registers to use
//

#include <inttypes.h>
void i2cInit();
uint8_t i2cSendRegister(uint8_t reg, uint8_t data);
uint8_t i2cReadRegister(uint8_t reg, uint8_t *data);

#define I2C_START 0x08
#define I2C_START_RPT 0x10
#define I2C_SLA_W_ACK 0x18
#define I2C_SLA_R_ACK 0x40
#define I2C_DATA_ACK 0x28
#define I2C_WRITE 0b11000000
#define I2C_READ 0b11000001
#ifndef SI5351A_H
#define SI5351A_H

#define SI_CLK0_CONTROL 16 // Register definitions
#define SI_CLK1_CONTROL 17
#define SI_CLK2_CONTROL 18
#define SI_SYNTH_PLL_A 26
#define SI_SYNTH_PLL_B 34
#define SI_SYNTH_MS_0 42
#define SI_SYNTH_MS_1 50
#define SI_SYNTH_MS_2 58
#define SI_PLL_RESET 177

#define SI_R_DIV_1 0b00000000 // R-division ratio definitions
#define SI_R_DIV_2 0b00010000
#define SI_R_DIV_4 0b00100000
#define SI_R_DIV_8 0b00110000
#define SI_R_DIV_16 0b01000000
#define SI_R_DIV_32 0b01010000
#define SI_R_DIV_64 0b01100000
#define SI_R_DIV_128 0b01110000

#define SI_CLK_SRC_PLL_A 0b00000000
#define SI_CLK_SRC_PLL_B 0b00100000

#define XTAL_FREQ 27000000 // Crystal frequency for Hans' board

void si5351aOutputOff(uint8_t clk);
void si5351aSetFrequency(uint32_t frequency);

#endif //SI5351A_H

uint8_t i2cStart()
{
TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

while (!(TWCR & (1<<TWINT))) ;

return (TWSR & 0xF8);
}

void i2cStop()
{
TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);

while ((TWCR & (1<<TWSTO))) ;
}

uint8_t i2cByteSend(uint8_t data)
{
TWDR = data;

TWCR = (1<<TWINT) | (1<<TWEN);

while (!(TWCR & (1<<TWINT))) ;

return (TWSR & 0xF8);
}

uint8_t i2cByteRead()
{
TWCR = (1<<TWINT) | (1<<TWEN);

while (!(TWCR & (1<<TWINT))) ;

return (TWDR);
}

uint8_t i2cSendRegister(uint8_t reg, uint8_t data)
{
uint8_t stts;

stts = i2cStart();
if (stts != I2C_START) return 1;

stts = i2cByteSend(I2C_WRITE);
if (stts != I2C_SLA_W_ACK) return 2;

stts = i2cByteSend(reg);
if (stts != I2C_DATA_ACK) return 3;

stts = i2cByteSend(data);
if (stts != I2C_DATA_ACK) return 4;

i2cStop();

return 0;
}

uint8_t i2cReadRegister(uint8_t reg, uint8_t *data)
{
uint8_t stts;

stts = i2cStart();
if (stts != I2C_START) return 1;

stts = i2cByteSend(I2C_WRITE);
if (stts != I2C_SLA_W_ACK) return 2;

stts = i2cByteSend(reg);
if (stts != I2C_DATA_ACK) return 3;

stts = i2cStart();
if (stts != I2C_START_RPT) return 4;

stts = i2cByteSend(I2C_READ);
if (stts != I2C_SLA_R_ACK) return 5;

*data = i2cByteRead();

i2cStop();

return 0;
}

// Init TWI (I2C)
//
void i2cInit()
{
TWBR = 92;
TWSR = 0;
TWDR = 0xFF;
PRR = 0;
}

void setup()
{
i2cInit();
si5351aSetFrequency(10000000);
}

void loop()
{
// Do a frequency sweep in 1000Hz steps delay 2.5 second
for(unsigned long frequency = 10000000; frequency < 20001000; 
frequency+=1000)
{
si5351aSetFrequency(frequency);
delay(2500);
}

}

//
// Set up specified PLL with mult, num and denom
// mult is 15..90
// num is 0..1,048,575 (0xFFFFF)
// denom is 0..1,048,575 (0xFFFFF)
//
void setupPLL(uint8_t pll, uint8_t mult, uint32_t num, uint32_t denom)
{
uint32_t P1; // PLL config register P1
uint32_t P2; // PLL config register P2
uint32_t P3; // PLL config register P3

P1 = (uint32_t)(128 * ((float)num / (float)denom));
P1 = (uint32_t)(128 * (uint32_t)(mult) + P1 - 512);
P2 = (uint32_t)(128 * ((float)num / (float)denom));
P2 = (uint32_t)(128 * num - denom * P2);
P3 = denom;

i2cSendRegister(pll + 0, (P3 & 0x0000FF00) >> 8);
i2cSendRegister(pll + 1, (P3 & 0x000000FF));
i2cSendRegister(pll + 2, (P1 & 0x00030000) >> 16);
i2cSendRegister(pll + 3, (P1 & 0x0000FF00) >> 8);
i2cSendRegister(pll + 4, (P1 & 0x000000FF));
i2cSendRegister(pll + 5, ((P3 & 0x000F0000) >> 12) | ((P2 & 
0x000F0000) >> 16));
i2cSendRegister(pll + 6, (P2 & 0x0000FF00) >> 8);
i2cSendRegister(pll + 7, (P2 & 0x000000FF));
}

//
// Set up MultiSynth with integer divider and R divider
// R divider is the bit value which is OR'ed onto the appropriate 
// register, it is a #define in si5351a.h
//
void setupMultisynth(uint8_t synth, uint32_t divider, uint8_t rDiv)
{
uint32_t P1; // Synth config register P1
uint32_t P2; // Synth config register P2
uint32_t P3; // Synth config register P3

P1 = 128 * divider - 512;
P2 = 0; // P2 = 0, P3 = 1 forces an integer value for the divider
P3 = 1;

i2cSendRegister(synth + 0, (P3 & 0x0000FF00) >> 8);
i2cSendRegister(synth + 1, (P3 & 0x000000FF));
i2cSendRegister(synth + 2, ((P1 & 0x00030000) >> 16) | rDiv);
i2cSendRegister(synth + 3, (P1 & 0x0000FF00) >> 8);
i2cSendRegister(synth + 4, (P1 & 0x000000FF));
i2cSendRegister(synth + 5, ((P3 & 0x000F0000) >> 12) | ((P2 & 
0x000F0000) >> 16));
i2cSendRegister(synth + 6, (P2 & 0x0000FF00) >> 8);
i2cSendRegister(synth + 7, (P2 & 0x000000FF));
}

//
// Switches off Si5351a output
// Example: si5351aOutputOff(SI_CLK0_CONTROL);
// will switch off output CLK0
//
void si5351aOutputOff(uint8_t clk)
{
i2cSendRegister(clk, 0x80); // Refer to SiLabs AN619 to see 
//bit values - 0x80 turns off the output stage
}

//
// Set CLK0 output ON and to the specified frequency
// Frequency is in the range 1MHz to 150MHz
// Example: si5351aSetFrequency(10000000);
// will set output CLK0 to 10MHz
//
// This example sets up PLL A
// and MultiSynth 0
// and produces the output on CLK0
//
void si5351aSetFrequency(uint32_t frequency)
{
uint32_t pllFreq;
uint32_t xtalFreq = XTAL_FREQ;
uint32_t l;
float f;
uint8_t mult;
uint32_t num;
uint32_t denom;
uint32_t divider;

divider = 900000000 / frequency;// Calculate the division ratio. 900,000,000 is the maximum internal
// PLL frequency: 900MHz
if (divider % 2) divider--; // Ensure an even integer 
//division ratio

pllFreq = divider * frequency; // Calculate the pllFrequency: 
//the divider * desired output frequency

mult = pllFreq / xtalFreq; // Determine the multiplier to 
//get to the required pllFrequency
l = pllFreq % xtalFreq; // It has three parts:
f = l; // mult is an integer that must be in the range 15..90
f *= 1048575; // num and denom are the fractional parts, the numerator and denominator
f /= xtalFreq; // each is 20 bits (range 0..1048575)
num = f; // the actual multiplier is mult + num / denom
denom = 1048575; // For simplicity we set the denominator to the maximum 1048575

// Set up PLL A with the calculated  multiplication ratio
setupPLL(SI_SYNTH_PLL_A, mult, num, denom);
// Set up MultiSynth divider 0, with the calculated divider.
// The final R division stage can divide by a power of two, from 1..128.
// reprented by constants SI_R_DIV1 to SI_R_DIV128 (see si5351a.h header file)
// If you want to output frequencies below 1MHz, you have to use the
// final R division stage
setupMultisynth(SI_SYNTH_MS_0, divider, SI_R_DIV_1);
// Reset the PLL. This causes a glitch in the output. For small changes to
// the parameters, you don't need to reset the PLL, and there is no glitch
i2cSendRegister(SI_PLL_RESET, 0xA0);
// Finally switch on the CLK0 output (0x4F)
// and set the MultiSynth0 input to be PLL A 
i2cSendRegister(SI_CLK0_CONTROL, 0x4F | SI_CLK_SRC_PLL_A);
}


