#include <Stepper.h>
#include <Arduino.h>
#include <LiquidCrystal.h>
// Motor pin definitions:
#define motorPin1  8      // IN1 on the ULN2003 driver
#define motorPin2  9      // IN2 on the ULN2003 driver
#define motorPin3  10     // IN3 on the ULN2003 driver
#define motorPin4  11     // IN4 on the ULN2003 driver
const int stepsPerRevolution = 2048;
int analogPin = A3;
float voltage_tasmota=0;
const int rs = 13, en = 12, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Ciao Panna");
}
void loop() {
  voltage_tasmota = analogRead(analogPin) * (3.3 / 1023.0);
  Serial.println(voltage_tasmota); 
	if ( voltage_tasmota > 1) {
	   Serial.println("Tasmota...................."); 
     while ( voltage_tasmota > 1)  {
         Serial.println(voltage_tasmota); 
         Serial.println("avvio il motore stepper");
         lcd.clear();
         lcd.print("Avvio il motore");
         Stepper myStepper(stepsPerRevolution, motorPin1, motorPin3, motorPin2, motorPin4);
         myStepper.setSpeed(15);
         myStepper.step(500);
         delay(10);
         voltage_tasmota= analogRead(analogPin) * (3.3 / 1023.0);
         Serial.println(voltage_tasmota); 
     }
     Serial.println("Spengo il motore stepper");
     lcd.clear();
     lcd.print("Spengo il motore");
     digitalWrite(motorPin1,LOW);
     digitalWrite(motorPin2,LOW);
     digitalWrite(motorPin3,LOW);
     digitalWrite(motorPin4,LOW);
    
	} 
     else
     {
      Serial.println("Aspetto................");
      delay(1000);
     }
  } 
   



 
