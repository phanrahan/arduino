/*
*  L298P Motor Shield
*  Code for exercising the L298P Motor Control portion of the shield
*  The low level motor control logic is kept in the function 'Motor'
*/
// The following pin designations are fixed by the shield
int const BUZZER = 4;

//  Motor A
int const ENA = 10;  
int const INA = 12;
//  Motor B
int const ENB = 11;  
int const INB = 13;
//  Throttle
int const THROTTLE = A2;

int const MIN_SPEED = 27;   // Set to minimum PWM value that will make motors turn

void setup()
{
  pinMode(ENA, OUTPUT);   // set all the motor control pins to outputs
  pinMode(ENB, OUTPUT);
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  Serial.begin(9600);     // Set comm speed for serial monitor messages
}

void loop()
{
  int speed = (analogRead(THROTTLE) - 512) / 2;
  int direction = speed < 0 ? 'R' : 'F';
  if( speed < 0 )
      speed = -speed;
  Motor('A', direction, speed);   
}

/*
 * Motor function does all the heavy lifting of controlling the motors
 * mot = motor to control either 'A' or 'B'.  'C' controls both motors.
 * dir = Direction either 'F'orward or 'R'everse
 * speed = Speed.  Takes in 1-100 percent and maps to 0-255 for PWM control.  
 * Mapping ignores speed values that are too low to make the motor turn.
 * In this case, anything below 27, but 0 still means 0 to stop the motors.
 */
void Motor(char mot, char dir, int speed)
{
  if (speed != 0 && speed < MIN_SPEED)
    speed = MIN_SPEED;
  

  switch (mot) {
    case 'A':   // Controlling Motor A
      if (dir == 'F') {
        digitalWrite(INA, HIGH);
      }
      else if (dir == 'R') {
        digitalWrite(INA, LOW);
      }
      analogWrite(ENA, speed);
      break;

    case 'B':   // Controlling Motor B
      if (dir == 'F') {
        digitalWrite(INB, HIGH);
      }
      else if (dir == 'R') {
        digitalWrite(INB, LOW);
      }
      analogWrite(ENB, speed);
      break;
  }

  // Send what we are doing with the motors out to the Serial Monitor.
  
  Serial.print ("Motor: ");
  if (mot=='C')
      Serial.print ("Both");
    else
      Serial.print (mot);
  Serial.print ("t Direction: ");
  Serial.print (dir);
  Serial.print ("t Speed: ");
  Serial.print (speed);
}
