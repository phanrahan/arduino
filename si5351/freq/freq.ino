#include <si5351.h>

const int calvalue = 6800; 
unsigned long int frequency = 5000000 ;

Si5351 si5351;

void setup() {
  frequency = frequency * 100;
  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, calvalue);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA);
  si5351.output_enable(SI5351_CLK0, 0);
  si5351.set_freq(frequency, SI5351_CLK0);
}

void loop() {
}
