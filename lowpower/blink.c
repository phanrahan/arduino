/*
** http://brownsofa.org/blog/2011/01/09/the-compleat-attiny13-led-flasher-part-3-low-power-mode/
*/

#include <avr/interrupt.h>
#include <avr/sleep.h>

ISR(WDT_vect) {
   // Toggle Port B pin 4 output state
   PORTB ^= 1<<PB4;
}

int main(void) {
   // Set up Port B pin 4 mode to output
    DDRB = 1<<DDB4;

   // temporarily prescale timer to 4s so we can measure current
   WDTCR |= (1<<WDP3); // (1<<WDP2) | (1<<WDP0);

   // Enable watchdog timer interrupts
   WDTCR |= (1<<WDTIE);

   sei(); // Enable global interrupts 

   // Use the Power Down sleep mode
   set_sleep_mode(SLEEP_MODE_PWR_DOWN);

   for (;;) {
      sleep_mode();   // go to sleep and wait for interrupt...
   }
}

