#include <Servo.h>

Servo servo_pin_8;
Servo servo_pin_9;

void setup()
{
  pinMode( 7, INPUT);
  servo_pin_8.attach(8);
  servo_pin_9.attach(9);
  do
  {
    delay( 10 );
  }
  while(digitalRead(7));

}

void loop()
{
  servo_pin_8.write( 50 );
  servo_pin_9.write( 50 );
  delay( 1000 );
  servo_pin_8.write( 90 );
  servo_pin_9.write( 90 );
  delay( 10000 );
}


