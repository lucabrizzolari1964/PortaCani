#include <Stepper.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
// pin per motore stepper
#define motorPin1  8      // IN1 
#define motorPin2  9      // IN2 
#define motorPin3  10     // IN3 
#define motorPin4  11     // IN4 
#define seconds() (millis()/1000)
const int stepsPerRevolution = 2048;
int ledVol = 2;
int ledUltr = 3;
int TasmotaAnalogPin = A3;
int sensore_volumetrico = 6;
int sensore_porta_up_on = 7;
int sensore_porta_down_on = 12;
int bottoneFineRegolazione = 13;
int SecondiAperturaPorta=0;
int SecondiOra=0;
int DiffSecondi=0;
int SecondiDiAperturaDef=10;
float voltage_tasmota=0;
const int trigPin = A0; 
const int echoPin = A1;
const int TempoChiusuraPortaPin=A2; 
long durata;
float distanza;
boolean portaAperta=false;
boolean stopChusura=false;
//variabile di servizio
int val =0;
LiquidCrystal_I2C MyLCD(0x27, 16, 2);
Stepper myStepper(stepsPerRevolution, motorPin1, motorPin3, motorPin2, motorPin4);

void ScriviLcd(String stampa, String stampa1){
    MyLCD.clear();
    MyLCD.setCursor(0, 0);
    MyLCD.print(stampa);
    MyLCD.setCursor(0, 1);
    MyLCD.print(stampa1);

}
void AccendiLedVol(boolean fai){
    digitalWrite(ledVol,HIGH);
    Serial.println("Accendo led Volumetrico");
}

void SpegniLedVol(boolean fai){
    digitalWrite(ledVol,LOW);
    Serial.println("Spengo led Volumetrico");
}

void AccendiLedUltr(boolean fai){
    digitalWrite(ledUltr,HIGH);
    Serial.println("Accendo led Ultrasuoni");
}

void SpegniLedUltr(boolean fai){
    digitalWrite(ledUltr,LOW);
    Serial.println("Spengo led Ultrasuoni");
}
boolean fsensore_porta_up_on()
{
  boolean risultato=false;
  val = digitalRead(sensore_porta_up_on);
  if (val == HIGH) {
    risultato=true;
    Serial.println("up porta on......"); 
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
      AccendiLedVol(true);
  } 
  else
  {
      Serial.println("Nessuna Presenza Interna"); 
      SpegniLedVol(true);
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
      AccendiLedUltr(true);
     }
     else
     {
      presente = false;
      Serial.println("Nessuna Presenza Esterna");
      SpegniLedUltr(true);
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
         Serial.println("Apro la porta ");
         ScriviLcd("Apro la porta", "");
         SecondiAperturaPorta=seconds();
         myStepper.setSpeed(15);
         while (fsensore_porta_up_on()==false)
            {
              myStepper.step(300);
            }
         Serial.println("Apertura porta completata");
         ScriviLcd("Porta Aperta","");
         portaAperta=true;
         digitalWrite(motorPin1,LOW);
         digitalWrite(motorPin2,LOW);
         digitalWrite(motorPin3,LOW);
         digitalWrite(motorPin4,LOW);
}

void ChiudoPorta()
{
         Serial.println("Chiudo la porta ");
         ScriviLcd("Chiudo la porta....","");
         myStepper.setSpeed(15);
         stopChusura=false;
         while ( (fsensore_porta_down_on()==false) and (stopChusura ==false) ) 
            {
              myStepper.step(-300);
              //controllo che il cane non si ripresenti vicino
              if ( (PresenzaEsterna() == true) or (PresenzaInterna() ==true) )
               {
                AproPorta();
                //esco dal loop sensore down on
                stopChusura=true;
               }
            }
         Serial.println("Chiusura porta completata");
         ScriviLcd("Porta Chiusa","");
         portaAperta=false;
         digitalWrite(motorPin1,LOW);
         digitalWrite(motorPin2,LOW);
         digitalWrite(motorPin3,LOW);
         digitalWrite(motorPin4,LOW);
}


void setup() {
  Serial.begin(9600);
  pinMode(sensore_porta_up_on, INPUT);
  pinMode(sensore_volumetrico, INPUT); 
  pinMode(sensore_porta_down_on, INPUT_PULLUP);
  pinMode(trigPin, OUTPUT); // Trigger pin needs to be an output
  pinMode(echoPin, INPUT);  // Echo pin needs to be an input
  pinMode(ledVol, OUTPUT);
  pinMode(ledUltr, OUTPUT);
  MyLCD.init();
  MyLCD.backlight();
  ScriviLcd("Porticina Panna","Regola il tempo");
  delay(5000);
  int val=30;
  int fatto=0;
  boolean fine=false;
  while (!fine)
  {
     val=analogRead(TempoChiusuraPortaPin);
     
     SecondiDiAperturaDef=map(val, 0, 1023, 0, 300);
     Serial.print("Tempo definito di chiusura porta =");
     Serial.println(SecondiDiAperturaDef);
     ScriviLcd("Se ok push","Tempo = "+String(SecondiDiAperturaDef));
     fatto=digitalRead(bottoneFineRegolazione);
     if (fatto == HIGH)
     {
      fine=true;
     }
     delay(100);
  }
  delay(1000);
  ScriviLcd("Porta Panna","sec chiusura:"+String(SecondiDiAperturaDef));

}
void loop() {
  //test se modalità automatica o manuale tramite domotica 
  
	if ( ComandoEsternoManuale() == true)
     {
      if (portaAperta == false)
        {
         AproPorta();
        }
        else
        {
          Serial.println("Porta Aperta    ");
          ScriviLcd("Porta Aperta","");
        }
	   } 

  if ( (portaAperta == true) and (ComandoEsternoManuale() != true) )
  {
    val=analogRead(TempoChiusuraPortaPin);
    SecondiDiAperturaDef=map(val, 0, 1023, 0, 300);
    Serial.print("Tempo definito di chiusura porta =");
    Serial.println(SecondiDiAperturaDef);
    SecondiOra=seconds();
    Serial.print("Porta aperta da ");
    DiffSecondi=SecondiAperturaPorta - SecondiOra;
    Serial.print(abs(DiffSecondi));
    Serial.println(" sec");
    if (abs(DiffSecondi) > SecondiDiAperturaDef)
      {
        //devo chiudere la porta
        ChiudoPorta();
        portaAperta=false;
        ScriviLcd("Porta Chiusa","");
      }
  }

  if (PresenzaInterna() == true)
     {
      if (portaAperta == false)
        {
          //devo aprire la porta
          AproPorta();
        }
     }

  if (PresenzaEsterna() == true)
      {
      if (portaAperta == false)
        {
          //devo aprire la porta
          AccendiLedUltr(true);
          AproPorta();
        }
      }
  //loop principale e wait  
  Serial.println("Aspetto................");
  delay(100);

} 




 
