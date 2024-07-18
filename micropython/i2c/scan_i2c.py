from machine import Pin, I2C

i2c = I2C(sda=Pin(6), scl=Pin(7), freq=100_000)

devices = i2c.scan()
print("devices: ")
for device in devices:
    print("0x{0:02x}".format(device))
