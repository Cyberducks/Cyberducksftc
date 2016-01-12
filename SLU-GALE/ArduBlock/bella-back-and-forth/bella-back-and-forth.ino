#include <Servo.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

Servo left_servo; // pin 9
Servo right_servo; // pin 8

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)

#define GYRO_TOLERANCE 10.0
#define SERVO_FORWARD 0
#define SERVO_REVERSE 110
#define SERVO_STOP 90

int State = 0;
unsigned long StartTime = 0;
int LightState = 0;
int LightCount = 0;

Adafruit_BNO055 bno = Adafruit_BNO055(55);

void ResetTimer () {
  StartTime = millis();
}

unsigned long Timer() {
  return millis() - StartTime;
}


void setup()
{
  pinMode(2, OUTPUT); // yellow LED
  pinMode(3, OUTPUT); // green LED
  pinMode(4, INPUT);  // line follower sensor
  pinMode(7, INPUT);  // green button
  
  /* Initialise the gyro sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    // Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1) {
        digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(1000);              // wait for a second
        digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
        delay(1000);              // wait for a second
    }
  }

  // ready for button push
  digitalWrite(3, HIGH); // green light on
  
  // wait for start button
  while(digitalRead(7));

  // wait until gyro is calibrated
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  do {
    bno.getCalibration(&system, &gyro, &accel, &mag);
  } while (gyro < 3);

  delay(1000);
  bno.setExtCrystalUse(true);

  // servos
  left_servo.attach(9);
  right_servo.attach(8);

  ResetTimer();

}

    // heading is never negative, but it can flip from 1 to 359, so do modulus 180
    // If this function returns negative, it means turns right to correct; positive means turn left
    float SubtractFromCurrHeading(float x) {
        /* Get a new sensor event */
        sensors_event_t event;
        bno.getEvent(&event);
        float result = 0.0;
        float ch = event.orientation.x;
        float diff = abs(ch - x);
        if (diff >= 180.0) { // more than 180deg apart, so flip
            result = 360.0 - diff;
            if (x < 180.0) {
                result = -result;
            }
        } else {
            result = ch - x;
        }
        return result;
    }

    float DiffFromCurrHeading(float x) {
        return abs(SubtractFromCurrHeading(x));
    }

    void StopMotors () {
      left_servo.write( SERVO_STOP );
      right_servo.write( SERVO_STOP );
      }
      
    boolean TurnByGyroAsync (boolean right, float deg) { // return true if turn is complete
        boolean result = false;
        int leftPower = right ? SERVO_FORWARD : SERVO_REVERSE;
        int rightPower = right ? SERVO_REVERSE : SERVO_FORWARD;
        int turnDegrees = right ? deg : - deg;
        if (DiffFromCurrHeading(turnDegrees) > GYRO_TOLERANCE) {
              if (SubtractFromCurrHeading(turnDegrees) > 0.0) {
                // we overshot; go back the other way
                rightPower = right ? SERVO_FORWARD : SERVO_REVERSE;
                leftPower = right ? SERVO_REVERSE : SERVO_FORWARD;
                }
              left_servo.write( leftPower );
              right_servo.write( rightPower );
        } else {
              // stop motors
              StopMotors();
              result = true;
        }
        return result;
    }

    void TurnByGyro (boolean right, float deg) {
      while (!TurnByGyroAsync(right, deg)) {
        //delay(BNO055_SAMPLERATE_DELAY_MS);
      }
    }

    void CountLightTransitions () {
      bool light_sensor = digitalRead(4);
      switch (LightState) {
        case 0: //  dark
          if (light_sensor) { // now light
            LightState = 1;
            LightCount++;
            digitalWrite( 2 , HIGH );
          }
          break;
        case 1: // light
          if (!light_sensor) { // now dark
            LightState = 0;
            digitalWrite( 2 , LOW );
          }
          break;
      }
    }
    
    void DriveStraightByGyro (float heading) {
      if (DiffFromCurrHeading(heading) < GYRO_TOLERANCE/2.0) {
          // on course, go straight
          left_servo.write( SERVO_FORWARD );
          right_servo.write( SERVO_FORWARD );
      } else { // not on course, correct
          if (SubtractFromCurrHeading(heading) > 0) {
              // correct left
              left_servo.write( SERVO_STOP );
              right_servo.write( SERVO_FORWARD );
          } else {
              // correct right
              left_servo.write( SERVO_FORWARD );
              right_servo.write( SERVO_STOP );
          }
      }
    }

    void DriveStraightByGyroAndCounts (float heading, int countLimit) {
      LightCount = 0;
      while (LightCount <= countLimit) {
        DriveStraightByGyro(heading);
        CountLightTransitions();
        //delay(BNO055_SAMPLERATE_DELAY_MS);
      }
    }

void loop()
{
  bool light_sensor = false ;
  switch (State) {
    case 0: 
      // go straight
      DriveStraightByGyroAndCounts(0.0, 20);
      // turn 180 degrees
      TurnByGyro (true, 170.0);
      // go straight
      DriveStraightByGyroAndCounts(170.0, 20);
      State = 1;
      break;
    case 1:
      StopMotors();
      digitalWrite(3, LOW); // green light off
      digitalWrite(2, LOW); // yellow light off
      break;
    }
  }




