#include <Arduino.h>
#include <Encoder.h>
#include <U8g2lib.h>
#include <si5351.h>

Encoder knob(2, 3);
int lastpos;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);


Si5351 si5351;

int calvalue = 6800;// the calibration value for the TCXO

long freq = 5000000;
long dfreq = 100000/4;


void display_frequency(long freq)
{
  char cstr[20];
  sprintf(cstr, "%ld", freq);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_logisoso16_tr);
  u8g2.drawStr(0,20,cstr);
  u8g2.sendBuffer();
  delay(100);
}

void setFrequency() {
  si5351.set_freq(freq, SI5351_CLK0);
  display_frequency(freq);
}

void setup() {
  lastpos = knob.read();

  u8g2.begin();

  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA);
  si5351.output_enable(SI5351_CLK0, 0);

  setFrequency();

}

void loop() {
  int pos = knob.read();
  if (pos != lastpos) {
    freq += (pos - lastpos) * dfreq;
    lastpos = pos;
    setFrequency();
  }
}
