#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <Servo.h>

// Pin Definitions
#define TRIG_PIN D5
#define ECHO_PIN D6
#define SERVO_PIN D7

// Motor Pins (example - change if needed)
int motorPins[] = {D1, D2, D3, D4};

// Relay Pins
int relayPins[] = {D0, D8};

// Objects
Servo seedServo;
FirebaseData firebaseData;

// WiFi Credentials
#define WIFI_SSID "YOUR_WIFI"
#define WIFI_PASSWORD "YOUR_PASSWORD"

// Firebase Credentials
#define FIREBASE_HOST "YOUR_FIREBASE_URL"
#define FIREBASE_AUTH "YOUR_FIREBASE_SECRET"

bool obstacleDetected = false;

// -------------------- Setup --------------------
void setup() {
  Serial.begin(115200);

  // Pin setup
  for (int i = 0; i < 4; i++) {
    pinMode(motorPins[i], OUTPUT);
  }

  for (int i = 0; i < 2; i++) {
    pinMode(relayPins[i], OUTPUT);
  }

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Servo
  seedServo.attach(SERVO_PIN);
  seedServo.write(0);

  // WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  stopMotors();
  allRelaysOff();
}

// -------------------- Loop --------------------
void loop() {
  checkObstacle();

  if (!obstacleDetected) {

    // Direction control
    if (Firebase.getString(firebaseData, "/Pesticide/direction")) {
      String direction = firebaseData.stringData();
      executeMovement(direction);
    }

    // Relay control
    if (Firebase.getInt(firebaseData, "/Pesticide/Relay")) {
      int relayValue = firebaseData.intData();
      executeRelay(relayValue);
      updateServo(relayValue);
    }
  }

  delay(100);
}

// -------------------- Functions --------------------

void executeMovement(String dir) {
  if (dir == "forward") {
    // forward logic
  } else if (dir == "backward") {
    // backward logic
  } else if (dir == "left") {
    // left logic
  } else if (dir == "right") {
    // right logic
  } else {
    stopMotors();
  }
}

void executeRelay(int val) {
  if (val == 1) {
    digitalWrite(relayPins[0], HIGH);
  } else {
    digitalWrite(relayPins[0], LOW);
  }
}

void updateServo(int val) {
  if (val == 2) {
    seedServo.write(90);
  } else {
    seedServo.write(0);
  }
}

void stopMotors() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(motorPins[i], LOW);
  }
}

void allRelaysOff() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(relayPins[i], LOW);
  }
}

void checkObstacle() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  int distance = duration * 0.034 / 2;

  if (distance < 20) {
    obstacleDetected = true;
    stopMotors();
  } else {
    obstacleDetected = false;
  }
}