// Test Grove - Collision Sensor
#define LED 2 // yellow LED on Pitsco shield
#define COLLISION_SENSOR 5 //collision sensor is connected with D5 of Arduino

  
void setup()
{
    pins_init();
}


void turnOnLED()
{
    digitalWrite(LED,HIGH);//the LED is on
}

void turnOffLED()
{
    digitalWrite(LED,LOW);//the LED is off
}

void loop()
{
    if(isTriggered())
    {
        turnOnLED();
        delay(2000);
    }
    else turnOffLED();
}

void pins_init()
{
    pinMode(LED,OUTPUT);
    turnOffLED();
    pinMode(COLLISION_SENSOR,INPUT);
}

boolean isTriggered()
{
  int cc = 0; // collision count
  int nc = 0; // no collision count
  for (int i=0; i<10; i++) {
    if(!digitalRead(COLLISION_SENSOR)) // collision
      {if (nc >= 7) cc++;}  // count collisions only after minimum quiet period
    else // no collision
      {if (cc == 0) nc++;} // quiet period at beginning of sample
    delay(50);  // see if we are getting continuous readings
  } // for
  if (cc >= 2) return true;
  return false;
}

