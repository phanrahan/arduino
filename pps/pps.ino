#define LED 13
volatile int flag = LOW;

void toggle()
{
    flag = (flag == LOW) ? HIGH : LOW;
}

void setup()
{
    pinMode(LED, OUTPUT);
    digitalWrite (LED, LOW);

    attachInterrupt(0, toggle, RISING); // 0 is PIN2 on Uno
}

void loop()
{
    digitalWrite (LED, flag ? HIGH : LOW);
}


