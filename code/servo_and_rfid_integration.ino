#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN    5    // ESP32 pin GPIO5
#define RST_PIN   22   // ESP32 pin GPIO22 (Reset pin for MFRC522)
#define SERVO_PIN 32   // ESP32 pin GPIO32 connects to servo motor

MFRC522 rfid(SS_PIN, RST_PIN);
Servo servo;

byte authorizedUID1[4] = {0x80, 0x5A, 0xB9, 0x55}; // RFID tag code: 80 5A B9 55
byte authorizedUID2[4] = {0x33, 0xD7, 0xCE, 0xC9}; // RFID tag code: 33 D7 CE C9

int angle = 0; // the current angle of servo motor

void setup() {
  Serial.begin(9600);
  SPI.begin();               // Initialize SPI bus
  rfid.PCD_Init();           // Initialize RFID reader
  servo.attach(SERVO_PIN);   // Attach the servo to its pin
  servo.write(angle);        // Set initial angle to 0°
  Serial.println("Tap RFID/NFC Tag on reader");
}

void loop() {
  if (rfid.PICC_IsNewCardPresent()) {
    if (rfid.PICC_ReadCardSerial()) {
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

      if (memcmp(rfid.uid.uidByte, authorizedUID1, 4) == 0) {
        Serial.println("Authorized Tag 1");
        changeServo();
      } else if (memcmp(rfid.uid.uidByte, authorizedUID2, 4) == 0) {
        Serial.println("Authorized Tag 2");
        changeServo();
      } else {
        Serial.print("Unauthorized Tag with UID:");
        for (int i = 0; i < rfid.uid.size; i++) {
          Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
          Serial.print(rfid.uid.uidByte[i], HEX);
        }
        Serial.println();
      }

      rfid.PICC_HaltA();       // Halt communication with the card
      rfid.PCD_StopCrypto1();  // Stop encryption on PCD
    }
  }
}

void changeServo() {
  // Toggle servo angle between 0° and 90°
  angle = (angle == 0) ? 90 : 0;
  servo.write(angle);
  Serial.print("Rotate Servo Motor to ");
  Serial.print(angle);
  Serial.println("°");
}
