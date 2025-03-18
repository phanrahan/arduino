from machine import Pin
import time
led_pin = 10  

#led = Pin(led_pin, Pin.OUT)
led = Pin(7, Pin.OUT)  # Use the correct GPIO number instead of D10
while 1:
  led.on()
  time.sleep_ms(500)
  led.off()
  time.sleep_ms(500)
