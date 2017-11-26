#include <PULSE.h>
#include <EnableInterrupt.h>

PULSE pulse;

#define CH4_PIN 2
#define CH3_PIN 3

// pwm value range for ch3: 1008 to 2008, middle is 1500
// pwm value range for ch4: 984 to 1984, middle is 1480
#define CH3MIN 1008
#define CH3MAX 2008
#define CH3MID 1504
#define CH3SCALE 50
#define CH4MIN 984
#define CH4MAX 1984
#define CH4MID 1480
#define CH4SCALE 50
#define DEADZONE 5

uint16_t pwm_value_ch4 = 0;
uint16_t pwm_value_ch3 = 0;

// see http://blog.mired.org/2015/10/a-close-look-at-pwm-input.html

void change_ch4() {
  static unsigned long prev_time_ch4 = 0;
  if (digitalRead(CH4_PIN))
    prev_time_ch4 = micros();
  else
    pwm_value_ch4 = micros() - prev_time_ch4;
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
  pinMode(CH4_PIN, INPUT_PULLUP);
  enableInterrupt(CH4_PIN, &change_ch4, CHANGE);
  pinMode(CH3_PIN, INPUT_PULLUP);
  enableInterrupt(CH3_PIN, &change_ch3, CHANGE);

  // Serial.begin(115200);
  
  pulse.PulseBegin();
  pulse.setMotorInvert (1,1);
}

void loop()
{
  uint16_t pwmin4, pwmin3;
  //noInterrupts();
  pwmin4 = pwm_value_ch4 ;
  pwmin3 = pwm_value_ch3 ;
  //interrupts(); 

  long powerStraight = ((long)pwmin3 - CH3MID) / CH3SCALE;
  long powerSteer = ((long)pwmin4 - CH4MID) / CH4SCALE;
  long powerLeft = powerStraight * abs(powerStraight) + powerSteer * abs(powerSteer);
  long powerRight = powerStraight * abs(powerStraight) - powerSteer * abs(powerSteer);

  if (powerLeft > 100) powerLeft = 100;
  else if (powerLeft < -100) powerLeft = -100;
  else if (abs(powerLeft) < DEADZONE) powerLeft = 0;
  
  if (powerRight > 100) powerRight = 100;
  else if (powerRight < -100) powerRight = -100;
  else if (abs(powerRight) < DEADZONE) powerRight = 0;

  // Serial.print(pwmin4);
  // Serial.print(" ");
  // Serial.println(powerLeft);
  
  pulse.setMotorPowers (powerLeft, powerRight); 

}


