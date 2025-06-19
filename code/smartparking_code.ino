#include <Arduino.h>
#include <ESP32Servo.h>
#include <MFRC522.h>
#include <SPI.h>
#include "thingProperties.h"

#define SS_PIN    5    // ESP32 pin GPIO5 
#define RST_PIN   22   // ESP32 pin GPIO22 (Reset pin for MFRC522)
#define SERVO_PIN 32   // ESP32 pin GPIO32 connects to servo motor
#define IR_SENSOR_PIN_1 13  // ESP32 pin GPIO13 connects to first IR sensor
#define IR_SENSOR_PIN_2 12  // ESP32 pin GPIO12 connects to second IR sensor

const int NUM_SPOTS = 2;  // Number of parking spots

MFRC522 rfid(SS_PIN, RST_PIN);
Servo servo;

byte authorizedUID1[4] = {0x80, 0x5A, 0xB9, 0x55}; // RFID tag code: 80 5A B9 55
byte authorizedUID2[4] = {0x33, 0xD7, 0xCE, 0xC9}; // RFID tag code: 33 D7 CE C9

int angle = 0; // the current angle of servo motor

unsigned long entryTimes[NUM_SPOTS] = {0};   // Array to store entry times (in milliseconds) for each spot
unsigned long exitTimes[NUM_SPOTS] = {0};    // Array to store exit times (in milliseconds) for each spot
bool vehiclePresent[NUM_SPOTS] = {false};    // Array to track vehicle presence for each spot

void setup() {
  Serial.begin(9600);
  delay(1500);

  initProperties();
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522
  servo.attach(SERVO_PIN);
  servo.write(angle); // rotate servo motor to 0°

  pinMode(IR_SENSOR_PIN_1, INPUT);
  pinMode(IR_SENSOR_PIN_2, INPUT);

  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  Serial.println("Tap RFID/NFC Tag on reader");
}

void loop() {
  ArduinoCloud.update();

  if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been read
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

      if (memcmp(rfid.uid.uidByte, authorizedUID1, 4) == 0 || memcmp(rfid.uid.uidByte, authorizedUID2, 4) == 0) {
        Serial.println("Authorized Tag");
        changeServo();
      } else {
        Serial.print("Unauthorized Tag with UID:");
        for (int i = 0; i < rfid.uid.size; i++) {
          Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
          Serial.print(rfid.uid.uidByte[i], HEX);
        }
        Serial.println();
      }

      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }

  handleParkingSpot(0, IR_SENSOR_PIN_1);
  handleParkingSpot(1, IR_SENSOR_PIN_2);

  // Display slot availability
  for (int i = 0; i < NUM_SPOTS; i++) {
    Serial.print("Slot ");
    Serial.print(i + 1);
    Serial.print(": ");
    if (vehiclePresent[i]) {
      Serial.println("Occupied");
    } else {
      Serial.println("Available");
    }
  }

  delay(1000); // Delay to avoid flooding the serial monitor
}

void handleParkingSpot(int spotIndex, int sensorPin) {
  int sensorValue = digitalRead(sensorPin);

  // Check if vehicle is present (IR sensor output is LOW)
  if (sensorValue == LOW && !vehiclePresent[spotIndex]) {
    vehiclePresent[spotIndex] = true;
    entryTimes[spotIndex] = millis();  // Record entry time for the spot
    Serial.print("Vehicle parked in spot ");
    Serial.print(spotIndex + 1);
    Serial.println(". Entry time recorded.");
    updateIRValues();
  }
  
  // Check if vehicle has left (IR sensor output is HIGH)
  if (sensorValue == HIGH && vehiclePresent[spotIndex]) {
    vehiclePresent[spotIndex] = false;
    exitTimes[spotIndex] = millis();  // Record exit time for the spot
    unsigned long parkingDuration = exitTimes[spotIndex] - entryTimes[spotIndex];
    float parkingHours = parkingDuration / 3600000.0;  // Convert milliseconds to hours
    float billingAmount = calculateBillingAmount(parkingHours);  // Calculate billing amount
    Serial.print("Vehicle left spot ");
    Serial.print(spotIndex + 1);
    Serial.print(". Parking Duration: ");
    Serial.print(parkingHours, 2);
    Serial.print(" hours. Billing Amount: Rs");
    Serial.println(billingAmount, 2);
    updateIRValues();
  }
}

void updateIRValues() {
  irvalue1 = digitalRead(IR_SENSOR_PIN_1);
  irvalue2 = digitalRead(IR_SENSOR_PIN_2);
}

void changeServo() {
  // Change angle of servo motor
  if (angle == 0) {
    angle = 90;
  } else {
    angle = 0;
  }

  // Control servo motor according to the angle
  servo.write(angle);
  Serial.print("Rotate Servo Motor to ");
  Serial.print(angle);
  Serial.println("°");
}

// Function to calculate billing amount based on parking hours
float calculateBillingAmount(float parkingHours) {
  // Define your billing rates and rules here
  const float HOURLY_RATE = 10.0;   // Rs10 per hour
  const float MAX_HOURLY_RATE = 50.0;  // Maximum charge for a single day

  float billingAmount = 0.0;
  if (parkingHours <= 1) {
    billingAmount = HOURLY_RATE;
  } else if (parkingHours > 1 && parkingHours <= 24) {
    billingAmount = min(MAX_HOURLY_RATE, parkingHours * HOURLY_RATE);
  } else {
    billingAmount = MAX_HOURLY_RATE;  // Assuming Rs50 per day
  }

  return billingAmount;
}

// Callback function for IR sensor 1 change
void onIrvalue1Change() {
  Serial.print("IR Sensor 1 Value changed to: ");
  Serial.println(irvalue1);
}

// Callback function for IR sensor 2 change
void onIrvalue2Change() {
  Serial.print("IR Sensor 2 Value changed to: ");
  Serial.println(irvalue2);
}