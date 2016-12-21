#include <DDS.h>

// AD9850 Pins
const int W_CLK = 2;
const int FQ_UD = 3; 
const int DATA  = 4;
const int RESET = 5;

DDS dds(W_CLK, FQ_UD, DATA, RESET);

void setup() {
  Serial.begin(9600);

  dds.init();  
  dds.trim(124997000);   // (Optional) trim if your xtal is not at 125MHz...
}

void loop() {
  const int sfreq = 7300000;
  const int efreq = 7400000;
  const int dfreq =    1000;

  for( int freq = sfreq; freq < efreq; freq += dfreq ) {
      dds.setFrequency(freq);  
      int response = analogRead(A0);
      Serial.print('freq');
      Serial.print(freq);
      Serial.print('response');
      Serial.print(response);
      Serial.println();
      delay(10);
  }
}
