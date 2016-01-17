#include <Servo.h>
#include <Ultrasonic.h>

Servo servo_pin_10;
Servo servo_pin_11;
//libraries at http://www.duinoedu.com/
Ultrasonic monUltrasonic(5);

void setup()
{
  servo_pin_10.attach(10);
  servo_pin_11.attach(11);
  pinMode( 3 , OUTPUT);
}

void loop()
{
  servo_pin_10.write( 90 );
  servo_pin_11.write( 160 );
  if (( ( monUltrasonic.mesurer() ) < ( 8 ) ))
  {
    digitalWrite( 3 , HIGH );
    servo_pin_10.write( 180 );
    delay( 700 );
    servo_pin_11.write( 50 );
    delay( 800 );
    servo_pin_10.write( 90 );
    delay( 700 );
  }
  else
  {
    digitalWrite( 3 , LOW );
  }
  servo_pin_11.write( 160 );
  delay( 1000 );
}


