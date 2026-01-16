#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

#define LASER_PIN    5
#define SERVO_AZIM   6
#define SERVO_TILT   7
#define SERVO_COVER  8
#define CE_PIN  14
#define CSN_PIN 15

RF24 radio(CE_PIN, CSN_PIN);
Servo servoAzim, servoTilt, servoCover;

const byte address[6] = "TGT01";

struct Command {
  bool laserOn;
  int8_t azimuth;
  int8_t tilt;
};

int angleToServo(int8_t angle) {
  return 90 + angle;
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
    Command cmd;
    radio.read(&cmd, sizeof(cmd));
    static bool coverOpened = false;
    if (!coverOpened && (cmd.azimuth != 0 || cmd.tilt != 0)) {
      servoCover.write(90);
      delay(1000);
      coverOpened = true;
    }
    int az = angleToServo(cmd.azimuth);
    int ti = angleToServo(cmd.tilt);
    servoAzim.write(az);
    servoTilt.write(ti);
    digitalWrite(LASER_PIN, cmd.laserOn ? HIGH : LOW);
  }
  delay(10);
}