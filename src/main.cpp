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
boolean portaAperta=false;
//variabile di servizio
int val =0;
LiquidCrystal_I2C MyLCD(0x27, 16, 2);
Stepper myStepper(stepsPerRevolution, motorPin1, motorPin3, motorPin2, motorPin4);

void ScriviLcd(String stampa){
    MyLCD.setCursor(0, 0);
    MyLCD.print(stampa);
}
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
    Serial.println("up porta on......"); 
    ScriviLcd("Porta up on......");
  }
  return risultato;
}


boolean fsensore_porta_down_on()
{
  boolean risultato=false;
  val = digitalRead(sensore_porta_down_on);
  if (val == HIGH) {
    risultato=true;
    Serial.println("down porta on..."); 
    ScriviLcd("Porta down on...");
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

boolean ComandoEsternoManuale()
//quando via domotica viene dato il comando di acceso manuale
{
  boolean ritorno=false;
  voltage_tasmota = analogRead(TasmotaAnalogPin) * (3.3 / 1023.0);
  Serial.print("Voltaggio da esp01 "); 
  Serial.println(voltage_tasmota); 
	if ( voltage_tasmota > 1) {
	   Serial.println("Arrivato segnale di comando manuale porta aperta");
     ritorno=true;
  } 
  return ritorno;
}
void AproPorta()
{
         Serial.println("Apro la porta in modalità manuale");
         MyLCD.setCursor(0, 0);
         MyLCD.print("Apro la porta....");
         myStepper.setSpeed(15);
         while (fsensore_porta_up_on()==false)
            {
              myStepper.step(300);
            }
         Serial.println("Apertura porta completata");
         MyLCD.clear();
         MyLCD.setCursor(0, 0);
         MyLCD.print("Fine Apertura   ");
         MyLCD.setCursor(0, 1);
         MyLCD.print("Porta           ");
         portaAperta=true;
         digitalWrite(motorPin1,LOW);
         digitalWrite(motorPin2,LOW);
         digitalWrite(motorPin3,LOW);
         digitalWrite(motorPin4,LOW);
         delay(5000);
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
  //1 test se modalità automatica o manuale tramite domotica 
  
	if ( ComandoEsternoManuale() == true)
     {
      if (portaAperta == false)
        {
         AproPorta();
        }
        else
        {
          Serial.println("Porta Aperta    ");
          MyLCD.setCursor(0, 0);
          MyLCD.clear();
          MyLCD.print("Porta Aperta    ");
        }
	   } 

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
      if (portaAperta == false)
        {
          //devo aprire la porta
          AproPorta();
          AccendiLedRossoVol(true);
        }
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
    
  Serial.println("Aspetto................");
  delay(1000);

} 




 
