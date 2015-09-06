// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!
 
#define REDPIN 5
#define BLUEPIN 6
#define BUTTONPIN 2
 
#define FADESPEED 15     // make this higher to slow down
 
void setup() {
  pinMode(REDPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  pinMode(BUTTONPIN, INPUT);
}
 
int buttonPressed () {  // return true if button pressed
//  int val = 0;
//  val = digitalRead(BUTTONPIN);
//  return (val != HIGH);
  return 0;
}
    
 
void loop() {
  int r, g, b;
  int doChange = 1; // true means keep changing color
  
    // fade up red color
    for (r = 0; r < 256 && doChange; r++, doChange = !buttonPressed()) { 
      analogWrite(REDPIN, r);
      delay(FADESPEED);
    } 
 
    // fade up blue
    for (r = 0; r < 256 && doChange; r++, doChange = !buttonPressed()) { 
      analogWrite(BLUEPIN, r);
      delay(FADESPEED);
    } 
 
    // fade down red color
    for (r = 255; r > 0 && doChange; r--, doChange = !buttonPressed()) { 
      analogWrite(REDPIN, r);
      delay(FADESPEED);
    } 
  
    // fade down blue
    for (r = 255; r > 0 && doChange; r--, doChange = !buttonPressed()) { 
      analogWrite(BLUEPIN, r);
      delay(FADESPEED);
    } 
  
  if (!doChange) {
    // wait for button to be released
    while (buttonPressed());
    delay(FADESPEED);
  
    // wait here until button is pressed again (stop changing)
    while (!buttonPressed());
    delay(FADESPEED);

    // wait for button to be released
    while (buttonPressed());
    delay(FADESPEED);
    }
  
  doChange = 1; // back to top of loop and start changing again
  analogWrite(REDPIN, 0);
  analogWrite(BLUEPIN, 0);


}
