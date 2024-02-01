/*
*  L298P Motor Shield
*/

// The following pin designations are fixed by the shield

//  Motor A
int const ENA = 10;  
int const INA = 12;

//  Throttle
int const SPEED = A2;

void setup()
{
    pinMode(ENA, OUTPUT);   // set all the motor control pins to outputs
    pinMode(INA, OUTPUT);
    Serial.begin(9600);  
}

void loop()
{

    int speed = analogRead(SPEED);
    speed = (speed - 512) / 2;
    int direction = speed < 0 ? 0 : 1;
    if( speed < 0 )
      speed = -speed;
    if( speed == 256 )
      speed = 255;
    // forward
    digitalWrite(INA, direction);
    analogWrite(ENA, speed);
    Serial.println(speed);
}
