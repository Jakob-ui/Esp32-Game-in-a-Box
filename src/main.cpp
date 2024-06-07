#include <Arduino.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <oled_Display.h>

int calculateDutyCycle(int angle);
void servoUp();
void servoDown();

#define LED_False 5
#define LED_True 15

#define SS_PIN 21
#define RST_PIN 16       

#define SERVO_PIN 4 
#define LEDC_CHANNEL 0
#define RESOLUTION 16
#define REQUENCY 50

bool ServoOn;
bool ServoOff;

MFRC522 rfid(SS_PIN, RST_PIN);  

const uint8_t expectedUID[] = {0x03, 0x85, 0x99, 0x92};
const uint8_t expectedUIDLength = sizeof(expectedUID) / sizeof(expectedUID[0]);


void setup() {
  Serial.begin(115200);
  ledcSetup(LEDC_CHANNEL, REQUENCY, RESOLUTION);
  ledcAttachPin(SERVO_PIN, LEDC_CHANNEL);
  int dutyCycle = calculateDutyCycle(0);
  ledcWrite(LEDC_CHANNEL, dutyCycle);
  delay(200);
  pinMode(LED_False, OUTPUT);
  pinMode(LED_True, OUTPUT);
  SPI.begin(); 
  rfid.PCD_Init();
  digitalWrite(LED_False, LOW);
  digitalWrite(LED_True, LOW);
  Serial.println("Tap an RFID/NFC tag on the RFID-RC522 reader");
  oledSetup();
}

void loop() {
  digitalWrite(LED_False, LOW);
  digitalWrite(LED_True, LOW);
 if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) {
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
    ServoOn = true;
    servoUp();
      } else {
        Serial.println("Login failed!");
        digitalWrite(LED_True, LOW);
        digitalWrite(LED_False, HIGH);
        delay(1000);
        ServoOff = true;
        servoDown();
      }

      rfid.PICC_HaltA(); 
      rfid.PCD_StopCrypto1(); 
    }
  }
  
}

int calculateDutyCycle(int angle) {
  float dutyCycle = map(angle, 0, 180, 1500, 7900);
  return static_cast<int>(dutyCycle);
}

void servoUp(){
  if(ServoOn){
  int dutyCycle = calculateDutyCycle(180);
  ledcWrite(LEDC_CHANNEL, dutyCycle);
  delay(1000);
  ServoOn = false;
  }
}

void servoDown(){
  if(ServoOff){
     int dutyCycle = calculateDutyCycle(0);
  ledcWrite(LEDC_CHANNEL, dutyCycle);
  delay(1000);
  ServoOff = false;
  }
}