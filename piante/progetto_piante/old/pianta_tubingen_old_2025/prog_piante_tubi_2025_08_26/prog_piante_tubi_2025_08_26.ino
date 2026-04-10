//#include <RTClib.h>

//RTC_DS3231 rtc;
//char *res = malloc(5);
//String pad(int n);
int pin_ventola_raffreddamento = 5;
int pin_controllo_luce = 6;

void setup() {
  Serial.begin(9600);

  Serial.println("avvio!");

  //////////////////////////////////////////CONTROLLO PER L'OROLOGIO

  //if (!rtc.begin()) {
  //  Serial.println("Controlla le connessioni");
  //  while(true);
  //}
  /*
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  if (rtc.lostPower()) {
    Serial.println("Imposto data/ora");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));    
    //rtc.adjust(DateTime(2020, 06, 3, 0, 37, 0));
  }
  */
  /////////////////////////////////////////////

  pinMode(pin_ventola_raffreddamento,OUTPUT); //ventola raffreddamento arduino
  pinMode(pin_controllo_luce,OUTPUT); //rele LUCE

  delay(2000);
}

void loop() {
  //Serial.println("--");
  while(!Serial.available()){}

  String new_line = Serial.readString();
  delay(1000);

  /////////////////////////////REGOLAZIONE LUCE

  if(new_line=="ON\n"){
    digitalWrite(pin_controllo_luce, HIGH);
    Serial.println("luce accesa");
  }else{
    digitalWrite(pin_controllo_luce, LOW);
    Serial.println("luce spenta");
  }

  while(!Serial){
    Serial.begin(9600);
    delay(2000);
  }
  /////////////////////////////////////////////TEMPO

  delay(1500);
}

//String pad(int n) {
// sprintf(res, "%02d", n);
//  return String(res);  
//}
