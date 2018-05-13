// originally TechShopBot-line-follower-Zitzman.abp (ArduBlock)

#include <Adafruit_NeoPixel.h>
#include <Servo.h>

Adafruit_NeoPixel strip_pin6 = Adafruit_NeoPixel(6,6, NEO_GRB + NEO_KHZ800);
int _ABVAR_1_Pixel = 0 ;
int _ABVAR_2_Color = 0 ;
int EncoderCount = 0;
int PauseEncoderCount = 0;
bool EncoderState = false; bool PauseEncoderState = false;
bool LineColorIsBlack = false;
// unsigned long _ABVAR_3_Time2 = 0UL ;
// unsigned long _ABVAR_4_Time3 = 0UL ;
unsigned long LineColorTimer = 0UL;
Servo servo_pin_13;
Servo servo_pin_11;

#define ENCODER_COUNT_FOR_OFF_LINE 4
#define ENCODER_COUNT_FOR_BACKUP 18
#define ENCODER_COUNT_FOR_FORWARD_AGAIN  32
#define ENCODER_COUNT_FOR_PAUSE 19

#define PAUSE_COUNTER_RUNNING 0
#define PAUSE_COUNTER_SUSPENDED 1

int PauseCounterState = PAUSE_COUNTER_SUSPENDED;

void ResetEncoder() {
  EncoderState = digitalRead(5); // line finder on wheel
  EncoderCount = 0;
}

void ResetPauseEncoder() {
  PauseEncoderState = digitalRead(5); // line finder on wheel
  PauseEncoderCount = 0;
}

void CheckEncoder() {
  int currentState = digitalRead(5); // line finder on wheel
  if (EncoderState != currentState) {  // state has changed, bump count
    EncoderCount++;
    EncoderState = currentState;
  }
  if (PauseEncoderState != currentState) {  // state has changed, bump count
    PauseEncoderCount++;
    PauseEncoderState = currentState;
  }
}

void UpdatePauseCounterState (bool runCounter) {
  // start pause counter at same time as we switch from backup to forward again
  if (!runCounter) {PauseCounterState = PAUSE_COUNTER_SUSPENDED;}
  else if (PauseCounterState == PAUSE_COUNTER_SUSPENDED) { // was suspended; reset and run counter
    ResetPauseEncoder();
    PauseCounterState = PAUSE_COUNTER_RUNNING;
  }
}

//void UpdatePauseCounterState (bool OnLine) {
//  if (!OnLine) { PauseCounterState = PAUSE_COUNTER_SUSPENDED; }
//  else if (PauseCounterState == PAUSE_COUNTER_SUSPENDED) { // just found line again; resume counting
//    ResetPauseEncoder();
//    PauseCounterState = PAUSE_COUNTER_RUNNING;
//  }
//}

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
  // _ABVAR_3_Time2 = millis() ;
  // _ABVAR_4_Time3 = 0 ;
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
  
  if (PauseCounterState == PAUSE_COUNTER_RUNNING && PauseEncoderCount > ENCODER_COUNT_FOR_PAUSE)
  {
    UpdatePauseCounterState (false);  // stop counting for pause
    servo_pin_13.write( 93 ); // stop mid-line so driver can shoot
    servo_pin_11.write( 90 );
    for (_ABVAR_1_Pixel= 1; _ABVAR_1_Pixel<= ( 6 ); _ABVAR_1_Pixel++ )
    {
      strip_pin6.setPixelColor(( _ABVAR_1_Pixel - 1 ),255 ,0 ,0 );
      strip_pin6.show();
    }
    delay( 3000 );
    for (_ABVAR_1_Pixel= 1; _ABVAR_1_Pixel<= ( 6 ); _ABVAR_1_Pixel++ )
    {
      strip_pin6.setPixelColor(( _ABVAR_1_Pixel - 1 ),0 ,0 ,0 );
      strip_pin6.show();
    }
  }
  else  // not pausing to shoot
  {
    if (digitalRead(4) == LineColorIsBlack)  // check line finder
    {
      ResetEncoder() ;  
      servo_pin_11.write( 180 );  // turn line-left when on line
      servo_pin_13.write( 0 );  // go line-forward when on line
    }
    else // not on line
    {
        // check how long we've been off line, using encoder counts
        if (EncoderCount > ENCODER_COUNT_FOR_FORWARD_AGAIN) {
          servo_pin_13.write( 0 );  // second: go line-forward again
          UpdatePauseCounterState (true);
        } else if (EncoderCount > ENCODER_COUNT_FOR_BACKUP) {
          servo_pin_13.write( 180 );  // first: go line-backward
        } else if (EncoderCount > ENCODER_COUNT_FOR_OFF_LINE) {
          servo_pin_13.write( 0 ); // not on line, continue driving forward for a while
        } else { // main mode: go line-forward
          servo_pin_13.write( 0 );  // main mode: go line-forward
        }
      servo_pin_11.write( 0 );  // turn line-right when off line
    }
  } // not paused to shoot
} // loop


