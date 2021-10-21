#include "WProgram.h"
#include "motor.h"

Motor motorA(9,6,7);
Motor motorB(10,12,8);

void setup() { 
    motorA.setup();
    motorB.setup();
} 

void loop() { 
    // on //
    motorA.On();
    motorA.Fwd();

    motorB.On();
    motorB.Rev();

    delay(10000);

    // off //
    motorA.Off();
    motorB.Off();

    delay(1000);

    // on //
    motorA.On();
    motorA.Rev();

    motorB.On();
    motorB.Fwd();

    delay(10000);

    // off //
    motorA.Off();
    motorB.Off();

    delay(1000);
} 

