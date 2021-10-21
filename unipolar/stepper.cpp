#include <WProgram.h>
#include <Stepper.h>

enum { motorPin1 = 8,
     motorPin2 = 12,
     motorPin3 = 10,
     motorPin4 = 13} ;
int ledPin = 11;

int motorSteps = 100;

Stepper myStepper(motorSteps, motorPin1, motorPin2, 
                             motorPin3, motorPin4);


void setup() {
 myStepper.setSpeed(200);
 Serial.begin(9600);
 pinMode(ledPin, OUTPUT);

}

void loop() {
 Serial.println("Forward");
 myStepper.step(1000);
 delay(1000);
 Serial.println("Back");
 myStepper.step(-1000);
 delay(1000);


}

int main()
{
    init();
    setup();

    for (;;)
        loop();

    return 0;
}

