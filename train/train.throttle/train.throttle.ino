int THROTTLE_PIN = 0;
int INPUT1_PIN = 2;
int INPUT2_PIN = 3;
int ENABLE_PIN = 7;
int LED_PIN = 13;
 
int THROTTLE_PROPORTION = 4;
int THROTTLE_THRESHOLD = 5;
int SPEED_MAX = 253;
float ACCELERATION_PROPORTION = 0.1;
int MIN_SPEED = 30;

int dir = 0;
float spd;
int throttle;
float acceleration;
int state = 0;
 
void setup() {
  Serial.begin(9600);
        digitalWrite(ENABLE_PIN, HIGH);
        pinMode(51,INPUT);
        pinMode(52,INPUT);
        pinMode(50,OUTPUT);
        pinMode(49,OUTPUT);
	pinMode(48,OUTPUT);
	pinMode(47,OUTPUT);
}

int oldDir = 1;

void loop() {
        throttle = getThrottle();
        if (throttle <= THROTTLE_THRESHOLD) {
                throttle = 0;
                digitalWrite(LED_PIN,HIGH);
        } else digitalWrite(LED_PIN,LOW);
        accelerate();
        updateSpeed();
        
        updateButtons();
        
        if (dir == 0 && oldDir != dir) {
          digitalWrite(49,HIGH);
                    digitalWrite(50,LOW);
        } else if (dir == 1 && oldDir != dir) {
          digitalWrite(49,LOW);
                    digitalWrite(50,HIGH);          
        }
        oldDir = dir;
}

void updateButtons() {
	if (digitalRead(52) == LOW) dir =0; 
	if (digitalRead(51) == LOW) dir =1;
}
 
int getThrottle() {
        int t = SPEED_MAX - analogRead(THROTTLE_PIN) / THROTTLE_PROPORTION;
        if (t > 5) t += MIN_SPEED;
        if (t > SPEED_MAX) {
          Serial.print("Warning, the throttle is: ");
          Serial.println(t);
        }
        return t;
}
 
void accelerate() {
        if (throttle != spd) {
                if (spd < throttle) {
                        //acceleration = (throttle - spd) * ACCELERATION_PROPORTION;
                        if (spd >5 && spd < MIN_SPEED) spd += 10;
                        spd += 0.5; //acceleration;
                        if (spd > throttle) spd = throttle;
                        if (state != 1) Serial.println("Accelerating...");
                        state = 1;
                        if (dir == 0) digitalWrite(48,HIGH);
                        if (dir == 1) digitalWrite(47,HIGH);
                } else if (spd > throttle) {
                        //acceleration = (spd - throttle) * ACCELERATION_PROPORTION;
                        if (spd >5 && spd < MIN_SPEED) spd -= 10;
                        spd -= 0.5; //acceleration;
                        if (spd < throttle) spd = throttle;
                        if (spd < 0) spd = 0;
                        if (state != 2) Serial.println("Decelerating...");
                        state = 2;
                        if (dir == 0) digitalWrite(48,HIGH);
                        if (dir == 1) digitalWrite(47,HIGH);
                }
                if (throttle == 0 && spd != 0) {
                        spd *= (1-ACCELERATION_PROPORTION);
                        if (state != 3) Serial.println("Crusing...");
                        state = 3;  
                }
        }
        Serial.print(spd);
        Serial.print(" :: ");
        Serial.println(throttle);
        digitalWrite(47,LOW);
        digitalWrite(48,LOW);
}
 
void updateSpeed() {
        if (dir == 1) {
                digitalWrite(INPUT1_PIN,HIGH);
                digitalWrite(INPUT2_PIN,LOW);
        } else if (dir == 0) {
                digitalWrite(INPUT1_PIN,LOW);
                digitalWrite(INPUT2_PIN,HIGH);
        } 
        
        analogWrite(ENABLE_PIN, spd);
       
        if (spd == 0) {
                digitalWrite(INPUT1_PIN,LOW);
                digitalWrite(INPUT2_PIN,LOW);
                if (state != 0) Serial.println("Stopped!");
                state = 0;
        }
}
