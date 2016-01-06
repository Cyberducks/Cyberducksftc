/*
Adafruit Arduino - Lesson 5. Serial Monitor
*/

int latchPin = 5;
int clockPin = 6;
int dataPin = 4;

//LED Pin
int ledPin1 = 9; 
int ledPin2 = 10; 
int ledPin3 = 11; 
int ledPin4 = 12; 

byte leds = 0;

void setup() 
{
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);
  Serial.begin(9600);
  while (! Serial); // Wait untilSerial is ready - Leonardo
  Serial.println("Enter LED Number 1 to 4 or 'x' to clear");
}

void loop() 
{
  if (Serial.available())
  {
    char ch = Serial.read();
    if (ch >= '1' && ch <= '4')
    {
      int led = ch - '1' + 9;
      Serial.print("Turned on LED ");
      Serial.println(ch);
      analogWrite(led, 200);  //write the value
    }
    if (ch == 'x')
    {
      analogWrite(ledPin1, 0);  //write the value
      analogWrite(ledPin2, 0);  //write the value
      analogWrite(ledPin3, 0);  //write the value
      analogWrite(ledPin4, 0);  //write the value
      Serial.println("Cleared");
    }
  }
}


