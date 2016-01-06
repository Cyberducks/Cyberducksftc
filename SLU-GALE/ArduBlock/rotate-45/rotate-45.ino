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

Adafruit_BNO055 bno = Adafruit_BNO055(55);

void ResetTimer () {
  StartTime = millis();
}

unsigned long Timer() {
  return millis() - StartTime;
}

void setup()
{
  // initialize digital pin 13 as an output for blinking
  pinMode(13, OUTPUT);
  
  /* Initialise the gyro sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    // Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1) {
        digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(1000);              // wait for a second
        digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
        delay(1000);              // wait for a second
    }
  }
    
  // wait for start button
  pinMode( 7, INPUT);
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
      
    boolean TurnByGyro (boolean right, float degrees) { // return true if turn is complete
        boolean result = false;
        int leftPower = right ? SERVO_FORWARD : SERVO_REVERSE;
        int rightPower = right ? SERVO_REVERSE : SERVO_FORWARD;
        int turnDegrees = right ? degrees : - degrees;
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


void loop()
{
  switch (State) {
    case 0: // go straight 3 seconds
      DriveStraightByGyro(0.0);
      if (Timer() > 3000) {
        StopMotors();
        State = 1;
      }
      break;
    case 1: // turn 180 degrees
      if (TurnByGyro (true, 170.0)) {
        State = 2;
        ResetTimer();
        }
      delay(BNO055_SAMPLERATE_DELAY_MS);
      break;
    case 2: // go straight 3 seconds
      DriveStraightByGyro(170.0);
      if (Timer() > 3000) {
        StopMotors();
        State = 3;
      }
      break;
    case 3:
      StopMotors();
    }
  }



