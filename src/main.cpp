#include <Stepper.h>
#include <Arduino.h>
// Motor pin definitions:
#define motorPin1  8      // IN1 on the ULN2003 driver
#define motorPin2  9      // IN2 on the ULN2003 driver
#define motorPin3  10     // IN3 on the ULN2003 driver
#define motorPin4  11     // IN4 on the ULN2003 driver
const int stepsPerRevolution = 2048;
int analogPin = A3;
float voltage_tasmota=0;
Stepper myStepper(stepsPerRevolution, motorPin1, motorPin3, motorPin2, motorPin4);

void setup() {
  Serial.begin(9600);
  // Set the maximum steps per second:
  myStepper.setSpeed(15);
}
void loop() {
  voltage_tasmota = analogRead(analogPin) * (3.3 / 1023.0);
  Serial.println(voltage_tasmota); 
	if ( voltage_tasmota > 1) {
	   Serial.println("Tasmota...................."); 
     while ( voltage_tasmota > 1)  {
         Serial.println("avvio il motore stepper");
         myStepper.step(50);
         delay(1);
         voltage_tasmota= analogRead(analogPin) * (3.3 / 1023.0);
         Serial.println(voltage_tasmota); 
     }
     Serial.println("Spengo il motore stepper");
	} 
     else
     {
      Serial.println("Aspetto................");
      delay(10);
     }
  } 
   



 
