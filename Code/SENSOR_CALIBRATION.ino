#define S1 A0  // Leftmost sensor
#define S2 A1  
#define S3 A2  // Center sensor
#define S4 A3  
#define S5 A4  // Rightmost sensor

void setup() {
  Serial.begin(9600);  // Start Serial Monitor
}

void loop() {
  int sensor1 = analogRead(S1);
  int sensor2 = analogRead(S2);
  int sensor3 = analogRead(S3);
  int sensor4 = analogRead(S4);
  int sensor5 = analogRead(S5);

  Serial.print("S1: "); Serial.print(sensor1);
  Serial.print(" S2: "); Serial.print(sensor2);
  Serial.print(" S3: "); Serial.print(sensor3);
  Serial.print(" S4: "); Serial.print(sensor4);
  Serial.print(" S5: "); Serial.println(sensor5);
  
  delay(0); // Small delay to stabilize readings
}


