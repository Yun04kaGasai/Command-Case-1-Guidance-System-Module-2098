#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN  PA0
#define CSN_PIN PA1

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "CMD01";

void setup() {
  pinMode(PC13, OUTPUT);
  Serial.begin(115200);

  SPI.setMOSI(PA7);
  SPI.setMISO(PA6);
  SPI.setSCLK(PA5);
  SPI.begin();

  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_1MBPS);
  radio.openWritingPipe(address);
  radio.stopListening();

  digitalWrite(PC13, LOW);
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input == "START") {
      const char cmd[] = "START";
      bool ok = radio.write(cmd, sizeof(cmd));
      if (ok) {
        Serial.println("SENT");
      } else {
        Serial.println("FAIL");
      }
    }
  }
  delay(10);
}