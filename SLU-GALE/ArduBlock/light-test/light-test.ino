
void setup()
{
  pinMode( 4, INPUT);
  Serial.begin(9600);
}

void loop()
{
      int state = 0;
    int counter = 0;
    int counts = 20;
    bool light_sensor = false ;
    while (counter < counts) {
      // check for transition from light to dark and count transitions
      light_sensor = digitalRead(4) ;
      switch (state) {
        case 0 : 
          if (light_sensor == 0) { // dark
            // transition
            counter ++;
            state = 1;
          }
          break;
        case 1: 
          if (light_sensor == 1) { // bright
            // transition
            counter ++;
            state = 0;
          }
          break;
      }
  Serial.print("light =");
  Serial.print(light_sensor);
  Serial.print(" count =");
  Serial.print(counter);
  Serial.println();
}
}


