#include <Ultrasonic.h>

//libraries at http://www.duinoedu.com/
Ultrasonic monUltrasonic(5);

void setup()
{
  pinMode( 3 , OUTPUT);
}

void loop()
{
  if (( ( monUltrasonic.mesurer() ) < ( 5 ) ))
  {
    digitalWrite( 3 , HIGH );
  }
  else
  {
    digitalWrite( 3 , LOW );
  }
}


