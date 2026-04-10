//  Corresponding Arduino: 
//  arduinodriver
//  SN: 75135333636351013221
//   Port: /dev/ttyACM3

//  Pin definition for the output
const int n_digital_io = 14;
int digital_io[n_digital_io];
int analog_inputs[6] = {A0, A1, A2, A3, A4, A5};
int analog_output[6] = {A0, A1, A2, A3, A4, A5};

// utility variables
String received_message, pin_name_string, value_string;
int separator_index, separator_index_l, pin_id, trig_pin, echo_pin, value, analog_read_temp;
// String check_message(String pin_name_, String value_str_);

void setDigitalIo(int _pin, int io){
  // io can be 0 or 1. 0 is OUTPUT, 1 is INPUT
  if(digital_io[_pin] != io){
    if(io == 0){
      pinMode(_pin, OUTPUT);
      digital_io[_pin] = 0;
    }else{
      pinMode(_pin, INPUT);
      digital_io[_pin] = 1;
    }
  }
}

float measureDistance(int _trig_pin, int _echo_pin) {
  // Invia impulso TRIG di 10µs
  digitalWrite(_trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(_trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(_trig_pin, LOW);

  // Legge la durata del segnale ECHO
  long _duration = pulseIn(_echo_pin, HIGH, 30000); // timeout 30ms

  // Timeout → nessun oggetto rilevato
  if (_duration == 0) return -1;

  // Calcola la distanza: velocità del suono ≈ 343 m/s = 0.0343 cm/µs
  // Diviso 2 perché il suono fa andata + ritorno
  return (_duration * 0.0343) / 2.0;
}

void setup() {
  for(int i=0; i<n_digital_io; i++){  
    digital_io[i] = 0; 
    pinMode(digital_io[i], OUTPUT);
  }
  Serial.begin(9600);
}

void loop() {
  // wait for data:
  while (Serial.available() == 0) {}
  // read the incoming byte:
  received_message = Serial.readStringUntil('\n');

  separator_index = received_message.indexOf(":");
  if(separator_index!=-1){
    pin_name_string = received_message.substring(0, separator_index);
    value_string = received_message.substring(separator_index+1);
    
    switch (pin_name_string[0]){
      // the case is based on the first char of the pin name string. this is the convention for the communication:
      // "D23:1" for digital write use D + pin number + : + value (1 or 0).
      // "A3:?" for analog read use A + pin number + : + ?
      // "O1:089" for analog write (on pins DAC0 and DAC1) use O + pin number + : + value (from 0 to 255) 
      case 'D':
        // digital case
        pin_id = pin_name_string.substring(1).toInt();
        value = value_string.toInt();
        // if(digital_io[pin_id] == 0){
        //   pinMode(pin_id, OUTPUT);
        //   digital_io[pin_id] = 1;
        // }
        setDigitalIo(pin_id, 0);

        if(value == 0){
          digitalWrite(pin_id, LOW);
        }else{
          digitalWrite(pin_id, HIGH);
        }
        break;
      case 'A':
        // analog case
        pin_id = pin_name_string.substring(1).toInt();
        analog_read_temp = analogRead(analog_inputs[pin_id]);
        Serial.println("A"+String(pin_id) + ":" + String(analog_read_temp));
        break;
      case 'O':
        // dac case
        pin_id = pin_name_string.substring(1).toInt();
        value = value_string.toInt();
        analogWrite(analog_output[pin_id], value);
        break;
      case 'L':
        // measure lenght case
        separator_index_l = pin_name_string.indexOf("_");
        trig_pin = pin_name_string.substring(1, separator_index_l).toInt();
        echo_pin = pin_name_string.substring(separator_index_l).toInt();
        
        setDigitalIo(trig_pin, 0);
        setDigitalIo(echo_pin, 1);
        // if(digital_io[pin_id] == 0){
        //   pinMode(pin_id, OUTPUT);
        //   digital_io[pin_id] = 1;
        // }
        float distance;
        distance = measureDistance(trig_pin, echo_pin);
        Serial.println(String("L")+":"+String(distance));
        break;
    }
  }
  while(!Serial) {  //check if Serial is available... if not, restart serial communication
    Serial.end();      // close serial port
    delay(100);        //wait 100 millisj
    Serial.begin(9600); // reenable serial again
    delay(1000);
  }
  delay(10);
}
