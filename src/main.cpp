#include <Arduino.h>
#include <Servo.h>


Servo Servomotor; 

void setup()
{
  Servomotor.attach(23);  
}

void loop()
{
  delay(1000);
  Servomotor.write(90);
  delay(1000);
  Servomotor.write(180);
   delay(1000);
  
}