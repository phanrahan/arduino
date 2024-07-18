from machine import Pin
import time
led_pin = 10  

led = Pin(led_pin, Pin.OUT)
for i in range(10):
  led.on()
  time.sleep_ms(500)
  led.off()
  time.sleep_ms(500)
