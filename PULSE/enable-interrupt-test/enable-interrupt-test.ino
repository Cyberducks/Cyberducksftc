#include <Servo.h>
#include <EnableInterrupt.h>

#define MY_PIN 5 // we could choose any pin

uint16_t pwm_value = 0;
Servo servo_pin_8;

// see http://blog.mired.org/2015/10/a-close-look-at-pwm-input.html
// value range: 1016 to 2024, middle is 1516

void change() {
  static unsigned long prev_time = 0;

  if (digitalRead(MY_PIN))
    prev_time = micros();
  else
    pwm_value = micros() - prev_time;
}

void setup() {
  servo_pin_8.attach(8);
  pinMode(MY_PIN, INPUT_PULLUP);
  enableInterrupt(MY_PIN, &change, CHANGE);
  Serial.begin(115200);
}

void loop() {
  uint16_t pwmin;

  noInterrupts();
  pwmin = pwm_value ;
  interrupts();  

  Serial.println(pwmin);
  //delay(500);

  if (pwmin > 1800)
  {
    servo_pin_8.write( 180 );
  } else {
    servo_pin_8.write( 95 );
  }
}
