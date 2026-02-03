#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN  PA0
#define CSN_PIN PA1

RF24 radio(CE_PIN, CSN_PIN);
const byte pipe[6] = "TGT01";

struct Message {
  uint8_t sender_id;
  uint8_t target_id;
  char command[4];
  int8_t azimuth;
  int8_t tilt;
  uint8_t mode;
};

const char* modeToString(uint8_t mode) {
  switch (mode) {
    case 1: return "Horizontal";
    case 2: return "Vertical";
    case 3: return "Diagonal1";
    case 4: return "Diagonal2";
    default: return "Idle";
  }
}

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
  radio.openWritingPipe(pipe);
  radio.openReadingPipe(1, pipe);
  radio.stopListening();
  digitalWrite(PC13, LOW);
}

void sendScanCommand() {
  Message msg;
  msg.sender_id = 67;
  msg.target_id = 69;
  strncpy(msg.command, "SCAN", 4);
  msg.command[3] = '\0';
  msg.azimuth = 0;
  msg.tilt = 0;
  msg.mode = 0;
  bool ok = radio.write(&msg, sizeof(msg));
  if (ok) {
    Serial.println("команда SCAN отправлена");
  } else {
    Serial.println("ошибка отправки SCAN");
  }
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input == "START") {
      sendScanCommand();
      delay(100);
      radio.startListening();
    }
  }

  if (radio.available()) {
    Message msg;
    radio.read(&msg, sizeof(msg));

    if (msg.sender_id == 69 && strncmp(msg.command, "POS", 3) == 0) {
      Serial.print("От Pico(#69):");
      Serial.print("азимут=");
      Serial.print(msg.azimuth);
      Serial.print("°,наклон=");
      Serial.print(msg.tilt);
      Serial.print("°,режим=");
      Serial.println(modeToString(msg.mode));
    }
  }

  delay(10);
}