import time
from machine import Pin, SoftI2C
import ssd1306

i2c = SoftI2C(sda=Pin(6), scl=Pin(7), freq=100_000)
display = ssd1306.SSD1306_I2C(64, 32, i2c)

while True:
    display.fill(0)
    display.text("hello", 10, 20, 1)
    display.show()
    time.sleep(1.0)
