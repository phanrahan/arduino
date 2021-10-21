enum MOTORSTATE {OFF, BRAKE, ON};
enum MOTORDIRECTION {FORWARD, REVERSE};

class Motor {
private:
    // I/O 
    int pinEnable;
    int pinA;
    int pinB;

public:
    // Current state
    MOTORSTATE state;
    MOTORDIRECTION direction;
    int speed;

    Motor(int pEnable, int pA, int pB);

    void setup(void);

    void setSpeed(int speed);

    void Fwd(void);
    void Rev(void);

    void Off(void);
    void On(void);
    void Brake(void);
};

