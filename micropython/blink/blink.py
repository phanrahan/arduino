from machine import Pin
import time
led_pin = 0  

led = Pin(led_pin, Pin.OUT)
while 1:
  led.on()
  time.sleep_ms(500)
  led.off()
  time.sleep_ms(500)
