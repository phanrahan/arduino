volatile uint32_t clk_cnt = 0UL ;
volatile uint32_t pps_cnt = 0UL ;
volatile bool dirty = false ;
 
void
pps_interrupt()
{
    pps_cnt ++ ;
    dirty = true ;
}
 
void
clk_interrupt()
{
    clk_cnt ++ ;
}
 
void
setup()
{
    Serial.begin(9600) ;
    Serial.println("CLOCK");
    pinMode(2, INPUT_PULLUP);
    pinMode(3, INPUT_PULLUP);
 
    attachInterrupt(0, pps_interrupt, FALLING);
    attachInterrupt(1, clk_interrupt, RISING);
}
 
void
loop()
{
    for (;;) {
        if (dirty) {
            dirty = false;
            Serial.print(pps_cnt);
            Serial.print(" ");
            Serial.println(clk_cnt);
             
        }
    }
}
