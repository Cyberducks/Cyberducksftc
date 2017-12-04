#include <PULSE.h>
#include <EnableInterrupt.h>
#include <ChainableLED.h>

PULSE pulse;

#define CH1_PIN 2
#define CH3_PIN 3
#define LEFT_EAR_PIN 4
#define RIGHT_EAR_PIN A3
#define LEFT_EYE_PIN A1
#define RIGHT_EYE_PIN A2

#define NUM_LEDS  2
#define LEFT_EYE 0
#define RIGHT_EYE 1

ChainableLED leds(LEFT_EYE_PIN, RIGHT_EYE_PIN, NUM_LEDS);

// pwm value range for ch3: 1008 to 2008, middle is 1500
// pwm value range for ch1: 984 to 1984, middle is 1480
#define CH3MIN 1008
#define CH3MAX 2008
#define CH3MID 1504
#define CH3SCALE 5 // was 50 when I was squaring power
#define CH1MIN 984
#define CH1MAX 1984
#define CH1MID 1480
#define CH1SCALE 5 // was 50 when I was squaring power
#define DEADZONE 5
#define SONARTOLERANCE 20
#define SERVOCENTER 102
#define SERVOMAX 170
#define SERVOMIN 10

uint16_t pwm_value_ch1 = 0;
uint16_t pwm_value_ch3 = 0;

int ServoPosition = SERVOCENTER;
float leftHue = 0.5;
float rightHue = 0.5;

// see http://blog.mired.org/2015/10/a-close-look-at-pwm-input.html

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
  pinMode(CH1_PIN, INPUT_PULLUP);
  enableInterrupt(CH1_PIN, &change_ch1, CHANGE);
  pinMode(CH3_PIN, INPUT_PULLUP);
  enableInterrupt(CH3_PIN, &change_ch3, CHANGE);

  leds.init();
  
  // Serial.begin(115200);

  // pulse.setServoPosition (1, ServoPosition); // this should center head, but doesn't, even with delay(1000)
  
  pulse.PulseBegin();
  pulse.setMotorInvert (1,1);
}

void loop()
{
  // Decode PWM signal from radio and set drive motors
  
  uint16_t pwmin1, pwmin3;
  //noInterrupts();
  pwmin1 = pwm_value_ch1 ;
  pwmin3 = pwm_value_ch3 ;
  //interrupts(); 

  long powerStraight = ((long)pwmin3 - CH3MID) / CH3SCALE;
  long powerSteer = ((long)pwmin1 - CH1MID) / CH1SCALE;
  // long powerLeft = powerStraight * abs(powerStraight) + powerSteer * abs(powerSteer);
  // long powerRight = powerStraight * abs(powerStraight) - powerSteer * abs(powerSteer);
  long powerLeft = powerStraight + powerSteer ;
  long powerRight = powerStraight - powerSteer;

  if (powerLeft > 100) powerLeft = 100;
  else if (powerLeft < -100) powerLeft = -100;
  else if (abs(powerLeft) < DEADZONE) powerLeft = 0;
  
  if (powerRight > 100) powerRight = 100;
  else if (powerRight < -100) powerRight = -100;
  else if (abs(powerRight) < DEADZONE) powerRight = 0;

  // Serial.print(pwmin1);
  // Serial.print(" ");
  // Serial.println(powerLeft);
  
  pulse.setMotorPowers (powerLeft, powerRight); 

  // Position head so sonar sensors are returning about the same value
  
  int leftEar = pulse.readSonicSensorCM(LEFT_EAR_PIN);
  int rightEar = pulse.readSonicSensorCM(RIGHT_EAR_PIN);

  // Serial.print(rightEar);
  // Serial.print(" ");
  // Serial.println(rightHue);

  if (abs(leftEar - rightEar) > SONARTOLERANCE) {
    if (leftEar > rightEar) ServoPosition += 2;
    else ServoPosition -= 2;
    if (ServoPosition > SERVOMAX || ServoPosition < SERVOMIN) ServoPosition = SERVOCENTER;
    // ServoPosition = min(SERVOMAX, max(SERVOMIN, ServoPosition));
  }
  
  pulse.setServoPosition (1, ServoPosition);

  leftHue = min (150.0, leftEar) / 150.0; // 100cm is about 3 feet
  rightHue = min (150.0, rightEar) / 150.0; // 100cm is about 3 feet

  leds.setColorHSB(LEFT_EYE, leftHue, 1.0, 0.5);
  leds.setColorHSB(RIGHT_EYE, rightHue, 1.0, 0.5);

}


