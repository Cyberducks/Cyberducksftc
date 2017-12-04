#include <Servo.h>
#include <Ultrasonic.h>
#include <EnableInterrupt.h>

#define CH1_PIN 5
#define CH3_PIN 6

uint16_t pwm_value_ch1 = 0;
uint16_t pwm_value_ch3 = 0;

#define LOOKING_STATE 0
#define CLAWOPEN_STATE 1
#define ARMDOWN_STATE 2
#define CLAWCLOSE_STATE 3
#define ARMUP_STATE 4

int state = LOOKING_STATE;
long timerstart = 0;

Servo servo_pin_8;
Servo servo_pin_9;
Servo servo_pin_10;
Servo servo_pin_11;
Servo servo_pin_12;

//libraries at http://duinoedu.com/dl/lib/grove/EDU_Ultrasonic_Grove/ 
Ultrasonic monUltrasonic_pin4(4);

// see http://blog.mired.org/2015/10/a-close-look-at-pwm-input.html
// pwm value range: 1016 to 2024, middle is 1516

void change_ch1() {
  static unsigned long prev_time_ch1 = 0;
  if (digitalRead(CH1_PIN))
    prev_time_ch1 = micros();
  else
    pwm_value_ch1 = micros() - prev_time_ch1;
}

void change_ch3() {
  static unsigned long prev_time_ch3 = 0;
  if (digitalRead(CH3_PIN))
    prev_time_ch3 = micros();
  else
    pwm_value_ch3 = micros() - prev_time_ch3;
}

void setup()
{
  pinMode( 3 , OUTPUT);
  servo_pin_8.attach(8);
  servo_pin_9.attach(9);
  servo_pin_10.attach(10);
  servo_pin_11.attach(11);
  servo_pin_12.attach(12);

  pinMode(CH1_PIN, INPUT_PULLUP);
  enableInterrupt(CH1_PIN, &change_ch1, CHANGE);
  pinMode(CH3_PIN, INPUT_PULLUP);
  enableInterrupt(CH3_PIN, &change_ch3, CHANGE);
}

void loop()
{
  // servo_pin_10.write( 115 ); // claw close
  // servo_pin_11.write( 60 ); // arm up
  switch (state) {
    case LOOKING_STATE:
      digitalWrite(3 , LOW); // light off
      if (( ( monUltrasonic_pin4.mesurer(1) ) < ( 5 ) )) { 
        digitalWrite(3 , HIGH); // light on
        timerstart = millis();
        state= CLAWOPEN_STATE; 
      }
      break;
    case CLAWOPEN_STATE: 
      servo_pin_10.write( 150 ); // claw open
      if (millis() > timerstart + 700) {  // delay(700)
        timerstart = millis();
        state = ARMDOWN_STATE;
      }
      break;
    case ARMDOWN_STATE:
      servo_pin_11.write( 180 ); // arm down
      if (millis() > timerstart + 800) { // delay( 800 );
        timerstart = millis();
        state = CLAWCLOSE_STATE;
      }
      break;
    case CLAWCLOSE_STATE:
      servo_pin_10.write( 115 ); // claw close
      if (millis() > timerstart + 700 ) { // delay( 700 );
        timerstart = millis();
        state = ARMUP_STATE;
      }
      break;
    case ARMUP_STATE:
      servo_pin_11.write( 60 ); // arm up
      if (millis() > timerstart + 700 ) { // delay( 700 );
        timerstart = millis();
        state = LOOKING_STATE;
      }
      break;
  } // switch

  uint16_t pwmin1, pwmin3;
  //noInterrupts();
  pwmin1 = pwm_value_ch1 ;
  pwmin3 = pwm_value_ch3 ;
  //interrupts(); 

  if (pwmin3 > 1800) {
    servo_pin_8.write( 0 ); // backward
    servo_pin_9.write( 0 );
  } else if (pwmin3 < 1200 && pwmin3 > 200) {
    servo_pin_8.write( 180 );  // forward
    servo_pin_9.write( 180 );
  } else {
    servo_pin_8.write( 95 ); // stop
    servo_pin_9.write( 95 );
  }

  if (pwmin1 > 1800) {
    servo_pin_12.write( 180 ); // left
  } else if (pwmin1 < 1200 && pwmin1 > 200) {
    servo_pin_12.write( 0 );  // right
  } else {
    servo_pin_12.write( 95 ); // stop
  }

  

  
}


