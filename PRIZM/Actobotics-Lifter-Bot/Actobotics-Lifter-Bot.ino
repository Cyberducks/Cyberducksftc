/*  PRIZM Controller example program
 *  Spin DC motor channel 1 for 5 seconds, then coast to a stop.
 *  After stopping, wait for 2 seconds, spin in opposite direction.
 *  Continue to repeat until RED reset button is pressed.
 *  author PWU on 08/05/2016
*/

  #include <PRIZM.h>    // include the PRIZM library in the sketch
  PRIZM prizm;          // instantiate a PRIZM object “prizm” so we can use its functions

  #define SPEEDUP 50
  #define SPEEDDOWN -50
  
  int state = 0;
  
void setup() {        
  prizm.PrizmBegin();   // Initialize the PRIZM controller
  prizm.setRedLED(LOW); // off
  prizm.setMotorPower(1,0); // stopped
}

void loop() {     // main loop



  
  switch (state) {  // state machine
    case 0:
      // starting state, and lift is somewhere in the middle... lower it
      state = 1;
      break;
    case 1: // lowering
      if(prizm.readLineSensor(5) == HIGH)  { // lift is at bottom, stop!  and then raise it
        prizm.setMotorPower(1,0);
        delay(2000); 
        state = 2;
      } else {
        prizm.setRedLED(HIGH); 
        prizm.setGreenLED(LOW); 
        prizm.setMotorPower(1,SPEEDDOWN);
      }
      break;
    case 2: // raising
      if (prizm.readLineSensor(4) == LOW)  { // lift is at top, stop!  and then lower it
        prizm.setMotorPower(1,0);
        delay(2000); 
        state = 1;
      } else {
        prizm.setRedLED(LOW); 
        prizm.setGreenLED(HIGH); 
        prizm.setMotorPower(1,SPEEDUP);
      }
      break;  
  }    
  
}






