  /* From N6QW using the 160 X 128 TFT Color display with the Si5351 Clock Generator.
    The sketch includes selectable USB/LSB, a Tune function with 988 Hz tone and the Fout is 1X
    the LO (
    
    June 15,2018 Modified to include the two frequewncy VFO Scheme.
    June 16, 2018 with many thanks to DuWayne KV4QB the frequency flciker is totally gone and
    the code has been simplified.
    July 2018 now the standard for the Sudden Transceiver

        
    TFT pinout information
    
    The VCC Pin is not used 
    
    LED = 3.3 Volts DC
    SCK = Pin D13
    SDA = Pin D11
    AO = Pin D9
    Reset = Pin D8
    CS = Pin D10
    GND = Ground
    Vcc = Not Used
    
    Tone = Pin D6
    LED = Pin D7
    SW = Pin A1 Selects Upper and Lower Sideband
    SW1 = Pin A2 Slects TUNE
    Encoder Button = Pin A3
    Encoder A = Pin D2
    Encoder B = Pin D3
    
    LED = Pin D7

    VFO = Pin A0
    USB/LSB = A1
    Tune = A2
    Step = A3
    SDA = A4
    SCL = A5
    VFO Select = D5  digitally selects which VFO to use-
    SM = A6 //S Meter pin
    TX = D4  // Keys the Transmit
    */
    
    
    #include <SPI.h>
    #include <Adafruit_GFX.h>
    #include <Adafruit_ST7735.h> // Hardware-specific library
    #include "Rotary.h"
    #include <toneAC.h>
    #define TONE_PIN 6
    #define NOTE_B5  988
    
    
    #if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
    #endif

    #define __CS 10
    #define __DC 9
    #define __RST 8
    #define ENCODER_B    2                      // Encoder pin A
    #define ENCODER_A    3                     // Encoder pin B
    #define ENCODER_BTN  A3
    
    
    // Color definitions
    #define  BLACK   0x0000
    #define BLUE    0x001F
    #define RED     0xF800
    #define GREEN   0x07E0
    #define CYAN    0x07FF
    #define MAGENTA 0xF81F
    #define YELLOW  0xFFE0  
    #define WHITE   0xFFFF
    
    Adafruit_ST7735 display = Adafruit_ST7735(__CS,  __DC, __RST);
    
    #include "si5351.h"
    #include "Wire.h"
    
    Si5351 si5351;
    long int frq;
    int_fast32_t rx = 16198500L; // Starting frequency of VFO operating frequency plus offset which is selectable IF = 9.0 MHz
    int_fast32_t rx2= 0L; // variable to hold the updated frequency
    int_fast32_t increment = 100; // starting VFO update increment in HZ.
    int_fast32_t bfo = 8998500L; // default offset sideband inversion thus USB
    int_fast32_t rx1 = 16198500L;
    int_fast32_t rx3 = 16198500L;
    int_fast32_t vfoA = 1;
    int_fast32_t vfoB = 1;
    int_fast32_t old_vfoA = 1;
    int_fast32_t old_vfoB = 1;

  
    volatile uint32_t oldAB = 1;
    volatile uint32_t vfo = 1;
     
  
    int RunOnce = 1; 
    
    String hertz = " 100";
    byte ones,tens,hundreds,thousands,tenthousands,hundredthousands,millions ;  //Placeholders
    int buttonstate = 0;
    int buttonstate2 = 0;
    
    const int tonepin = 6;
    const int SW = A1; //selects upper or lower sideband
    const int SW1 = A2; // provides the TUNE fucntion
    const int LED = 7; //Controls the transmitter in TUNE simple 5 VDC reed relay
    
   
    int backlight = 0;
    int i = 0;
    const int VFO = A0;  // 
    int val = 0;
    
    const int TX = 4;  //USE VFO A only on transmit
  
    const int VS = 5; //digital select of VFO important for transmit select
    

    int adcval = A6; // Analog Pin A6 for the S Meter function

    
    int buttonState = 0;
    int lastButtonState = 0;
    
    Rotary r = Rotary(2,3); // sets the pins the rotary encoder uses.  Must be interrupt pins.
    
    
    
    void setup() {
      Serial.begin(9600);
        
         
        
          
         PCICR |= (1 << PCIE2);
         PCMSK2 |= (1 << PCINT18) | (1 << PCINT19);
         sei();
         //display.begin();
         display.initR(INITR_BLACKTAB);
          display.setRotation(1); // landsacape versus portrait
       
         display.fillScreen(BLACK);
         
         
         pinMode(SW, INPUT); digitalWrite(SW,HIGH);  // Selects either USB or LSB````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````[
         
         pinMode(SW1, INPUT); digitalWrite(SW1,HIGH);//Tune
         
         pinMode(LED, OUTPUT);digitalWrite(LED,LOW);
         
         pinMode(backlight, OUTPUT);
         pinMode(A3,INPUT); digitalWrite(A3,HIGH);// Connect to a button that goes to GND on push
         pinMode(A1,INPUT); digitalWrite(A1,HIGH); 
        
         pinMode(A0, INPUT); 
         pinMode(A6,INPUT); //digitalWrite(A6, HIGH); //Problem with S Meter reading
         pinMode(TX, INPUT); digitalWrite(TX, HIGH);
         pinMode(VS, INPUT);  digitalWrite(VS,HIGH);
         pinMode(7,OUTPUT);
        
         
         // init done
         // set backlight level
       
        

         
        
      
      //while (!Serial);
      //tft.begin();
      #if defined(__MK20DX128__) || defined(__MK20DX256__)
      display.setBitrate(24000000);
      #endif
      
      
        //  initialize the Si5351
    
          si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 100);
          //si5351.set_correction(100);
          
          si5351.set_pll(SI5351_PLL_FIXED, SI5351_PLLA);
          
     
    
         
          si5351.set_freq_manual(bfo, 0, SI5351_CLK2);
          
          si5351.drive_strength(SI5351_CLK0,SI5351_DRIVE_8MA); 
          
         
    
    } 
    
    
    void setincrement(){
        if (increment == 10){increment = 100;  hertz = "100";}
        else if (increment == 100){increment = 1000; hertz="  1K";}
        else if (increment == 1000){increment = 10000; hertz=" 10K"; }
        else if (increment == 10000){increment = 100000; hertz="100K";}
       // else if (increment == 100000){increment = 1000000; hertz="  1M";}  
        else{increment = 10; hertz = " 10";};  
    
         delay(100); // Adjust this delay to speed up/slow down the button menu scroll speed.
    }

 
 

 void showFreq1(){

       
         
          millions = int(((rx1)-bfo)/1000000);
          hundredthousands = ((((rx1)-bfo )/100000)%10);
          tenthousands = ((((rx1)-bfo )/10000)%10);
          thousands = ((((rx1)-bfo )/1000)%10);
          hundreds = ((((rx1)-bfo )/100)%10);
          tens = ((((rx1)-bfo )/10)%10);
          ones = ((((rx1)-bfo )/1)%10);
           display.setTextSize(3);
           display.setTextColor(YELLOW,BLACK);
          if (millions > 9){display.setCursor(25,5);}
          else{display .setCursor(35,5);}
          
          display.print(millions);
          
          display.print(hundredthousands);
          display.print(tenthousands);
          display.print(thousands);
        
          display.setTextColor(RED,BLACK );
          display.setTextSize(2);
          
          display.print(hundreds);
          display.print(tens);
          display.print(ones);
             
                
    
     
 }


   void showFreq2(){

      
         
          millions = int(((rx3)-bfo)/1000000);
          hundredthousands = ((((rx3)-bfo )/100000)%10);
          tenthousands = ((((rx3)-bfo )/10000)%10);
          thousands = ((((rx3)-bfo )/1000)%10);
          hundreds = ((((rx3)-bfo )/100)%10);
          tens = ((((rx3)-bfo )/10)%10);
          ones = ((((rx3)-bfo )/1)%10);
            
            display.setTextSize(3);
           display.setTextColor(YELLOW,BLACK);
          if (millions > 9){display.setCursor(25,100);}
          else{display .setCursor(35,100);}
          
          display.print(millions);
          
          display.print(hundredthousands);
          display.print(tenthousands);
          display.print(thousands);
        
          display.setTextColor(RED,BLACK);
          display.setTextSize(2);
          
          display.print(hundreds);
          display.print(tens);
          display.print(ones);

    
   }


 void showFreq3(){  // used to show transmit on the screen

       
         
          millions = int(((rx1)-bfo)/1000000);
          hundredthousands = ((((rx1)-bfo )/100000)%10);
          tenthousands = ((((rx1)-bfo )/10000)%10);
          thousands = ((((rx1)-bfo )/1000)%10);
          hundreds = ((((rx1)-bfo )/100)%10);
          tens = ((((rx1)-bfo )/10)%10);
          ones = ((((rx1)-bfo )/1)%10);
           display.setTextSize(1);
           display.setTextColor(YELLOW,BLACK);
          if (millions > 9){display.setCursor(56,44);}
          else{display .setCursor(56,44);}
          
          display.print(millions);
          
          display.print(hundredthousands);
          display.print(tenthousands);
          display.print(thousands);
        
          display.setTextColor(RED,BLACK );
          display.setTextSize(1);
          
          display.print(hundreds);
          display.print(tens);
          display.print(ones);
             
                
    
     
 }


    void showFreq4(){// used to show transmit on the screen

       
         
          millions = int(((rx3)-bfo)/1000000);
          hundredthousands = ((((rx3)-bfo )/100000)%10);
          tenthousands = ((((rx3)-bfo )/10000)%10);
          thousands = ((((rx3)-bfo )/1000)%10);
          hundreds = ((((rx3)-bfo )/100)%10);
          tens = ((((rx3)-bfo )/10)%10);
          ones = ((((rx3)-bfo )/1)%10);
           display.setTextSize(1);
           display.setTextColor(YELLOW,BLACK);
          if (millions > 9){display.setCursor(56,55);}
          else{display .setCursor(56,55);}
          
          display.print(millions);
          
          display.print(hundredthousands);
          display.print(tenthousands);
          display.print(thousands);
        
          display.setTextColor(RED,BLACK );
          display.setTextSize(1);
          
          display.print(hundreds);
          display.print(tens);
          display.print(ones);
             
                
    
     
 }

    ISR(PCINT2_vect) {
      unsigned char result = r.process();
      if (result) {    
        if (result == DIR_CW){rx=rx+(1*increment);} // account that rx = 1 times the frequency so the increment must be 4* or /.25
        else {rx = rx-(1*increment);};       
          if (rx >=40100000){rx=rx2;}; // UPPER VFO LIMIT = 1X the opearting frequency + offset
          if (rx <=9100000){rx=rx2;}; // LOWER VFO LIMIT = 1 X the opearting frequency + Offset
          
          }
    
           
    
        
            
            
      
    
    }
  //*********************************** LOOP ***********************  
    void loop() {   // The loop is small as all action are sub routines

              SplashScreen();
              RunOnce = 0;
                  
              display.setRotation(1); // landsacape versus portrait

    
              CheckMode(); //********Moved void checkBand out of the loop
              CheckSideBand();
           // CheckVFO(); //tells which vfo to use but is logic selected versus switch selected
              CheckFreq();
              CheckDisplay();
              CheckSM();
              CheckTX();
              CheckVS(); //
              showFreq1();
              showFreq2();

              
             lastButtonState = buttonState;


            display.setCursor(116,43);
            display.setTextColor(YELLOW);
            display.setTextSize(1);
            display.print(hertz);
 

 
          
           buttonstate = digitalRead(A3);
           if(buttonstate == LOW) {
            setincrement(); 
         
           
           display.fillRect(115, 40, 140, 10, BLACK);
     
            
 
 
            display.setCursor(116,43);
            display.setTextColor(BLACK); // blanks out the old setting
            display.setTextSize(1);
            display.print(hertz);
            delay(10);
     
          
        }
        
        delay(25);
          
   
  }      //End of Loop
      
//*******************************Tune Function  Check Mode *************************     
         void CheckMode(){
        buttonState = digitalRead(SW1); // creates a 10 second tuning pulse trani 50% duty cycle and makes TUNE appear on the screen
       if(buttonState != lastButtonState){
        if(buttonState == LOW){


          
          // si5351.set_freq(vfoA  , SI5351_PLL_FIXED, SI5351_CLK0);
           digitalWrite(LED,HIGH);
          
           display.setTextSize(1);
           display.setTextColor(RED);
           display.setCursor(132, 67);  
           display.print("TUNE");
           digitalWrite(A0, HIGH);
        
       
           display.fillCircle(152, 57, 3, GREEN); 

           
         delay(12);
         for(int i = 0; i < 100; i++) {
         tone(6, NOTE_B5);
         delay(75);
         noTone(6);
         delay(25);
         
         }
             
        } 
       else{
         digitalWrite(LED,LOW);
         display.setTextSize(1);    // This prints a Black TUNE over the RED TUNE and makes it disappear from the scereen
         display.setTextColor(BLACK);
         display.setCursor(132, 67);
         display.print("TUNE");
      
     
         noTone(6);
    
       }
          
      
         delay(50);
    
    }
    
      
    
    }   
    //******************************** Check SideBand ******************************


    void CheckSideBand() {

               
         if(digitalRead(SW)){  //********If SW is true do the following.    
            //Upper Sideband  --- Sideband Inversion
           
           bfo = 9001500L;
           si5351.set_freq_manual( bfo, 0, SI5351_CLK2);{
           
            if (bfo = 9001500L);
           
           

            display.fillCircle(46,52, 3, RED);  //shows which sideband is selected
            display.fillCircle(46,68, 3, BLACK);
        
          
          }
        
           
        
           
      }
      else{                //**********if not, do this.
          
           //LOWER Sideband --sideband inversion
          
         
          bfo = 8998500L;
          //si5351.set_freq( bfo, 0, SI5351_CLK2);
          si5351.set_freq( bfo, SI5351_CLK2);
        
         
           display.fillCircle(46,68, 3, RED);  // shows whihc sideband is selected
           display.fillCircle(46,52, 3, BLACK);  //shows which sideband is selected

      } 
    }
// *********************Check Digital VFO Select *****************************

/* by using another pin on the Arduino we can digitally write the VFO selection such as you would have for receiving 
 *  on one VFO and TRansmitting on another. Right now you can select whihc VFO reievies but the old logic will oscillate
 *  beteen the two VFO's on transmit if you try to do it direct. Old logic would let you transmit on the VFO 
 *  that was doing the receiving.
 *  
 *  
 */
 //*********************VFO A or VFO B ***************************

  void CheckVS(){  //thus a toggle switch controls writing which VFO indirectly

        digitalRead(5);
        if(digitalRead(5) == HIGH){
            digitalWrite(A0, HIGH);
            useVFOA();
           
            }
           else {
           digitalWrite(A0, LOW);
           useVFOB();
        
            }
    
       }
//***************************************************** Check VFO *********************

/*  // Don't use this
    void CheckVFO() {
           digitalRead(A0);
          if(digitalRead(A0)== HIGH){

          
          useVFOA();
        
          showFreq();
          z = 1;
          
          }
          else {
          useVFOB();
         
           showFreq();
          z = 2;
    
            }
    }

*/
    // *************************************VFO A*******************

       void useVFOA(){   // Has logic so you can have the correct display in concert with USB/LSB

         
           CheckFreq1();

           display.fillCircle(6, 25, 4, GREEN);
           display.fillCircle(6, 121, 4, BLACK);  // Active VFO and Blanks out

            
    
          digitalRead(A0) == HIGH;

          display.setTextSize(3);

     
            rx = rx1;
     
             rx1 == vfoA;
             vfoA = rx1;
             old_vfoA = vfoA;
            
             si5351.set_freq_manual(vfoA , SI5351_PLL_FIXED, SI5351_CLK0);
            
          }
 
    
    // ******************************** VFO B *****************************
  void useVFOB(){
        
                         
            CheckFreq2();

            display.fillCircle(6, 121, 4, GREEN);  // Active VFO and Blanks out
            display.fillCircle(6, 25, 4, BLACK);
           
            digitalRead(A0) == LOW;
            
            
            display.setTextSize(3);
          
            rx = rx3;
        
              rx3 == vfoB;
              vfoB = rx3;
              old_vfoB = vfoB;
              si5351.set_freq_manual(vfoB , SI5351_PLL_FIXED, SI5351_CLK0);

             }




    //**************************************** Frequency Change **************************

    void CheckFreq(){


                if (rx != rx2){  

              
          
                
             
           
            si5351.set_freq_manual(rx  , SI5351_PLL_FIXED, SI5351_CLK0);
            Serial.println(rx);
            si5351.set_freq_manual(bfo  , SI5351_PLL_FIXED, SI5351_CLK2);
            rx2 = rx;
          
       

   
       }


    }




    
 //************************* Frequency Change 1 *************************


  void CheckFreq1(){

          

           if(rx != rx2) { 

            
            si5351.set_freq_manual(rx1  , SI5351_PLL_FIXED, SI5351_CLK0);
          
            si5351.set_freq_manual(bfo  , SI5351_PLL_FIXED, SI5351_CLK2);
            rx1 = rx;
           
            showFreq1();

           }     
  }

//************************* Check Frequency 2 *******************************

void CheckFreq2(){

             
          if (rx != rx2){   //This is key to separating the frequencies
          
         
            si5351.set_freq_manual(rx3  , SI5351_PLL_FIXED, SI5351_CLK0);
           
            si5351.set_freq_manual(bfo  , SI5351_PLL_FIXED, SI5351_CLK2);
            rx3 = rx;

           showFreq2();

 }
}

//***********************************CHECK DISPLAY *********************


  void CheckDisplay(){  //Sets up the display with stock items

            display.setCursor(144,43);
            display.setTextColor(YELLOW);
            display.setTextSize(1);
            display.print("Hz");
          
            display.setTextSize(2);
            display.setTextColor(WHITE);
            display.setCursor(5,44);
            display.println("USB");
            display.setTextColor(WHITE);
            display.setCursor(5,62);  
            display.println("LSB");
           
       
          
            display.setCursor(106, 54);
            display.setTextColor(CYAN);  
            display.setTextSize(1);
            display.print("   N6QW");

            display.setTextSize(1);
            display.setTextColor(WHITE);
            display.setCursor(1,33);
            display.println(" Scratch Built Sudden XCVR");
            display.setCursor(54,69);
            
            display.println("T/R A or B");
          
            display.setTextSize(1);
            display.setTextColor(GREEN);
            display.setCursor(6,83);
            display.println("S Meter");
            display.setCursor(118,67);
            display.print("S9");
            display.drawRect(56,79,95,15,RED);
           display.fillRect(57, 86, 93,4, BLACK);
            display.setTextColor(RED);

            for(int i = 0; i < 8; i++) {   // A Trick to add "Tick Marks" to the S Meter
          
            display.fillRect(60 + 10* i,80,1,5,WHITE);}

            
        
                

            display.setTextSize(2);   //Designating VFO A or B readout location
            display.setTextColor(RED);
            display.setCursor(2,5);
            display.print("A");
            display.setCursor(2,100);
            display.print("B");
            display.drawRect(17,2,140,29,WHITE); // Frame the readout VFO A
            display.drawRect(17,97,140,28,WHITE); //Frame the readout VFO B
                
            
  }

  //************************************* S Meter ***************************************


     void CheckSM(){
          
        adcval = analogRead(A6);  //S meter routine
        
       
        
       
        val= 15*log((adcval-200)/20);

        
       
    
        
       
        display.fillRect(58,87, val,2, YELLOW);
        
        delay(20);
        
        
        
       

        
       }
// ***************************************Check for TRansmit ******************************************
/* This has proven to be a nasty problem for me as I have not been able to implement this in software. Try as I might 
 *  the software does not follow what I want to do. I have total functionality with two separate VFO's --my failure is how 
 *  to transmit on one while receiving on the other. Finally I hit on a simple hardware solution. Pin A0 is either set high or 
 *  low depedning on whether Pin 5 is grounded via a switch. So lets add some hardware in that loop like a NC relay. So here is
 *  how it would work. 
 *  1) In the receiove mode by engaging Pin 5 high or low with an external switch we have a choice of VFO A or VFO B
 *  2) Let us say we want to make VFO A the Receive and Transmit VFO and VFO B Receive only
 *  3) When the PTT Switch ground Pin 4 it also causes a NC relay to open. So if you were receiving on VFO B the break in the circuit
 *     would cause the controlling VFO to be VFO A whihc means you would be transmitting on VFO A.
 *  4) If you purposefully open the VFO A/B sweitch then you would receive and transmit entirely on VFO A.
 *  5) Adding yet another switch in the loop in the line feeding the juice (or ground) to the NC relay Then you would receive and transmit only on VFO B
 *  6) Normal Opeartion would entail opening the A/B select swithc so that you would pick the transmit frequency on VFO A. Then closing the switch you would
 *     have VFO A fixed for transmit and tune around on VFO B. Opening the switch then tuning and transmitting is on VFO A
 */
 void CheckTX() { //Green Dot comes on to PTT Also selects which VFO A is used for transmit
          
       digitalRead(4);

         if(digitalRead(4) == LOW){

                if(digitalRead(5) == HIGH){
               
                showFreq3();
                
                display.fillCircle(152, 57, 3, GREEN); }

                 if(digitalRead(5) == LOW){

               
                showFreq4();
                

                display.fillCircle(152, 57, 3, GREEN); }
                  
                 }

            
                
                  
                
                 
        
                 else {

                display.fillRect(56 , 44 , 50,20, BLACK);
           

                

                display.fillCircle(152, 57, 3, BLACK);

                digitalWrite(7, LOW);
             
                  CheckVS();
                  
            }
}           
 
    //************************************ Splash Screen***********************


    void SplashScreen() {      // Fun with Run Once Splash Screens
    if (RunOnce == 1) {
    display.setTextColor(YELLOW);
    display.fillScreen(BLACK);
    display.setCursor(50, 10);
    display.setTextSize(3);
    display.println("N6QW");
    display.setCursor(40,45);
    display.setTextSize(1);
    display.setTextColor(RED);
    display.setCursor(50, 50);
    
    display.println("Illegitimus");
    display.setCursor(40, 65);
    display.println("Non-Carborundum");
    delay(200);
    display.fillScreen(BLACK);
    display.setTextColor(YELLOW);
    display.setTextSize(3);
    display.setCursor(50,10);
    display.println("N6QW");
    display.setTextSize(1);
    display.setCursor(40 , 45);
    display.print("Genius at Work!");
    display.setCursor(1, 60);
    display.println("Scratch Built Sudden XCVR");
    display.setCursor(20,80);
    display.print("KNACKER's == Know How!");
    display.setCursor(2,105);
    display.print("  www.n6qwradiogenius.us");
    delay(200);
    display.fillScreen(BLACK);
    display.setTextColor(YELLOW);
    display.setTextSize(2);
    
    display.setCursor(35,30);
    display.println("SEABEE's");
    display.setCursor(37,60);
    display.println("Can Do!");

   
    

    
 
    delay(200);
    RunOnce = 0;

    
    
    display.fillScreen(BLACK);
    
  }
}
