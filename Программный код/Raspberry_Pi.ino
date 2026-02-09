#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

#define SERVO_AZIM   6
#define SERVO_TILT   7
#define SERVO_COVER  8
#define LASER_PIN    5
#define CE_PIN       14
#define CSN_PIN      15

RF24 radio(CE_PIN, CSN_PIN);
Servo servoAzim, servoTilt, servoCover;
const byte address[6] = "CMD01";

int angleToServo(int8_t angle) {
  return 90 + angle;
}

void runScans() {
  static bool coverOpened = false;
  if (!coverOpened) {
    servoCover.write(90);
    coverOpened = true;
  }

  digitalWrite(LASER_PIN, HIGH);

  for (int a = -40; a <= 40; a += 10) {
    servoAzim.write(angleToServo(a));
    servoTilt.write(angleToServo(0));
    delay(3000);
  }

  for (int t = -40; t <= 40; t += 10) {
    servoAzim.write(angleToServo(0));
    servoTilt.write(angleToServo(t));
    delay(3000);
  }

  for (int i = -40; i <= 40; i += 10) {
    servoAzim.write(angleToServo(i));
    servoTilt.write(angleToServo(i));
    delay(3000);
  }

  for (int i = -40; i <= 40; i += 10) {
    servoAzim.write(angleToServo(i));
    servoTilt.write(angleToServo(-i));
    delay(3000);
  }

  digitalWrite(LASER_PIN, LOW);
  servoAzim.write(90);
  servoTilt.write(90);
}

void setup() {
  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LASER_PIN, LOW);

  servoAzim.attach(SERVO_AZIM);
  servoTilt.attach(SERVO_TILT);
  servoCover.attach(SERVO_COVER);

  servoCover.write(0);
  servoAzim.write(90);
  servoTilt.write(90);

  SPI.begin();
  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_1MBPS);
  radio.openReadingPipe(0, address);
  radio.startListening();

  Serial.begin(115200);
}

void loop() {
  if (radio.available()) {
    char cmd[6] = {0};
    radio.read(cmd, sizeof(cmd));
    if (strncmp(cmd, "START", 5) == 0) {
      Serial.println("RECEIVED");
      runScans();
    }
  }
  delay(10);
}