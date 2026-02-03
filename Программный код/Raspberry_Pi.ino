#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
#define SERVO_AZIM   6
#define SERVO_TILT   7
#define SERVO_COVER  8
#define CE_PIN  14
#define CSN_PIN 15

RF24 radio(CE_PIN, CSN_PIN);
Servo servoAzim, servoTilt, servoCover;
const byte pipe[6] = "TGT01";

struct Message {
  uint8_t sender_id;
  uint8_t target_id;
  char command[4];
  int8_t azimuth;
  int8_t tilt;
  uint8_t mode;
};

int angleToServo(int8_t angle) {
  return 90 + angle;
}

void sendTelemetry(uint8_t to_id, int8_t az, int8_t ti, uint8_t md) {
  Message msg;
  msg.sender_id = 69;
  msg.target_id = to_id;
  strncpy(msg.command, "POS", 3);
  msg.command[3] = '\0';
  msg.azimuth = az;
  msg.tilt = ti;
  msg.mode = md;

  radio.stopListening();
  radio.write(&msg, sizeof(msg));
  radio.startListening();
}

void runScans() {
  static bool coverOpened = false;
  if (!coverOpened) {
    servoCover.write(90);
    coverOpened = true;
  }

  //горизонтал
  for (int a = -40; a <= 40; a += 10) {
    servoAzim.write(angleToServo(a));
    servoTilt.write(angleToServo(0));
    sendTelemetry(67, a, 0, 1);
    delay(3000);
  }

  //верт
  for (int t = -40; t <= 40; t += 10) {
    servoAzim.write(angleToServo(0));
    servoTilt.write(angleToServo(t));
    sendTelemetry(67, 0, t, 2);
    delay(3000);
  }

  //-40,-40 -> 40,40
  for (int i = -40; i <= 40; i += 10) {
    servoAzim.write(angleToServo(i));
    servoTilt.write(angleToServo(i));
    sendTelemetry(67, i, i, 3);
    delay(3000);
  }

  //-40,40 -> 40,-40
  for (int i = -40; i <= 40; i += 10) {
    servoAzim.write(angleToServo(i));
    servoTilt.write(angleToServo(-i));
    sendTelemetry(67, i, -i, 4);
    delay(3000);
  }

  servoAzim.write(90);
  servoTilt.write(90);
  sendTelemetry(67, 0, 0, 0);
}

void setup() {
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
  radio.openReadingPipe(0, pipe);
  radio.startListening();
  Serial.begin(115200);
}

void loop() {
  if (radio.available()) {
    Message msg;
    radio.read(&msg, sizeof(msg));

    if (msg.target_id == 69 && strncmp(msg.command, "SCAN", 4) == 0) {
      runScans();
    }
  }
  delay(10);
}