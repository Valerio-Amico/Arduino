//  Corresponding Arduino: 
//  arduinodriver
//  SN: 75135333636351013221
//   Port: /dev/ttyACM3

//  Pin definition for the output
int digital_outputs[14];
int analog_inputs[6] = {A0, A1, A2, A3, A4, A5};
int analog_output[6] = {A0, A1, A2, A3, A4, A5};

// utility variables
String received_message, pin_name_string, value_string;
int separator_index, pin_id, value, analog_read_temp;

String check_message(String pin_name_, String value_str_);
 
void setup() {
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
        if(digital_outputs[pin_id] == 0){
          pinMode(pin_id, OUTPUT);
          digital_outputs[pin_id] = 1;
        }
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
