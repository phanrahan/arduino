/*
*  L298P Motor Shield
*/

// The following pin designations are fixed by the shield

//  Motor A
int const ENA = 10;  
int const INA = 12;

//  Motor B
int const ENB = 11;  
int const INB = 13;

void setup()
{
    pinMode(ENA, OUTPUT);   // set all the motor control pins to outputs
    pinMode(ENB, OUTPUT);
    pinMode(INA, OUTPUT);
    pinMode(INB, OUTPUT);
    digitalWrite(ENA, HIGH);
    digitalWrite(ENB, HIGH);
}

void loop()
{
    // forward
    digitalWrite(INA, HIGH);
    digitalWrite(INB, HIGH);
    delay(5000);
    // backward
    digitalWrite(INA, LOW);
    digitalWrite(INB, LOW);
    delay(5000);
}
