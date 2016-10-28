#include <Servo.h>
#include <Ultrasonic.h>

Servo servo_pin_11;
//libraries at http://www.duinoedu.com/
Ultrasonic monUltrasonic(5);

void setup()
{
  servo_pin_11.attach(11);
  Serial.begin(9600);
}

void loop()
{
  servo_pin_11.write( 90 );
  Serial.print("message");
  Serial.print(" ");
  Serial.print("sonar =");
  Serial.print(" ");
  Serial.print(monUltrasonic.mesurer());
  Serial.print(" ");
  Serial.println();
}


