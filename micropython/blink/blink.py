from machine import Pin
import time

# rp2040
#led_pin = 0  

# esp32c3
led_pin = 10  

led = Pin(led_pin, Pin.OUT)
while 1:
  led.on()
  time.sleep_ms(500)
  led.off()
  time.sleep_ms(500)
