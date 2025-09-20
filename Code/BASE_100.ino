#define ENA 10  // Speed control for Left Motor
#define ENB 5   // Speed control for Right Motor
#define IN1 9   // Left Motor Direction 1
#define IN2 8   // Left Motor Direction 2
#define IN3 7   // Right Motor Direction 1
#define IN4 6   // Right Motor Direction 2
#define IR1 A0
#define IR2 A1
#define IR3 A2
#define IR4 A3
#define IR5 A4

// PID Constants
float Kp = 40;
float Ki = 0;
float Kd = 200;

// Robot Speed
int baseSpeed = 100;
float lastError = 0;
float integral = 0;

// All black detection
unsigned long allBlackStart = 0;
bool stopped = false;

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int s1 = analogRead(IR1);
  int s2 = analogRead(IR2);
  int s3 = analogRead(IR3);
  int s4 = analogRead(IR4);
  int s5 = analogRead(IR5);

  int threshold = 850;

  // Binary flags: true if sensor sees the line (value > threshold)
  bool b1 = s1 > threshold;
  bool b2 = s2 > threshold;
  bool b3 = s3 > threshold;
  bool b4 = s4 > threshold;
  bool b5 = s5 > threshold;

  int currentBaseSpeed;

  if (b3) {
    currentBaseSpeed = baseSpeed;   // Normal speed
  } else {
    currentBaseSpeed = 65;          // Reduced speed when center sensor misses line
  }

  // --- Permanent STOP condition ---
  if (b1 && b2 && b3 && b4 && b5 && !stopped) {
    if (allBlackStart == 0) allBlackStart = millis();
    if (millis() - allBlackStart >= 1000) {   // 1 sec hold
      Serial.println("All black for 1s -> STOP");
      stopMotors();
      stopped = true;   // freeze permanently
    }
    return;  // don’t run PID while in all black state
  } else if (!(b1 && b2 && b3 && b4 && b5)) {
    allBlackStart = 0;  // reset if not all black anymore
  }

  if (stopped) return;  // stay frozen permanently

 

  // Position weighting (left negative, right positive)
  int position = (-2 * b1) + (-1 * b2) + (0 * b3) + (1 * b4) + (2 * b5);

  // Smart filter: neutralize small central combos (keep previous sign)
   
  float error = position;
  integral += error;
  if (integral > 10) integral  = 0;
  float derivative = error - lastError;
  
  if (abs(error) < 0.1) integral = 0;

  float correction = (Kp * error) + (Ki * integral) + (Kd * derivative);

  // clamp correction
  if (correction > 120) correction = 255;
  if (correction < -120) correction = -255;

  int rightSpeed = (int)constrain(baseSpeed - correction, 0, 255);
  int leftSpeed  = (int)constrain(baseSpeed + correction, 0, 255);
  moveMotors(leftSpeed, rightSpeed);
  // Debug
  Serial.print("Err: "); Serial.print(error);
  Serial.print(" | L: "); Serial.print(leftSpeed);
  Serial.print(" | R: "); Serial.println(rightSpeed);

  // Special cases: call search functions
  if ((b1 && b2 && b3 && !b4 && !b5) || (b1 && b2 && b3 && b4 && !b5)) {
    lastError = -0.1;
    Serial.println("Pattern -> turnLeft search");
    searchForLineLeft(threshold);
    return;
  }
  if ((!b1 && !b2 && b3 && b4 && b5) || (!b1 && b2 && b3 && b4 && b5)) {
    lastError = 0.1;
    Serial.println("Pattern -> turnRight search");
    searchForLineRight(threshold);
    return;
  }
 // --- Line lost handling ---
if (!b1 && !b2 && !b3 && !b4 && !b5) {
  // Line lost (gap or discontinuity)
  
  // Small wait before action (gap recovery)
  delay(10);  

  // Check sensors again after delay
  if (!b1 && !b2 && !b3 && !b4 && !b5) {
    // Still no line → now decide
    if (lastError < 0) {
      searchForLineLeft(threshold);
    }
    else if (lastError > 0) {
      searchForLineRight(threshold);
    }
    else {
      reverse(threshold);
    }
  }
  return;
}



  
  lastError = error;
}

// ---------------- Motor control helpers ----------------

void moveMotors(int leftSpeed, int rightSpeed) {
  digitalWrite(IN1, HIGH);  // LEFT forward
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);   // RIGHT forward (flipped polarity)
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, leftSpeed);
  analogWrite(ENB, rightSpeed);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void searchForLineLeft(int threshold) {
  while (analogRead(IR1) < threshold &&
         analogRead(IR2) < threshold &&
         analogRead(IR3) < threshold &&
         analogRead(IR4) < threshold &&
         analogRead(IR5) < threshold) {
    digitalWrite(IN1, LOW);   // LEFT backward
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);   // RIGHT forward
    digitalWrite(IN4, HIGH);

    analogWrite(ENA, 100);
    analogWrite(ENB, 100);

  }
  integral = 0;
}

void searchForLineRight(int threshold) {
  while (analogRead(IR1) < threshold &&
         analogRead(IR2) < threshold &&
         analogRead(IR3) < threshold &&
         analogRead(IR4) < threshold &&
         analogRead(IR5) < threshold) {
    digitalWrite(IN1, HIGH);  // LEFT forward
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);  // RIGHT backward
    digitalWrite(IN4, LOW);

    analogWrite(ENA, 100);
    analogWrite(ENB, 100);
 
  }
  integral = 0;
}

void reverse(int threshold) {
  while (analogRead(IR1) < threshold &&
         analogRead(IR2) < threshold &&
         analogRead(IR3) < threshold &&
         analogRead(IR4) < threshold &&
         analogRead(IR5) < threshold) {
    digitalWrite(IN1, LOW);   // LEFT backward
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);  // RIGHT backward
    digitalWrite(IN4, LOW);

    analogWrite(ENA, 90);
    analogWrite(ENB, 90);
    delay(10);
  }
  integral = 0;
}
