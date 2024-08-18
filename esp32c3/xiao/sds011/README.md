- python sds
	- https://github.com/ikalchev/py-sds011
	- git clone py-sds011.py
	- pip install -e .
	- python sds.py - WORKS
      - note fan control

There’s actually three UART devices on this board!

The USB CDC Serial-overUSB, and two hardware UART devices.

These two devices can be allocated to any available GPIO ports you want.

UART0 is mapped to TX=D6, RX=D7 on the xiao. 
UART0 is mapped to TX=21, RX=22 on the supermini. 


To use both, the following code should work.

// Need this for the lower level access to set them up.
#include <HardwareSerial.h>

//Define two Serial devices mapped to the two internal UARTs
HardwareSerial MySerial0(0);
HardwareSerial MySerial1(1);

void setup()
{
    // For the USB, just use Serial as normal:
    Serial.begin(115200);

    // Configure MySerial0 on pins TX=6 and RX=7 (-1, -1 means use the default)
    MySerial0.begin(9600, SERIAL_8N1, -1, -1);
    MySerial0.print("MySerial0");

    // And configure MySerial1 on pins RX=D9, TX=D10
    MySerial1.begin(115200, SERIAL_8N1, 9, 10);
    MySerial1.print("MySerial1");
}

You will need to do some special handling on the MySerial0 port as it gets some diagnostic messages sent to it on boot and if it crashes etc. I’m using it for a LoraWAN modem, so its fairly immune as it won’t react to anything except AT commands. I believe you can stop this with fuse bits, but I haven’t dug that deep yet.
