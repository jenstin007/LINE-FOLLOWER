#define ENA 10  // Speed control for Left Motor
#define ENB 11 // Speed control for Right Motor
#define IN1 9 // Left Motor Direction 1
#define IN2 8  // Left Motor Direction 2
#define IN3 7  // Right Motor Direction 1
#define IN4 6  // Right Motor Direction 2
#define IR1 A0
#define IR2 A1
#define IR3 A2
#define IR4 A3
#define IR5 A4

void setup() {
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    Serial.begin(9600);
void loop() {
  
    Serial.println("Moving Forward");
    analogWrite(ENA, 80);  // Left motor speed
    analogWrite(ENB, 75); // Right motor speed

    // 🔄 Swapped direction HIGH/LOW for Forward
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    delay(5000);

    Serial.println("Moving Backward");
    // 🔄 Swapped direction HIGH/LOW for Backward
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    delay(5000);

    Serial.println("Stopping");
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    delay(2000);
}
