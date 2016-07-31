#include <Servo.h>
#include <Ultrasonic.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

Servo left_servo; // pin 9
Servo right_servo; // pin 8

Servo servo_pin_10; // claw
Servo servo_pin_11; // arm
//libraries at http://www.duinoedu.com/
Ultrasonic monUltrasonic(5);

#define FORWARD true
#define REVERSE false
#define SERVO_FORWARD 0
#define SERVO_REVERSE 110
#define SERVO_STOP 95
#define OBJECT_UNDER_CLAW 7

// globals
int State = 0;
unsigned long StartTime = 0;
int LightState = 0;
int LightCount = 0;

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
  
}

    void StopMotors () {
      left_servo.write( SERVO_STOP );
      right_servo.write( SERVO_STOP );
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

    bool SweepForBall(int countLimit) {
      LightCount = 0;
      bool foundBall = false;
      while (LightCount <= countLimit && !foundBall) { // sweep right counts
        left_servo.write( SERVO_FORWARD );
        right_servo.write( SERVO_REVERSE );
        foundBall = monUltrasonic.mesurer() < OBJECT_UNDER_CLAW;
        CountLightTransitions();
      }
      StopMotors(); delay (500); LightCount = 0;
      while (LightCount <= countLimit*2 && !foundBall) { // sweep back to center then left
        left_servo.write( SERVO_REVERSE );
        right_servo.write( SERVO_FORWARD );
        foundBall = monUltrasonic.mesurer() < OBJECT_UNDER_CLAW;
        CountLightTransitions();
      }
      StopMotors(); delay (500); LightCount = 0;
      while (LightCount <= countLimit && !foundBall) { // sweep back to center
        left_servo.write( SERVO_FORWARD );
        right_servo.write( SERVO_REVERSE );
        foundBall = monUltrasonic.mesurer() < OBJECT_UNDER_CLAW;
        CountLightTransitions();
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

void loop()
{
  // arm up, claw closed
  servo_pin_10.write( 115 );
  servo_pin_11.write( 60 );
  
  bool light_sensor = false ;
  bool found_ball = false;
  bool grabbed_ball = false;
  
  switch (State) {
    case 0: // go forward
      DriveByCounts(FORWARD, 2);
      StopMotors();
      State = 1;
      break;
    case 1:
      delay (500);
      found_ball = SweepForBall(1);
      delay (500);
      if (found_ball) State = 2;
      else State = 0; // keep creeping forward
      break;
    case 2:
      // successfully found ball, try to pick it up
      grabbed_ball = GrabBall();
      if (grabbed_ball) State = 4; // done
      else State = 3; // backup and try again
      break;
    case 3: // backup
      DriveByCounts(REVERSE, 2);
      StopMotors();
      State = 1;
      break;
    case 4:
      StopMotors();
      digitalWrite(3, LOW); // green light off
      digitalWrite(2, LOW); // yellow light off
      break;
    }
  }

