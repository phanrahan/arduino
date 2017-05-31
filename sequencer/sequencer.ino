#define STEPS 4
const int note[] = { 2, 3, 4, 5, 6, 7, 8, 9 };
const int run = 10;
const int reset = 11;
const int noteon = 12;
const int gate = 13;
const int tempo = A0;
const int duration = A1;

int step = 0;

void setup() 
{
   for( int i = 0; i < STEPS; i++ ) {
      pinMode(note[i], OUTPUT);
   }
   pinMode(tempo, INPUT);
   pinMode(duration, INPUT);
   pinMode(run, INPUT);
   pinMode(reset, INPUT);
   pinMode(noteon, INPUT);
   pinMode(gate, OUTPUT);

   Serial.begin(9600);
}

void loop() 
{
    if( !digitalRead(run) ) return;
    uint32_t bpm = analogRead(tempo)/2;
    uint32_t length = (60000ULL)/(long long)bpm;
    uint16_t d = (length * analogRead(duration))/1023;
    digitalWrite(note[step], 1);
    int on = digitalRead(noteon);
    Serial.println(length);
    Serial.println(d);
    digitalWrite(gate,on);
    delay(d);
    digitalWrite(gate,0);
    delay(length-d);
    digitalWrite(note[step], 0);
    step = (step + 1) % STEPS;
}
