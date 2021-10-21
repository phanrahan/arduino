// The following is a simple stepper motor control procedures,

# define EN 8 // stepper motor enable , active low
# define X_DIR 5 // X -axis stepper motor direction control
# define Y_DIR 6 // y -axis stepper motor direction control
# define Z_DIR 7 // z axis stepper motor direction control
# define X_STP 2 // x -axis stepper control
# define Y_STP 3 // y -axis stepper control
# define Z_STP 4 // z -axis stepper control

/*
// Function : step . function: to control the direction of the stepper motor , the number of steps .
// Parameters : dir direction control , dirPin corresponding stepper motor DIR pin, 
// stepperPin corresponding stepper motor " step " pin , Step number of step of no return value.
*/
void step (boolean dir, byte dirPin, byte stepperPin, int steps)
{
    digitalWrite (dirPin, dir);
    delay (50);
    for (int i = 0; i <steps; i++) {
        digitalWrite (stepperPin, HIGH);
        delayMicroseconds (100);
        digitalWrite (stepperPin, LOW);
        delayMicroseconds (100);
    }
}

void setup () { // The stepper motor used in the IO pin is set to output
    pinMode (X_DIR, OUTPUT); pinMode (X_STP, OUTPUT);
    //pinMode (Y_DIR, OUTPUT); pinMode (Y_STP, OUTPUT);
    //pinMode (Z_DIR, OUTPUT); pinMode (Z_STP, OUTPUT);
    pinMode (EN, OUTPUT);
    digitalWrite (EN, LOW);
}

void loop () {
    step (false, X_DIR, X_STP, 3200); // X axis motor reverse 1 ring, the 200 step is a circle.
    //step (false, Y_DIR, Y_STP, 200); // y axis motor reverse 1 ring, the 200 step is a circle.
    //step (false, Z_DIR, Z_STP, 200); // z axis motor reverse 1 ring, the 200 step is a circle.
    delay (1000);
    step (true, X_DIR, X_STP, 3200); // X axis motor forward 1 laps, the 200 step is a circle.
    //step (true, Y_DIR, Y_STP, 200); // y axis motor forward 1 laps, the 200 step is a circle.
    //step (true, Z_DIR, Z_STP, 200); // z axis motor forward 1 laps, the 200 step is a circle.
    delay (1000);
} 
