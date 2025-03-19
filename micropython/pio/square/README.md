drive - Pin.Drive_0, Pin.Drive_1, or Pin.Drive_2

Datasheet says there are 4 drive levels: 2ma, 4ma, 8ma, and 12ma.
The default on reset is 4ma.

MicroPython seems to not change the drive strength from its reset value,
so it's 4 mA. 

You can check it by initialising a pin and reading its pad control register.


