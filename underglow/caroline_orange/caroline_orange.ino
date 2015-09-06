// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!
 
#define GREENPIN 3
#define REDPIN 5
#define BLUEPIN 6
#define BUTTONPIN 2
 
#define FADESPEED 15     // make this higher to slow down
 
void setup() {
  pinMode(REDPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BUTTONPIN, INPUT);
}
 
int buttonPressed () {  // return true if button pressed
  int val = 0;
  val = digitalRead(BUTTONPIN);
  return (val != HIGH);
}
    
int g = 0;
 
void loop() {

  // burnt orange, see http://colors.findthedata.com/q/654/10857/What-are-the-RGB-values-of-Burnt-Orange

  analogWrite (GREENPIN, g);
  analogWrite (REDPIN, 255);
  analogWrite (BLUEPIN, 0);
  
    if (buttonPressed()) {
      // wait for button to be released
      while (buttonPressed()) { delay(100); }
      g += 1;
    }
    
    
    
//      // fade up green
//    for (r = 0; r < 40; r++) { 
//      analogWrite(GREENPIN, r);
//      delay(FADESPEED);
//    } 
// 
//    // fade down red color
//    for (r = 40; r > 0; r--) { 
//      analogWrite(GREENPIN, r);
//      delay(FADESPEED);
//    } 


}
