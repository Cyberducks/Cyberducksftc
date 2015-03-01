int redPin = 6;    // LED connected to digital pin 9
int greenPin = 3;
int bluePin = 9;

void setup()  { 
  // nothing happens in setup 
} 

void rgb(int rLevel, int gLevel, int bLevel) {
 analogWrite(redPin, rLevel);
 analogWrite(greenPin, gLevel);
 analogWrite(bluePin, bLevel); 
}

void fade(int fromRed, int fromGreen, int fromBlue, int toRed, int toGreen, int toBlue) {
  int steps = 100;
  int redDelta = toRed - fromRed;
  int greenDelta = toGreen - fromGreen;
  int blueDelta = toBlue - fromBlue;
  
  int redFactor = redDelta/steps;
  int greenFactor = greenDelta/steps;
  int blueFactor = blueDelta/steps;
  
  int curRed = fromRed;
  int curGreen = fromGreen;
  int curBlue = fromBlue;
  
  while (steps > 0) {
     analogWrite(redPin, curRed);
     analogWrite(greenPin, curGreen);
     analogWrite(bluePin, curBlue);
     
     curRed += redFactor;
     curGreen += greenFactor;
     curBlue += blueFactor;
     steps--;
     
     delay(25); 
  }
}

void loop()  {
  rgb(0xFF,0x10,0);
  delay(1000);
  fade(0xFF,0x10,0, 255, 255, 255);
  fade(255,255,255,0xFF,0x10,0);
  // blue 
//  rgb(0,0,255);
//  delay(3000);
  
  // green
//  rgb(0,255,0);
//  delay(3000);
  
  // red
//  rgb(255,0,0);
//  delay(3000);
//rgb(255,0,0);
//delay(6000);
//
//rgb(0,255,0);
//delay(3000);
//
//rgb(0,0,255);
//delay(3000);
  
  // orange
//  rgb(0xFF,0x10,0);
//  delay(3000);
  
  // purple
//  rgb(50,0,50);
//  delay(3000);
  
  // yellow
//  rgb(255,125,0);
//  delay(3000);
  
  // white
//  rgb(255,255,255);
//  delay(3000);
  
  
  
  
//  rgb(255,255,255);
//  delay(3000);
}
