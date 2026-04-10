#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include <OneWire.h> 
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 9 // pin digitale sensore temp Dallas
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

RTC_DS3231 rtc;   //per orologio
char *res = malloc(5);
String pad(int n);

OneWire oneWire(ONE_WIRE_BUS); //temperatura arduino
DallasTemperature sensors(&oneWire);

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

double Ta;
const int ADDR =0x40;
int i,j,N=5;
long int X0,X1,Y0,Y1,Y2,Y3;
long durata;
double X,Y,X_out,Y_out1,Y_out2;
long int t0l, t0w, t, tACCl, tSPEl, tACCw, tSPEw;
bool wat;
int levelSERB;
int altezza=10;

void setup() {
  Serial.begin(9600);

  ///////////////PER IL DISPLAY
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  // Clear the buffer
  display.clearDisplay();  

  ///////////////PER L'OROLOGIO

  if (!rtc.begin()) {
    Serial.println("Controlla le connessioni");
    while(true);
  }

  ///////////////////PER IL SENSORE UMIDITA'
  
  Wire.begin();                                   
  delay(100);  
  Wire.beginTransmission(ADDR);
  Wire.endTransmission();  
  i=0;

  sensors.begin(); ////////TEMPERATURA ARDUINO

  pinMode(12, OUTPUT); //impulso sensore di prossimità
  pinMode(13, INPUT); //acquisizione sensore di prossimità
  pinMode(5,OUTPUT); //ventola raffreddamento arduino
  pinMode(8,OUTPUT); //rele LUCE
  pinMode(7,OUTPUT); //rele POMPETTA
  //pinMode(3,OUTPUT); //allarme acqua

  digitalWrite(5,HIGH); //ventola sempre accesa
  
  wat=false;
  tSPEw=240000;//1000000; // esperimento, eroga 900ms e spenta 5000ms, inizio 19:25, fine 19:31. Riempie 100ml.
  tACCw=1100;//500;
}

void loop() {
  
  DateTime now = rtc.now();

  
  /////////////////////////TEMPERATURA E UMIDITA' PIANTA, X e Y
  /**Send command of initiating temperature measurement**/
  Wire.beginTransmission(ADDR);
  Wire.write(0xE3);
  Wire.endTransmission();
 
  Serial.print("Temp");
  Serial.print("\t");
  Serial.println("Humidity");
 
  /**Read data of temperature**/
  Wire.requestFrom(ADDR,2);
 
  if(Wire.available()<=2);
  {
    X0 = Wire.read();
    X1 = Wire.read();
    X0 = X0<<8;
    X_out = X0+X1;
  }
 
  /**Calculate and display temperature**/
  X=(175.72*X_out)/65536;                        
  X=X-46.85;
  Serial.print(X);
  Serial.print("C");
  Serial.print("\t");
 
  /**Send command of initiating relative humidity measurement**/
  Wire.beginTransmission(ADDR);                     
  Wire.write(0xE5);
  Wire.endTransmission(); 
 
  /**Read data of relative humidity**/
  Wire.requestFrom(ADDR,2);
  if(Wire.available()<=2);
  {
    Y0 = Wire.read();
    Y2=Y0/100; 
    Y0=Y0%100;
    Y1 = Wire.read();
    Y_out1 = Y2*25600;
    Y_out2 = Y0*256+Y1;
  }
 
  /**Calculate and display relative humidity**/
  Y_out1 = (125*Y_out1)/65536;                     
  Y_out2 = (125*Y_out2)/65536;
  Y = Y_out1+Y_out2;
  Y=Y-6;
  Serial.print(Y);
  Serial.println("%");                     
  delay(100);

  //////////////////////////TEMPERATURA MICROCONTROLLORE

  sensors.requestTemperatures();
  Ta=sensors.getTempCByIndex(0);
  Serial.print("Temperatura Microcontrollore = ");
  Serial.println(Ta);

  if(Ta >= 21){
    digitalWrite(5,HIGH);
  }else{
    digitalWrite(5,LOW);
  }
  
  /////////////////////////////////LIVELLO ACQUA

  digitalWrite(12,LOW);
  digitalWrite(12,HIGH);
  delayMicroseconds(10);
  digitalWrite(12,LOW);
 
  durata=pulseIn(13,HIGH);
  levelSERB=altezza-(0.034*durata/2);

  Serial.print(levelSERB); Serial.println(" cm");
  
  /////////////////////////////////////////////////////////////////////////////////////////////


  /////////////////////////////REGOLAZIONE LUCE E ACQUA
  if(now.hour()>=21 && now.hour()<=23 || now.hour()>=0 && now.hour()<9){
    digitalWrite(6, HIGH);
  }else{
    digitalWrite(6, LOW);
  }

  t=millis();
  
  if(Y<85){
  if(wat==true){
      digitalWrite(7, HIGH);
      delay(tACCw);
      digitalWrite(7, LOW);
      t0w=millis();
      wat=false;
  }else if(t-t0w >= tSPEw){
      wat=true;
  }
  }
  
  //////////////////////////////DISPLAY

  display.clearDisplay();  
  display.setTextSize(1.6);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.print(X);
  display.setCursor(0, 16);
  display.cp437(true); 
  display.print(Y);
  display.setCursor(50, 0);
  display.cp437(true); 
  display.print(Ta);
  display.setCursor(50, 16);
  display.print(pad(now.hour()));
  display.print(":");
  display.print(pad(now.minute()));
  display.print(":");
  display.print(pad(now.second()));
  display.display();
  
 
  Serial.println(); 
  delay(1200);
  i++;
}

String pad(int n) {
  sprintf(res, "%02d", n);
  return String(res);  
}
