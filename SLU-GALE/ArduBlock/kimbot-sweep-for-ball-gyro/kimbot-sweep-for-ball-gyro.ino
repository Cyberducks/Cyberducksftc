#include <Servo.h>
#include <Ultrasonic.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <math.h>

Servo left_servo; // pin 9
Servo right_servo; // pin 8

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)

Servo servo_pin_10; // claw
Servo servo_pin_11; // arm
//libraries at http://www.duinoedu.com/
Ultrasonic monUltrasonic(5);

#define GYRO_TOLERANCE 2.0
#define GYRO_TOLERANCE_TURN 10.0
#define FORWARD true
#define REVERSE false
#define SERVO_FORWARD 0
#define SERVO_REVERSE 110
#define SERVO_STOP 95
#define OBJECT_UNDER_CLAW 7
#define TURN_RIGHT true
#define TURN_LEFT false
#define CREEP_COUNT 1
#define DEG_TO_RADS 1 / 360 * M_PI

// globals
int State = 0;
unsigned long StartTime = 0;
int LightState = 0;
int LightCount = 0;
float HeadingToBall = 0.0;
float DistanceTraveled = 0;
Adafruit_BNO055 bno = Adafruit_BNO055(55);

void CalibrateGyro () {
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  digitalWrite(3, LOW); // green light off
  do {
    bno.getCalibration(&system, &gyro, &accel, &mag);
  } while (gyro < 3);
  delay(1000);
  bno.setExtCrystalUse(true);
  digitalWrite(3, HIGH); // green light on
}

void setup()
{
  servo_pin_10.attach(10); // claw
  servo_pin_11.attach(11); // arm
  left_servo.attach(9);
  right_servo.attach(8);

  pinMode(2, OUTPUT); // yellow LED
  pinMode(3, OUTPUT); // green LED
  pinMode(4, INPUT);  // line follower sensor
  pinMode(7, INPUT);  // green button

  /* Initialise the gyro sensor */
  digitalWrite(3, LOW); // green light off
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
  CalibrateGyro();

}

    float GetCurrentHeading () {
        /* Get a new sensor event */
        sensors_event_t event;
        bno.getEvent(&event);
        float ch = event.orientation.x; 
        return ch;
    }
    
    // heading is never negative, but it can flip from 1 to 359, so do modulus 180
    // If this function returns negative, it means turns right to correct; positive means turn left
    float SubtractFromCurrHeading(float x) {
        float result = 0.0;
        float ch = GetCurrentHeading();
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
        //int turnDegrees = right ? deg : - deg;  // 45 deg turn left from 0 should be -45, but let caller do that
        if (DiffFromCurrHeading(deg) > GYRO_TOLERANCE_TURN) {
              if (SubtractFromCurrHeading(deg) > 0.0) {
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
            digitalWrite( 2 , HIGH ); // yellow LED
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

    void DriveByCounts (bool forward, int countLimit) {
      LightCount = 0;
      int servoPos = forward ? SERVO_FORWARD : SERVO_REVERSE;
      while (LightCount <= countLimit) {
        left_servo.write( servoPos );
        right_servo.write( servoPos );
        CountLightTransitions();
      }
      StopMotors();
    }

    bool SweepForBall(float centerHead) {   // sweep 45 degrees right and left of center
      LightCount = 0;
      bool foundBall = false;
      while (!TurnByGyroAsync(TURN_RIGHT, centerHead+45.0) && !foundBall) { // sweep right counts
        left_servo.write( SERVO_FORWARD );
        right_servo.write( SERVO_REVERSE );
        foundBall = monUltrasonic.mesurer() < OBJECT_UNDER_CLAW;
      }
      StopMotors(); delay (500); LightCount = 0;
      while (!TurnByGyroAsync(TURN_LEFT, centerHead-45.0) && !foundBall) { // sweep back to center then left
        left_servo.write( SERVO_REVERSE );
        right_servo.write( SERVO_FORWARD );
        foundBall = monUltrasonic.mesurer() < OBJECT_UNDER_CLAW;
      }
      StopMotors(); delay (500); LightCount = 0;
      while (!TurnByGyroAsync(TURN_RIGHT, centerHead) && !foundBall) { // sweep back to center
        left_servo.write( SERVO_FORWARD );
        right_servo.write( SERVO_REVERSE );
        foundBall = monUltrasonic.mesurer() < OBJECT_UNDER_CLAW;
      }
      StopMotors();
      return (foundBall);
    }

    bool GrabBall()
    {
      bool grabbedBall = false;
      delay( 700 );
      if (( ( monUltrasonic.mesurer() ) < ( OBJECT_UNDER_CLAW ) ))
      {
        digitalWrite( 3 , HIGH ); // green LED
        servo_pin_10.write( 180 ); // claw open
        delay( 700 );
        servo_pin_11.write( 175 ); // arm down
        delay( 800 );
        servo_pin_10.write( 115 ); // claw close
        delay( 700 );
        servo_pin_11.write( 60 ); // arm up
        delay( 700 );
        grabbedBall = monUltrasonic.mesurer() >= OBJECT_UNDER_CLAW; // if we did grab ball, it is no longer visible to sensor
      }
      else
      {
        digitalWrite( 3 , LOW );
      }
      return (grabbedBall);
    }

    void DriveStraightByGyro (float heading) {
      if (DiffFromCurrHeading(heading) < GYRO_TOLERANCE) {
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
  // arm up, claw closed
  servo_pin_10.write( 115 );
  servo_pin_11.write( 60 );
  
  bool light_sensor = false ;
  bool found_ball = false;
  bool grabbed_ball = false;
  int directionToHome = TURN_RIGHT;
    
  switch (State) {
    case 0: // go forward
      DriveStraightByGyroAndCounts(HeadingToBall, CREEP_COUNT);
      StopMotors();
      DistanceTraveled += CREEP_COUNT * abs(cos(HeadingToBall*DEG_TO_RADS));  // as we wander around ball, not moving directly away from home
      State = 1;
      break;
    case 1:
      delay (500);
      found_ball = SweepForBall(HeadingToBall);
      delay (500);
      if (found_ball) State = 2;
      else State = 0; // keep creeping forward
      break;
    case 2:
      // successfully found ball, try to pick it up
      grabbed_ball = GrabBall();
      if (grabbed_ball) State = 4; // got it!!
      else State = 3; // backup and try again
      break;
    case 3: // backup
      HeadingToBall = GetCurrentHeading(); // ball is nearby to our front; adjust
      DriveByCounts(REVERSE, CREEP_COUNT);
      StopMotors();
      DistanceTraveled -= CREEP_COUNT * abs(cos(HeadingToBall*DEG_TO_RADS));  // we're backing, but probably not straight back toward home
      State = 1;
      break;
    case 4:  // go back to home base
      directionToHome = GetCurrentHeading() > 0 ? TURN_RIGHT : TURN_LEFT;
      TurnByGyro (directionToHome, 180.0);
      DriveStraightByGyroAndCounts(180, (int)DistanceTraveled);
      StopMotors();
      servo_pin_10.write( 180 ); // claw open; release ball
      delay( 700 );
      State = 5;
      break; 
    case 5: // all done
      StopMotors();
      digitalWrite(3, LOW); // green light off
      digitalWrite(2, LOW); // yellow light off
      break;
    }
  }

