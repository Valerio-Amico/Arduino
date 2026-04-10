#include <RTClib.h>
#include <Wire.h> 
#include <OneWire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <OneWire.h> 
#include <DallasTemperature.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo myservo;
RTC_DS3231 rtc;
char *res = malloc(5);
String pad(int n);

#define ONE_WIRE_BUS 10  // pin digitale sensore temp Dallas

OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);

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


void setup() {
  Serial.begin(9600);

  lcd.begin();
  lcd.backlight();
  lcd.print("Avvio!");

  //////////////////////////////////////////CONTROLLO PER L'OROLOGIO

  if (!rtc.begin()) {
    Serial.println("Controlla le connessioni");
    while(true);
  }
  /*
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  if (rtc.lostPower()) {
    Serial.println("Imposto data/ora");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));    
    //rtc.adjust(DateTime(2020, 06, 3, 0, 37, 0));
  }
  */
  /////////////////////////////////////////////

  sensors.begin(); 
  
  myservo.attach(9);

  pinMode(12, OUTPUT);
  pinMode(13, INPUT);
  pinMode(5,OUTPUT); //ventola raffreddamento arduino
  pinMode(6,OUTPUT); //rele LUCE
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(3,OUTPUT);

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

  DateTime now = rtc.now();

  /////////////////////////////REGOLAZIONE LUCE E ACQUA
  if(now.hour()>=21 && now.hour()<=23 || now.hour()>=0 && now.hour()<9){
    digitalWrite(6, HIGH);
  }else{
    digitalWrite(6, LOW);
  }
  
  /*
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
  */
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

  /*
  temp = 0;
  umid = 0;
  levelSERB = 0;
  levelPLANT = 0;
  */

  /////////////////////////////////////TEMPERATURA E UMIDITA

  //////////temperatura microcontrollore

  sensors.requestTemperatures();
  Ta=sensors.getTempCByIndex(0);
  Serial.print("Temperatura Microcontrollore = ");
  Serial.println(Ta);
  
  lcd.clear();

  /////////////////////////////////////////////TEMPO
  
  Serial.print(pad(now.hour()));
  Serial.print(':');
  Serial.print(pad(now.minute()));
  Serial.print(':');
  Serial.print(pad(now.second()));
  Serial.println();

  lcd.setCursor(0,0);
  lcd.print(pad(now.hour()));
  lcd.print(':');
  lcd.print(pad(now.minute()));
  lcd.print(':');
  lcd.print(pad(now.second()));
  
  ///////////////////////////////temperatura pianta
  
  lcd.setCursor(0,1);
  lcd.print("A=");
  lcd.print((int)Ta); 

  if(Ta >= 21){
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

  lcd.setCursor(0,1);
  lcd.print("S=");
  lcd.print((int)levelSERB);
  lcd.print("cm");

  /*
  if(levelSERB<=3){
    analogWrite(3,10);
  }else{
    digitalWrite(3,LOW);
  }
  */
  
  delay(1500);
}

String pad(int n) {
  sprintf(res, "%02d", n);
  return String(res);  
}
