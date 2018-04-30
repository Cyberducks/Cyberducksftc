#include <Servo.h>
#include <Ultrasonic.h>
#include <EnableInterrupt.h>

#define CH1_PIN 5
#define CH3_PIN 6

uint16_t pwm_value_ch1 = 0; // left and right
uint16_t pwm_value_ch3 = 0; // forward and backward

#define LOOKING_STATE 0
#define CLAWOPEN_STATE 1
#define ARMDOWN_STATE 2
#define CLAWCLOSE_STATE 3
#define ARMUP_STATE 4
#define RESTING_STATE 5

#define ARMUP_POS  60 // was 60
#define ARMDOWN_POS 150 // was 180
#define SONAR_REST_TIME 300 // msecs

int state = LOOKING_STATE;
long timerstart = 0;
long sonarThreshold = 0;

Servo servo_pin_8;
Servo servo_pin_9;
Servo servo_pin_10;
Servo servo_pin_11;
Servo servo_pin_12;

#define ROTARY_ANGLE_SENSOR A1 // on some Pitsco shields this is erroneously labeled A0

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
  servo_pin_11.write( ARMUP_POS ); // arm up

  pinMode(CH1_PIN, INPUT_PULLUP);
  enableInterrupt(CH1_PIN, &change_ch1, CHANGE);
  pinMode(CH3_PIN, INPUT_PULLUP);
  enableInterrupt(CH3_PIN, &change_ch3, CHANGE);
  
  // Serial.begin(9600);
    
  pinMode( 7 , INPUT); // green button
  long knobMin = 0; long knobMax = 1023;
  while ( digitalRead(7) ) { // wait for green button
    long knobReading = analogRead(ROTARY_ANGLE_SENSOR); // value is typically between 0 and 1023 when batt power is on
    // knobMin = min(knobMin, knobReading); knobMax = max(knobMax, knobReading);  // adjust range if it changes
    sonarThreshold = (knobReading - knobMin) * 10 / (knobMax - knobMin); // should give value between 0 and 10
    unsigned int sonarReading = monUltrasonic_pin4.distanceRead(CM);
    //    Serial.print(knobMin);
    //    Serial.print (" ");
    //    Serial.print(knobReading);
    //    Serial.print (" ");
    //    Serial.print(knobMax);
    //    Serial.print (" ");
    //    Serial.print(sonarThreshold);
    //    Serial.print (" ");
    //    Serial.println(sonarReading);
    if (sonarReading < sonarThreshold) {digitalWrite(3 , HIGH);} // light on
    else {digitalWrite(3 , LOW);} // light off
    delay(SONAR_REST_TIME);
  } // while waiting for green button
  
}

void loop()
{
  
  switch (state) {
    case LOOKING_STATE:
      //Serial.print("Distance in CM: "); Serial.println(monUltrasonic_pin4.distanceRead()); delay(1000);
      digitalWrite(3 , LOW); // light off
      if (monUltrasonic_pin4.distanceRead(CM) < sonarThreshold ) {   // was mesurer(1), before library update
        digitalWrite(3 , HIGH); // light on
        timerstart = millis();
        state= CLAWOPEN_STATE; 
      } else { // give ultrasonic a short break
        timerstart = millis();
        state = RESTING_STATE;
      }
      break;
    case RESTING_STATE: 
      if (millis() > timerstart + SONAR_REST_TIME) {  
        timerstart = millis();
        state = LOOKING_STATE;
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
      servo_pin_11.write( ARMDOWN_POS ); // arm down
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
      servo_pin_11.write( ARMUP_POS ); // arm up
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


