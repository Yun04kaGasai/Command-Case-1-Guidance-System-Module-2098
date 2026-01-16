#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

#define LASER_PIN   5    // KY-008
#define SERVO_PIN   6    // GH-S37A (азимут)
#define CE_PIN      14   // nRF24
#define CSN_PIN     15

RF24 radio(CE_PIN, CSN_PIN);
Servo servo;

const byte address[6] = "TGT01";

struct Command {
  bool laser_on = false;
  uint8_t servo_angle = 90;
};

void setup() {
  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LASER_PIN, LOW);
  
  servo.attach(SERVO_PIN);
  servo.write(90);

  SPI.begin();

  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_1MBPS);
  radio.openReadingPipe(0, address);
  radio.startListening();

  Serial.begin(115200);
  Serial.println("Pico: готов");
}

void loop() {
  if (radio.available()) {
    Command cmd;
    radio.read(&cmd, sizeof(cmd));

    digitalWrite(LASER_PIN, cmd.laser_on ? HIGH : LOW);
    servo.write(cmd.servo_angle);

    Serial.print("Лазер: ");
    Serial.print(cmd.laser_on ? "ON" : "OFF");
    Serial.print(" | Угол: ");
    Serial.print(cmd.servo_angle);
    Serial.println("°");
  }
  delay(10);
}