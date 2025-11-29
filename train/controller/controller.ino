/*
*  L298P Motor Shield
*/

// The following pin designations are fixed by the shield

//  Motor A
int const ENA = 10;  
int const INA = 12;

//  Throttle
int const SPEED = A2;

// Direction
int const FORW = 3;
int const BACK = 2;

void setup()
{
    //TCCR1B = 0x02;   // Timer 1: PWM 9 & 10 @ 4Khz kHz
    pinMode(ENA, OUTPUT);   // set all the motor control pins to outputs
    pinMode(INA, OUTPUT);
    pinMode(FORW, INPUT_PULLUP);
    pinMode(BACK, INPUT_PULLUP);
    Serial.begin(9600);  
}

void loop()
{
    int speed;
    int forw = digitalRead(FORW) == 0;
    int back = digitalRead(BACK) == 0;
    if( !forw && !back ) {
        speed = 0;
    }
    else {
        speed = map(analogRead(SPEED), 0, 1023, 0, 255);
    }
    int direction = speed < 0 ? 0 : 1;

    digitalWrite(INA, forw);
    analogWrite(ENA, speed);
    Serial.println(speed);
}
