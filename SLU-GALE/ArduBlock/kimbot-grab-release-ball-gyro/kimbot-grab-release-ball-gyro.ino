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
#define CLAW_OPEN 150
#define CLAW_CLOSE 115
#define ARM_DOWN 175
#define ARM_UP 60
#define OBJECT_UNDER_CLAW 7
#define GREEN_FLASH_DURATION 200

// globals
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

void BlinkYellowGreen() {
  // alternate yellow/green
  digitalWrite(2, LOW); // yellow light off
  digitalWrite(3, HIGH); //  green light on
  delay (700);
  digitalWrite(2, HIGH); // yellow light on
  digitalWrite(3, LOW); //  green light off
  delay (700);
}

void LightsOff() {
  digitalWrite(2, LOW); // yellow light off
  digitalWrite(3, LOW); //  green light off  
}

void StartRapidGreenFlash() {
  ResetTimer();
}

void FlashRapidGreen() {
  if (Timer() > GREEN_FLASH_DURATION*2)
    ResetTimer();
  else if (Timer() > GREEN_FLASH_DURATION)
    digitalWrite(3, LOW); //  green light off
  else
    digitalWrite(3, HIGH); //  green light on
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
  
  // wait for start button
  while(digitalRead(7)) {
      BlinkYellowGreen();
  }

  // wait until gyro is calibrated
  CalibrateGyro();
  LightsOff();

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

    bool LetDriverFindBall() {   // stay in loop until ball is found, counting lights
      digitalWrite( 3 , LOW ); // green light off
      bool foundBall = false;
      bool turnedAround = false;
      while (!foundBall && DiffFromCurrHeading(180) > GYRO_TOLERANCE_TURN*2) { 
        CountLightTransitions();
        foundBall = monUltrasonic.mesurer() < OBJECT_UNDER_CLAW;
      }
      turnedAround = DiffFromCurrHeading(180) <= GYRO_TOLERANCE_TURN*2;
      return (turnedAround);
    }

    bool GrabBall()
    {
      bool grabbedBall = false;
      delay( 700 );
      if (( ( monUltrasonic.mesurer() ) < ( OBJECT_UNDER_CLAW ) ))
      {
        servo_pin_10.write( CLAW_OPEN ); // claw open
        delay( 700 );
        servo_pin_11.write( ARM_DOWN ); // arm down
        delay( 800 );
        servo_pin_10.write( CLAW_CLOSE ); // claw close
        delay( 700 );
        servo_pin_11.write( ARM_UP ); // arm up
        delay( 700 );
        grabbedBall = monUltrasonic.mesurer() >= OBJECT_UNDER_CLAW; // if we did grab ball, it is no longer visible to sensor
        if (grabbedBall) digitalWrite( 3 , HIGH ); // green LED on
      }
      return (grabbedBall);
    }

    
    void LetDriverDriveHome (int countLimit) {
      LightCount = 0;
      while (LightCount <= countLimit) {
        FlashRapidGreen();
        CountLightTransitions();
        //delay(BNO055_SAMPLERATE_DELAY_MS);
      }
    }

void loop()
{
  // arm up, claw closed
  servo_pin_10.write( CLAW_CLOSE );
  servo_pin_11.write( ARM_UP );
  
  bool light_sensor = false ;
  bool turned_around = false;
  bool grabbed_ball = false;

  switch (State) {
    case 0: // driver will locate ball; try to pick it up
      turned_around = LetDriverFindBall();
      if (turned_around) State = 2; // must have found ball, so drive home!
      else State = 1;
      break;
    case 1:
      // successfully found ball, try to pick it up
      grabbed_ball = GrabBall();
      State = 0; // try again if necessary
      break;
    case 2:  // go back to home base
      StartRapidGreenFlash();
      LetDriverDriveHome(LightCount);
      servo_pin_10.write( CLAW_OPEN ); // claw open; release ball
      delay( 700 );
      State = 3;
      break; 
    case 3: // all done
      LightsOff();
      break;
    }
  }

