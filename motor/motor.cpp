#include <WProgram.h>
#include "motor.h"

Motor::Motor(int pEnable, int pA, int pB) : 
    pinEnable(pEnable),
    pinA(pA),
    pinB(pB)
{
}

void Motor::setup(void)
{
    pinMode (pinEnable,OUTPUT);
    pinMode (pinA,OUTPUT);
    pinMode (pinB,OUTPUT);

    speed = 255;
    Off();
    Fwd();
}

void Motor::setSpeed(int newspeed)
{
    speed = newspeed;

    if( state == ON )
	analogWrite(pinEnable,speed);
}

void Motor::Off(void)
{
    state = OFF;

    analogWrite(pinEnable,0);
}

void Motor::On(void)
{
    state = ON;

    analogWrite(pinEnable,speed);
}

void Motor::Brake(void)
{
    state = BRAKE;

    analogWrite(pinEnable,255);
    digitalWrite(pinA,LOW);
    digitalWrite(pinB,LOW);
}

void Motor::Fwd(void)
{
    direction = FORWARD;

    digitalWrite(pinA,HIGH);
    digitalWrite(pinB,LOW);
}

void Motor::Rev(void)
{
    direction = REVERSE;

    digitalWrite(pinA,LOW);
    digitalWrite(pinB,HIGH);
}

