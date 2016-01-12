bool _ABVAR_1_light_sensor= false ;

void setup()
{
  pinMode( 4, INPUT);
  Serial.begin(9600);
}

void loop()
{
  _ABVAR_1_light_sensor = digitalRead(4) ;
  Serial.print("light =");
  Serial.print(_ABVAR_1_light_sensor);
  Serial.println();
}


