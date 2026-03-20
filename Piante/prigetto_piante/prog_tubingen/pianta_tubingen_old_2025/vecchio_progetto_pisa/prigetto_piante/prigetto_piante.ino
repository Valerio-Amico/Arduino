#include <SimpleDHT.h>  //      for DHT11, VCC: 5V or 3V, GND: GND, DATA: 2
#include <Wire.h> 
#include <OneWire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <OneWire.h> 
#include <DallasTemperature.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo myservo;

#define ONE_WIRE_BUS 10
#define DHTPIN 2
SimpleDHT11 dht11;

OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);

byte temp;
byte umid;
double Ta;
int sinistra = 140;
int centro = 100;
int destra = 60;
int levelSERB;
int levelPLANT=0;
long int t0l, t0wd, t0wc, t0ws, t, tACCl, tSPEl, tACCws, tSPEws, tACCwd, tSPEwd, tACCwc, tSPEwc;
long durata;
int altezza=10;
bool lux, wats, watd, watc, hl;

//int convert(int x);

void setup() {
  Serial.begin(9600);

  sensors.begin(); 
  
  myservo.attach(9);

  pinMode(12, OUTPUT);
  pinMode(13, INPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(3,OUTPUT);
  
  lcd.begin();
  lcd.backlight();
  lcd.print("Avvio!");

  lux=true;
  hl=true;
  tACCl=57600000;//100;//
  tSPEl=28800000;//100000;//

  wats=false; //La pompetta eroga 100ml in 6s.
  tSPEws=3000000; // esperimento, eroga 900ms e spenta 5000ms, inizio 19:25, fine 19:31. Riempie 100ml.
  tACCws=500;

  watc=false;
  tSPEwc=1000000; // esperimento, eroga 900ms e spenta 5000ms, inizio 19:25, fine 19:31. Riempie 100ml.
  tACCwc=500;

  watd=false;
  tSPEwd=4000000; // esperimento, eroga 900ms e spenta 5000ms, inizio 19:25, fine 19:31. Riempie 100ml.
  tACCwd=400;
  
  delay(2000);
}

void loop() {

 /*
    for (pos = 0; pos <= 20; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(100);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 20; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(100);                       // waits 15ms for the servo to reach the position
  }
  */
  /////////////////////////////REGOLAZIONE LUCE E ACQUA

  t=millis();
  if(lux==true){
    Serial.println("Luce accesa!!!");
    if(hl==true){
      digitalWrite(6, LOW);
      t0l=millis();
      hl=false;
    }else if(t-t0l >= tACCl){
      lux=false;
      hl=true;
    }
  }else{
    Serial.println("Luce spenta!!!");
    if(hl==true){
        digitalWrite(6, HIGH);
        t0l=millis();
        hl=false;
        }else if(t-t0l >= tSPEl){
          lux=true;
          hl=true;
    }
  }

  //////////////////////////////////////////Innaffio sinistra
  
  if(wats==true){
      digitalWrite(8, HIGH);
      delay(1000);
      myservo.write(sinistra);
      delay(1000);
      digitalWrite(8, LOW);
      digitalWrite(7, HIGH);
      delay(tACCws);
      digitalWrite(7, LOW);
      t0ws=millis();
      wats=false;
  }else if(t-t0ws >= tSPEws){
      wats=true;
  }

  //////////////////////////////////////////Innaffio centro
  
  if(watc==true){
      digitalWrite(8, HIGH);
      delay(1000);
      myservo.write(centro);
      delay(1000);
      digitalWrite(8, LOW);
      digitalWrite(7, HIGH);
      delay(tACCwc);
      digitalWrite(7, LOW);
      t0wc=millis();
      watc=false;
  }else if(t-t0wc >= tSPEwc){
      watc=true;
  }

  ////////////////////////////////////////////Innaffio destra

  if(watd==true){
      digitalWrite(8, HIGH);
      delay(1000);
      myservo.write(destra);
      delay(1000);
      digitalWrite(8, LOW);
      digitalWrite(7, HIGH);
      delay(tACCwd);
      digitalWrite(7, LOW);
      t0wd=millis();
      watd=false;
  }else if(t-t0wd >= tSPEwd){
      watd=true;
  }
  
  /////////////////////////////SENSORI
  
  Serial.println("=================================");
  Serial.println("Dati: ");
  
  temp = 0;
  umid = 0;
  levelSERB = 0;
  levelPLANT = 0;

  /////////////////////////////////////TEMPERATURA E UMIDITA

  //////////temperatura microcontrollore

  sensors.requestTemperatures();
  Ta=sensors.getTempCByIndex(0);
  Serial.print("Temperatura Microcontrollore = ");
  Serial.println(Ta);
  
  lcd.clear();
  
  //////////temperatura pianta
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(DHTPIN, &temp, &umid, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); 
    Serial.println(err);
    lcd.setCursor(0,0);
    lcd.print("Error!");
    delay(1000);
    return;
  }

  lcd.setCursor(5,0);
  lcd.print("A=");
  lcd.print((int)Ta);

  Serial.print("Sample OK: ");
  Serial.print((int)temp); Serial.print(" *C, "); 

  lcd.setCursor(0,0);
  lcd.print("T=");
  lcd.print((int)temp);
  //lcd.print("C");
  
  Serial.print((int)umid); Serial.print(" H, ");

  lcd.setCursor(10,0);
  lcd.print("U=");
  lcd.print((int)umid);
  lcd.print("RH");

  if(Ta >= 28){
    digitalWrite(5,HIGH);
    lcd.setCursor(14,1);
    lcd.print("Vn");
  }else{
    digitalWrite(5,LOW);
    lcd.setCursor(14,1);
    lcd.print("Vf");
  }
  /////////////////////////////////LIVELLO ACQUA SERBATORIO

  digitalWrite(12,LOW);
  digitalWrite(12,HIGH);
  delayMicroseconds(10);
  digitalWrite(12,LOW);
 
  durata=pulseIn(13,HIGH);
  levelSERB=altezza-(0.034*durata/2);

  Serial.print(levelSERB); Serial.println(" cm");
  //Serial.print(levelPLANT); Serial.println(" cm");

  lcd.setCursor(0,1);
  lcd.print("S=");
  lcd.print((int)levelSERB);
  lcd.print("cm");

  if(levelSERB<=3){
    analogWrite(3,10);
  }else{
    digitalWrite(3,LOW);
  }
  /*
  lcd.setCursor(6,1);
  lcd.print("P=");
  lcd.print((int)levelPLANT);
  //lcd.print("mm");
  */
  
  
  delay(1500);
}
