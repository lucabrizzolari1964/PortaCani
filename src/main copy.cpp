#include <Stepper.h>
#include <Arduino.h>
//#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
// Motor pin definitions:
#define motorPin1  8      // IN1 on the ULN2003 driver
#define motorPin2  9      // IN2 on the ULN2003 driver
#define motorPin3  10     // IN3 on the ULN2003 driver
#define motorPin4  11     // IN4 on the ULN2003 driver
const int stepsPerRevolution = 2048;
int analogPin = A3;
int sensore_volumetrico = 6;
int sensore_porta_down = 7;
int sensore_porta_down_2 = 12;
int val =0;
float voltage_tasmota=0;
const int trigPin = A0; 
const int echoPin = A1; 
long duration;
float distance;
LiquidCrystal_I2C MyLCD(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  pinMode(sensore_porta_down, INPUT);
  pinMode(sensore_volumetrico, INPUT); 
  pinMode(sensore_porta_down_2, INPUT_PULLUP);
  pinMode(trigPin, OUTPUT); // Trigger pin needs to be an output
  pinMode(echoPin, INPUT);  // Echo pin needs to be an input
  Serial.println("Ultrasonic Sensor Ready");

  MyLCD.init();
  MyLCD.backlight();
  MyLCD.setCursor(0, 0);
  MyLCD.print("Prova Seriale:");

}
void loop() {
  Serial.println("Loop..........."); 
  val = digitalRead(sensore_porta_down);

  if (val == HIGH) {
      Serial.println("Porta Aperta"); 
      MyLCD.setCursor(0, 0);
      MyLCD.print("Porta up aperta");

  } 
  val = digitalRead(sensore_porta_down_2);

  if (val == HIGH) {
      Serial.println("Porta 2 Aperta"); 
      MyLCD.setCursor(0, 0);
      MyLCD.print("Porta down aperta");

  } 
  val = digitalRead(sensore_volumetrico);
  if (val == HIGH) {
      Serial.println("Apri porta volumetrico attivo"); 
  } 
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the time it takes for the echo pulse to return
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in centimeters
  // Speed of sound = 343 m/s or 0.0343 cm/µs
  // Distance = (duration * speed of sound) / 2 (because the pulse travels there and back)
  distance = duration * 0.0343 / 2;

  // Print the distance to the serial monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  voltage_tasmota = analogRead(analogPin) * (3.3 / 1023.0);
  Serial.println(voltage_tasmota); 
	if ( voltage_tasmota > 1) {
	   Serial.println("Tasmota...................."); 
     while ( voltage_tasmota > 1)  {
         Serial.println(voltage_tasmota); 
         Serial.println("avvio il motore stepper");
         MyLCD.setCursor(0, 0);
         MyLCD.print("Avvio il motore.....");
         Stepper myStepper(stepsPerRevolution, motorPin1, motorPin3, motorPin2, motorPin4);
         myStepper.setSpeed(15);
         myStepper.step(300);
         delay(10);
         voltage_tasmota= analogRead(analogPin) * (3.3 / 1023.0);
         Serial.println(voltage_tasmota); 
     }
     Serial.println("Spengo il motore stepper");
     MyLCD.setCursor(0, 0);
     MyLCD.print("Spengo il motore.....");
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
   



 
