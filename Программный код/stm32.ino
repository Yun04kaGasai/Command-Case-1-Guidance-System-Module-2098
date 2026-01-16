#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN  PA0
#define CSN_PIN PA1

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "TGT01";

struct Command {
  bool laserOn;
  int8_t azimuth;
  int8_t tilt;
};

void setup() {
  Serial.begin(115200);
  SPI.setMOSI(PA7);
  SPI.setMISO(PA6);
  SPI.setSCK(PA5);
  SPI.begin();

  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_1MBPS);
  radio.openWritingPipe(address);
  radio.stopListening();
}

void sendPosition(bool laser, int8_t az, int8_t ti) {
  Command cmd = {laser, az, ti};
  radio.write(&cmd, sizeof(cmd));
  delay(100);
}

void runScans() {
  sendPosition(false, 0, 0);
  delay(1000);
  for (int a = -40; a <= 40; a += 10) {
    sendPosition(true, a, 0);
    delay(3000);
  }
  for (int t = -40; t <= 40; t += 10) {
    sendPosition(true, 0, t);
    delay(3000);
  }
  for (int i = -40; i <= 40; i += 10) {
    sendPosition(true, i, i);
    delay(3000);
  }
  for (int i = -40; i <= 40; i += 10) {
    sendPosition(true, i, -i);
    delay(3000);
  }
  sendPosition(false, 0, 0);
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    if (input.indexOf("START") >= 0) {
      runScans();
    }
  }
  delay(10);
}