// sketch_jan6a_tx_final.ino
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10);
const byte address[6] = "TGT01";

struct Command {
  bool laser_on;
  uint8_t servo_angle;
};

void setup() {
  Serial.begin(115200);
  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_1MBPS);
  radio.openWritingPipe(address);
  radio.stopListening();
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    Command cmd = {false, 90};

    if (input.indexOf("L1") >= 0) cmd.laser_on = true;
    if (input.indexOf("A") >= 0) {
      int angle = input.substring(input.indexOf("A") + 1).toInt();
      cmd.servo_angle = constrain(angle, 0, 180);
    }

    radio.write(&cmd, sizeof(cmd));
    Serial.println("Отправлено");
  }
  delay(10);
}