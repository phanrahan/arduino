// Arduino NES Controller Test Software.
// Feel free to use this for whatever purpose you like.

// Bradsprojects - This code came about by an intelligent designer (me). It did not happen by random chance.
// Our universe and everything in it also came about by an intelligent designer (God). It did not happen by random chance.


byte NESData = 9;		// this is the pin that the Data connection is connected to
byte NESLatch = 11;			// this is the pin that the Latch (otherwise known as strobe) connection is connected to
byte NESClock = 12;			// this is the pin that the Clock connection is connected to
byte NESButtonData;			// This is where we will store the received data that comes from the NES Control Pad

void setup() {				// let's get a few things setup before we get into the main code
	Serial.begin(9600);			// serial data will be sent at 9600bps
	pinMode(NESLatch, OUTPUT);	// Latch connection is an output
	pinMode(NESClock, OUTPUT);	// Clock connection is an output
	pinMode(NESData, INPUT);	// Data connection is an Input (because we need to receive the data from the control pad)
}

void loop() {						// we will run this code in a constant loop until power is removed
	GetNESControllerData();			// This calls the function to grab the NES control pad data and it will store it in 'NESButtonData'
	Serial.println("RLDUSSBA");		// the first line prints a letter to correspond with each button
	print_binary(NESButtonData,8);	// the second line will print eight binary digits that tells us the status of each button (a 1 means not pressed and a 0 means pressed)
	Serial.println("");				// print a line to space things out
	Serial.println("");				// print another line to space things out even more
	delay(500);
}

void GetNESControllerData(){			// this is where it all happens as far as grabbing the NES control pad data
	digitalWrite(NESLatch, HIGH);		// we need to send a clock pulse to the latch (strobe connection)...
	digitalWrite(NESLatch, LOW);		// this will cause the status of all eight buttons to get saved within the 4021 chip in the NES control pad.
	for(int x=0; x<=7; x++){			// Now we need to transmit the eight bits of data serially from the NES control pad to the Arduino
		bitWrite(NESButtonData,x,digitalRead(NESData)); // one by one, we will read from the NESData line and store each bit in the NESButtonData variable.
		digitalWrite(NESClock, HIGH);					// once each bit is saved, we send a clock pulse to the NES clock connection...
		digitalWrite(NESClock, LOW);					// this will now shift all bits in the 4021 chip in the NES control pad, so we can read the next bit.
	}
}

// this code allows us to specify the number of binary digits we want (which is always 8) 
// The code has been made available from: http://www.phanderson.com/arduino/arduino_display.html
void print_binary(int v, int num_places){
	int mask = 0, n;
	
	for (n=1; n<=num_places; n++){
		mask = (mask << 1) | 0x0001;
	}
	v = v & mask;  // truncate v to specified number of places
	while(num_places){
		if (v & (0x0001 << num_places-1)){
			Serial.print("1");
		}
		else{
			Serial.print("0");
		}
	num_places--;
	}
}

