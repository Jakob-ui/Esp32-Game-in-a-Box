#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define LED_False 5
#define LED_True 15

#define SS_PIN 21
#define RST_PIN 22       

MFRC522 rfid(SS_PIN, RST_PIN);  

const uint8_t expectedUID[] = {0x03, 0x85, 0x99, 0x92};
const uint8_t expectedUIDLength = sizeof(expectedUID) / sizeof(expectedUID[0]);


void setup() {
  Serial.begin(115200);
  pinMode(LED_False, OUTPUT);
  pinMode(LED_True, OUTPUT);
  SPI.begin(); 
  rfid.PCD_Init();
  digitalWrite(LED_False, LOW);
  digitalWrite(LED_True, LOW);
  Serial.println("Tap an RFID/NFC tag on the RFID-RC522 reader");
}

void loop() {
  digitalWrite(LED_False, LOW);
  digitalWrite(LED_True, LOW);
 if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been read
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      Serial.print("RFID/NFC Tag Type: ");
      Serial.println(rfid.PICC_GetTypeName(piccType));

      // Print UID in Serial Monitor in the hex format
      Serial.print("UID:");
      for (int i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid.uid.uidByte[i], HEX);
      }
      Serial.println();

      // Compare the UID to the expected UID
      if (rfid.uid.size == expectedUIDLength && memcmp(rfid.uid.uidByte, expectedUID, rfid.uid.size) == 0) {
        Serial.println("Hallo Jakob");
        digitalWrite(LED_True, HIGH);
		digitalWrite(LED_False, LOW);
		delay(1000);
      } else {
        Serial.println("Login failed!");
        digitalWrite(LED_True, LOW);
        digitalWrite(LED_False, HIGH);
        delay(1000);
      }

      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }
  
}

