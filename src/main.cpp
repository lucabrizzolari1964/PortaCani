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
int numerostepmotoreup=0;
int numerostepmotoredown=0;
int ledVol = 2;
int ledUltr = 3;
int TasmotaAnalogPin = A3;
int sensore_volumetrico = 6;
int sensore_porta_up_on = 7;
int sensore_porta_down_on = 12;
int bottone = 4;
int SecondiAperturaPorta=0;
int SecondiOra=0;
int DiffSecondi=0;
int SecondiDiAperturaDef=10;
int velocitaStepper=1;
boolean stopApertura=0;
int stopAperturaButton=0; //bottone di stopApertura 
float voltage_tasmota=0;
const int trigPin = A0; 
const int echoPin = A1;
const int TempoChiusuraPortaPin=A2; 
long durata;
float distanza;
boolean portaAperta=false;
boolean stopChusura=false; //se true deve aprire immediatamente metre chiude
boolean comandoesternomanuale=false;
boolean presenzainterna=false;
boolean presenzaesterna=false;
boolean ledvolAcceso=false;
boolean ledultrAcceso=false;
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
    if (ledvolAcceso==false)
    {
      digitalWrite(ledVol,HIGH);
      ledvolAcceso=true;
      Serial.println("Accendo led Volumetrico");
    }
}

void SpegniLedVol(boolean fai){
    if (ledvolAcceso==true)
    {
      digitalWrite(ledVol,LOW);
      ledvolAcceso=false;
      Serial.println("Spengo led Volumetrico");
    }
}

void AccendiLedUltr(boolean fai){
    if (ledultrAcceso==false)
    {
      digitalWrite(ledUltr,HIGH);
      ledultrAcceso=true;
      Serial.println("Accendo led Ultrasuoni");
    }
}

void SpegniLedUltr(boolean fai){
    if (ledultrAcceso==true)
    {
      digitalWrite(ledUltr,LOW);
      ledultrAcceso=false;
      Serial.println("Spengo led Ultrasuoni");
    }
    
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
      //Serial.println("Nessuna Presenza Interna"); 
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
  //
  durata = pulseIn(echoPin, HIGH); // in microsecondi
  // velocità del suono = 343 m/s o 0.0343 cm/µs
  // Distance = (duration * speed of sound) / 2 (because the pulse travels there and back)
  distanza = durata * 0.0343 / 2;

  // Print the distance to the serial monitor
  //Serial.print("Distanza: ");
  //Serial.print(distanza);
  //Serial.println(" cm");
  if (distanza < 20)
     {
      presente = true;
      Serial.println("Presenza Esterna ....");
      AccendiLedUltr(true);
     }
     else
     {
      presente = false;
      //Serial.println("Nessuna Presenza Esterna");
      SpegniLedUltr(true);
     }
  return presente;
} 

boolean ComandoEsternoManuale()
//quando via domotica viene dato il comando di acceso manuale
{
  boolean ritorno=false;
  voltage_tasmota = analogRead(TasmotaAnalogPin) * (3.3 / 1023.0);
  //Serial.print("Voltaggio da esp01 "); 
  //Serial.println(voltage_tasmota); 
	if ( voltage_tasmota > 1) {
     Serial.print("Voltaggio da esp01 "); 
     Serial.println(voltage_tasmota); 
	   Serial.println("Arrivato segnale di comando manuale porta chiusa");
     ritorno=true;
  } 
  return ritorno;
}
void AproPorta()
{
         Serial.println("Apro la porta ");
         ScriviLcd("Apro la porta", "");
         numerostepmotoreup=0;
         SecondiAperturaPorta=seconds();
         velocitaStepper=1;
         myStepper.setSpeed(1);
         while  (fsensore_porta_up_on()==false) 
            {
              myStepper.step(-50);
              numerostepmotoreup=numerostepmotoreup+1;
              if (velocitaStepper < 15)
              {
                velocitaStepper=velocitaStepper+1;
                myStepper.setSpeed(velocitaStepper);
              }
            }
         ScriviLcd("Porta Aperta","per "+String(SecondiDiAperturaDef)+" sec.");
         portaAperta=true;
         SecondiAperturaPorta=seconds();
         digitalWrite(motorPin1,LOW);
         digitalWrite(motorPin2,LOW);
         digitalWrite(motorPin3,LOW);
         digitalWrite(motorPin4,LOW);
         Serial.print("Apertura porta completata Numero passi motore=");
         Serial.println(numerostepmotoreup);
}

void ChiudoPorta()
{
         Serial.println("Chiudo la porta ");
         ScriviLcd("Chiudo la porta....","");
         numerostepmotoredown=0;
         velocitaStepper=1;
         myStepper.setSpeed(velocitaStepper);
         stopChusura=false;
         while ( (fsensore_porta_down_on()==false) and (stopChusura ==false) ) 
            {
              myStepper.step(100);
              numerostepmotoredown=numerostepmotoredown+1;
              if (velocitaStepper < 15)
              {
                velocitaStepper=velocitaStepper+1;
                myStepper.setSpeed(velocitaStepper);
              }
              //controllo che il cane non si ripresenti vicino
              if ( (PresenzaEsterna() == true) or (PresenzaInterna() ==true) )
               {
                Serial.print("Chiusura porta completata Numero passi motore=");
                Serial.println(numerostepmotoredown);
                AproPorta();
                Serial.print("Apertura porta completata Numero passi motore=");
                Serial.println(numerostepmotoreup);
                //esco dal loop sensore down on
                stopChusura=true;
               }
            }
         ScriviLcd("Porta Chiusa","");
         portaAperta=false;
         digitalWrite(motorPin1,LOW);
         digitalWrite(motorPin2,LOW);
         digitalWrite(motorPin3,LOW);
         digitalWrite(motorPin4,LOW);
         

}


void ChiudoPortaManualmente()
{
         Serial.println("Chiudo la porta manualmente");
         ScriviLcd("Chiudo la porta....","Manualmente");
         myStepper.setSpeed(15);
         stopChusura=false;
         while ( (fsensore_porta_down_on()==false) and (stopChusura ==false) ) 
            {
              myStepper.step(100);
            }
         Serial.println("Chiusura porta completata");
         ScriviLcd("Porta Chiusa","Manualmente");
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
  pinMode(trigPin, OUTPUT); // Trigger pin 
  pinMode(echoPin, INPUT);  // Echo pin 
  pinMode(ledVol, OUTPUT);
  pinMode(ledUltr, OUTPUT);
  MyLCD.init();
  MyLCD.backlight();
  ScriviLcd("Porticina Panna","Regola il tempo");
  delay(5000);
  int val=30;
  int fatto=0;
  int fattotimeout=0;
  boolean fine=false;
  while (!fine)
  {
     val=analogRead(TempoChiusuraPortaPin);
     SecondiDiAperturaDef=map(val, 0, 1023, 0, 300);
     Serial.print("Tempo definito di chiusura porta =");
     Serial.println(SecondiDiAperturaDef);
     ScriviLcd("Se ok premi","Tempo = "+String(SecondiDiAperturaDef));
     fattotimeout=fattotimeout+1;
     fatto=digitalRead(bottone);
     if (( fatto == HIGH) or (fattotimeout > 20) )
     {
      fine=true;
     }
     delay(100);
  }
  if (fsensore_porta_up_on() == true)
  {
    portaAperta=true;
  }
  else
    {
      portaAperta=false;
      AproPorta();
    }
   ScriviLcd("Porta Panna","sec chiusura:"+String(SecondiDiAperturaDef));
   delay(1000);
}
void loop() {
  comandoesternomanuale=ComandoEsternoManuale();
  presenzainterna=PresenzaInterna();
  presenzaesterna=PresenzaEsterna();
  stopAperturaButton=digitalRead(bottone);
  
  //test se modalità automatica o manuale tramite domotica 
	if ( (comandoesternomanuale == true) or (stopAperturaButton == HIGH) )
     {
      if (portaAperta == true)
        {
         ChiudoPortaManualmente();
         stopApertura=true;
        }
        else
        {
          Serial.println("Porta gia' Chiusa ");
          if (stopAperturaButton == HIGH)
          {
            AproPorta();
            stopApertura=false;
          }
        }
	   }
   if ( (comandoesternomanuale == false) or (stopApertura == false) )
      {
        stopApertura=false;
      }
  
  if ( (portaAperta == true) and (comandoesternomanuale != true) )
  {
    val=analogRead(TempoChiusuraPortaPin);
    SecondiDiAperturaDef=map(val, 0, 1023, 0, 300);
    Serial.print("Tempo definito di chiusura porta =");
    Serial.println(SecondiDiAperturaDef);
    SecondiOra=seconds();
    Serial.print("Porta aperta");
    DiffSecondi=SecondiAperturaPorta - SecondiOra;
    Serial.print(abs(DiffSecondi));
    Serial.println(" sec");
    ScriviLcd("Porta Aperta ","alla chiusura "+String(SecondiDiAperturaDef - abs(DiffSecondi))+" sec");
    if (abs(DiffSecondi) > SecondiDiAperturaDef)
      {
        //devo chiudere la porta
        ChiudoPorta();
        portaAperta=false;
        ScriviLcd("Porta Chiusa","");
      }
  }

  if ( (presenzainterna == true) and (comandoesternomanuale != true) and (stopApertura == false))
     {
      if (portaAperta == false)
        {
          //devo aprire la porta
          AproPorta();
        }
     }

  if ( (presenzaesterna == true) and (comandoesternomanuale != true) and (stopApertura == false))
      {
      if (portaAperta == false)
        {
          //devo aprire la porta
          AccendiLedUltr(true);
          AproPorta();
        }
      }
  //loop principale e wait  
  Serial.println("Aspetto.....................................................................................");
  delay(10);

} 




 
