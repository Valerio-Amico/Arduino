// HC-SR04 Ultrasonic Sensor
// TRIG → Pin 9  |  ECHO → Pin 10

const int TRIG_PIN = 9;
const int ECHO_PIN = 10;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  float distance = measureDistance();

  Serial.print("Distanza: ");
  Serial.print(distance, 1);
  Serial.println(" cm");

  delay(500);
}

float measureDistance() {
  // Invia impulso TRIG di 10µs
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Legge la durata del segnale ECHO
  long duration = pulseIn(ECHO_PIN, HIGH); // timeout 30ms

  // Timeout → nessun oggetto rilevato
  if (duration == 0) return -1;

  // Calcola la distanza: velocità del suono ≈ 343 m/s = 0.0343 cm/µs
  // Diviso 2 perché il suono fa andata + ritorno
  return (duration * 0.0343) / 2.0;
}
