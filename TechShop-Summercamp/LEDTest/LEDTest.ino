/*************************************************************************
* File Name          : GroveLEDDemoCode.ino
* Author             : Seeedteam
* Version            : V1.1
* Date               : 18/2/2012
* Description        : Demo code for Grove - LED
*************************************************************************/

#define LED 5 //connect LED to digital pin5
void setup() {
    // initialize the digital pin2 as an output.
    pinMode(LED, OUTPUT);
}

void loop() {
    digitalWrite(LED, HIGH);   // set the LED on
    delay(1500);               // for 500ms
    digitalWrite(LED, LOW);   // set the LED off
    delay(500);
}
