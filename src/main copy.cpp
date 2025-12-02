#include <Stepper.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
// pin per motore stepper
#define motorPin1  8      // IN1 
#define motorPin2  9      // IN2 
#define motorPin3  10     // IN3 
#define motorPin4  11     // IN4 
const int stepsPerRevolution = 2048;
int ledRossoVol = 5;
int ledBluUltr = 4;
int TasmotaAnalogPin = A3;
int sensore_volumetrico = 6;
int sensore_porta_up_on = 7;
int sensore_porta_down_on = 12;
float voltage_tasmota=0;
const int trigPin = A0; 
const int echoPin = A1; 
long durata;
float distanza;
//variabile di servizio
int val =0;
LiquidCrystal_I2C MyLCD(0x27, 16, 2);

void AccendiLedRossoVol(boolean fai){
    digitalWrite(ledRossoVol,HIGH);
    Serial.println("Accendo led rosso");
}

void SpegniLedRossoVol(boolean fai){
    digitalWrite(ledRossoVol,LOW);
    Serial.println("Spengo led rosso");
}

void AccendiLedBluUltr(boolean fai){
    digitalWrite(ledBluUltr,HIGH);
    Serial.println("Accendo led blu");
}

void SpegniLedBluUltr(boolean fai){
    digitalWrite(ledBluUltr,LOW);
    Serial.println("Spengo led blu");
}
boolean fsensore_porta_up_on()
{
  boolean risultato=false;
  val = digitalRead(sensore_porta_up_on);
  if (val == HIGH) {
    risultato=true;
    Serial.println("up porta on"); 
    MyLCD.setCursor(0, 0);
    MyLCD.print("Porta Aperta");
  }
  return risultato;
}


boolean fsensore_porta_down_on()
{
  boolean risultato=false;
  val = digitalRead(sensore_porta_down_on);
  if (val == HIGH) {
    risultato=true;
    Serial.println("down porta on"); 
    MyLCD.setCursor(0, 0);
    MyLCD.print("Porta Chiusa");
  }
  return risultato;
}

boolean PresenzaInterna()
{
  boolean risultato=false;
  val = digitalRead(sensore_volumetrico);
  if (val == HIGH) {
      Serial.println("Presenza Interna ......."); 
      risultato=true;
  } 
  if (val == LOW) {
      Serial.println("Nessuna Presenza Interna"); 
      risultato=false;
  } 
  return risultato;
}

boolean PresenzaEsterna()
{
  boolean presente=false;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  durata = pulseIn(echoPin, HIGH); // in microsecondi
  // velocità del suono = 343 m/s o 0.0343 cm/µs
  // Distance = (duration * speed of sound) / 2 (because the pulse travels there and back)
  distanza = durata * 0.0343 / 2;

  // Print the distance to the serial monitor
  Serial.print("Distanza: ");
  Serial.print(distanza);
  Serial.println(" cm");
  if (distanza < 20)
     {
      presente = true;
      Serial.println("Presenza Esterna ....");
     }
     else
     {
      presente = false;
      Serial.println("Nessuna Presenza Esterna");
     }
  return presente;
} 
void setup() {
  Serial.begin(9600);
  pinMode(sensore_porta_up_on, INPUT);
  pinMode(sensore_volumetrico, INPUT); 
  pinMode(sensore_porta_down_on, INPUT_PULLUP);
  pinMode(trigPin, OUTPUT); // Trigger pin needs to be an output
  pinMode(echoPin, INPUT);  // Echo pin needs to be an input
  pinMode(ledRossoVol, OUTPUT);
  MyLCD.init();
  MyLCD.backlight();
  MyLCD.setCursor(0, 0);

}
void loop() {
  if (fsensore_porta_up_on() == true)
     {
      Serial.println("Raggiunto fine corsa porta up..........................................................."); 
     }

  if (fsensore_porta_down_on() == true)
     {
      Serial.println("Raggiunto fine corsa porta down..........................................................."); 
     }

  if (PresenzaInterna() == true)
     {
      AccendiLedRossoVol(true);
     }
     else
     {
      SpegniLedRossoVol(true);

     }
  if (PresenzaEsterna() == true)
     {
      AccendiLedBluUltr(true);
     }
     else
     {
      SpegniLedBluUltr(true);
     }
  
  voltage_tasmota = analogRead(TasmotaAnalogPin) * (3.3 / 1023.0);
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
         voltage_tasmota= analogRead(TasmotaAnalogPin) * (3.3 / 1023.0);
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




 
