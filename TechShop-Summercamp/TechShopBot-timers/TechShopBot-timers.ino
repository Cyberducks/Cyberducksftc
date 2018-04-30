// originally TechShopBot-line-follower-Zitzman.abp (ArduBlock)

#include <Adafruit_NeoPixel.h>
#include <Servo.h>

Adafruit_NeoPixel strip_pin6 = Adafruit_NeoPixel(6,6, NEO_GRB + NEO_KHZ800);
int _ABVAR_1_Pixel = 0 ;
int _ABVAR_2_Color = 0 ;
int EncoderCount = 0;
int EncoderState = false;
int LineColorIsBlack = false;
unsigned long _ABVAR_3_Time2 = 0UL ;
unsigned long _ABVAR_4_Time3 = 0UL ;
unsigned long LineColorTimer = 0UL;
Servo servo_pin_13;
Servo servo_pin_11;

void ResetEncoder() {
  EncoderState = digitalRead(5); // line finder on wheel
  EncoderCount = 0;
}

void CheckEncoder() {
  int currentState = digitalRead(5); // line finder on wheel
  if (EncoderState != currentState) {  // state has changed, bump count
    EncoderCount++;
    EncoderState = currentState;
  }
}

void CheckForLineColorSwitch() {
  while (!digitalRead(7)) {  // while green button is held down
    if (millis() > LineColorTimer + 1000) { // green button held down for 1 sec after start
      // flip color
      LineColorIsBlack = !LineColorIsBlack;
      digitalWrite( 2 , LineColorIsBlack ); // yellow LED
      LineColorTimer = millis(); // reset timer
    }
  }
}

void setup()
{
  pinMode( 4 , INPUT); // line finder
  pinMode( 5 , INPUT); // line finder on wheel
  pinMode( 7 , INPUT); // green button
  pinMode(2, OUTPUT); // yellow LED
  pinMode(3, OUTPUT); // green LED
  strip_pin6.begin();
  strip_pin6.show();
  servo_pin_13.attach(13);
  servo_pin_11.attach(11);

  _ABVAR_1_Pixel = 0 ;
  _ABVAR_2_Color = 0 ;

  digitalWrite( 3 , LOW ); // green LED
  digitalWrite( 2 , LOW ); // yellow LED

  while ( digitalRead(7) ) // while waiting for green button press, play neo-pixels
  {
    _ABVAR_1_Pixel = ( ( _ABVAR_1_Pixel + 1 ) % 6 ) ;
    if (( ( _ABVAR_1_Pixel ) == ( 0 ) ))
    {
      _ABVAR_2_Color = ( ( _ABVAR_2_Color + 20 ) % 255 ) ;
    }
    strip_pin6.setPixelColor(_ABVAR_1_Pixel,( ( 255 - _ABVAR_2_Color ) % 255 ) ,0 ,_ABVAR_2_Color );
    strip_pin6.show();
    delay( 100 );
  } // waiting for green button


  for (_ABVAR_1_Pixel= 1; _ABVAR_1_Pixel<= ( 6 ); _ABVAR_1_Pixel++ ) // turn off neopixels
  {
    strip_pin6.setPixelColor(( _ABVAR_1_Pixel - 1 ),0 ,0 ,0 );
    strip_pin6.show();
  }

  delay( 100 );
  _ABVAR_3_Time2 = millis() ;
  _ABVAR_4_Time3 = 0 ;
  LineColorTimer = millis();
  ResetEncoder();
  digitalWrite( 3 , HIGH ); // green LED

  // check for switch between black line and white line
  CheckForLineColorSwitch();
}

void loop()
{

  // check encoder for state change
  CheckEncoder();
  
  if (( ( ( _ABVAR_4_Time3 ) > ( 0 ) ) && ( ( millis() ) > ( ( _ABVAR_4_Time3 + 3000 ) ) ) ))
  {
    servo_pin_13.write( 91 ); // stop mid-line so driver can shoot
    servo_pin_11.write( 90 );
    for (_ABVAR_1_Pixel= 1; _ABVAR_1_Pixel<= ( 6 ); _ABVAR_1_Pixel++ )
    {
      strip_pin6.setPixelColor(( _ABVAR_1_Pixel - 1 ),255 ,0 ,0 );
      strip_pin6.show();
    }
    delay( 3000 );
    _ABVAR_4_Time3 = 0 ;
    for (_ABVAR_1_Pixel= 1; _ABVAR_1_Pixel<= ( 6 ); _ABVAR_1_Pixel++ )
    {
      strip_pin6.setPixelColor(( _ABVAR_1_Pixel - 1 ),0 ,0 ,0 );
      strip_pin6.show();
    }
  }
  else
  {
    if (digitalRead(4) == LineColorIsBlack)  // check line finder
    {
      _ABVAR_3_Time2 = millis() ;  // turn line-left when on line
      servo_pin_11.write( 180 );
    }
    else
    {
      if (( ( millis() ) > ( ( _ABVAR_3_Time2 + 1500 ) ) )) // off line too long, go into find-line mode
      {
        if (( ( millis() ) > ( ( _ABVAR_3_Time2 + 2750 ) ) )) 
        {
          servo_pin_13.write( 0 );  // second: go line-forward again
        }
        else
        {
          _ABVAR_4_Time3 = millis() ; // first: go line-backward
          servo_pin_13.write( 180 );
        }
      }
      else
      {
        servo_pin_13.write( 0 );  // main mode: go line-forward
      }
      servo_pin_11.write( 0 );  // turn line-right when off line
    }
  }
}


