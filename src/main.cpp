#include <Arduino.h>
#include <MFRC522.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>


AsyncWebServer server(80);

const char* ssid = "Pixel7pro_J";
const char* password = "123456789";

const char* PARAM_MESSAGE = "message";
void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}


int calculateDutyCycle(int angle);
void servoUp();
void servoDown();
void timerBegin();
void oledDisplay();

#define LED_False 5
#define LED_True 15

#define SS_PIN 16
#define RST_PIN 3       

#define SERVO_PIN 4
#define LEDC_CHANNEL 0
#define RESOLUTION 16
#define REQUENCY 50

#define OLED_RESET     -1
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int numberToDisplay = 100;

bool ServoOn;
bool ServoOff;
bool gameActive = false;

bool boxOpen = false;
#define REEDSWITCH 2

unsigned long currentmillis;
unsigned long previousmillis;
float timer = 30;

MFRC522 rfid(SS_PIN, RST_PIN);  

const uint8_t expectedUID[] = {0x03, 0x85, 0x99, 0x92};
const uint8_t expectedUIDLength = sizeof(expectedUID) / sizeof(expectedUID[0]);


void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        return;
    }
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    
    

    server.onNotFound(notFound);
    server.begin();

  ledcSetup(LEDC_CHANNEL, REQUENCY, RESOLUTION);
  ledcAttachPin(SERVO_PIN, LEDC_CHANNEL);
  int dutyCycle = calculateDutyCycle(0);
  ledcWrite(LEDC_CHANNEL, dutyCycle);
  delay(200);
  pinMode(LED_False, OUTPUT);
  pinMode(LED_True, OUTPUT);
  pinMode(REEDSWITCH, INPUT);
  SPI.begin(); 
  rfid.PCD_Init();
  digitalWrite(LED_False, LOW);
  digitalWrite(LED_True, LOW);
  Serial.println("Tap an RFID/NFC tag on the RFID-RC522 reader");

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
  Serial.println(F("SSD1306 allocation failed"));
  for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(4);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0,0);
  


  
  display.print(numberToDisplay);
  
  display.display();
}

void loop() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      String message = String(numberToDisplay);
        request->send(200, "text/plain", "My Score" + message);
    });

  oledDisplay();
  currentmillis = millis();
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
    gameActive = true;
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

void oledDisplay(){
  if(digitalRead(REEDSWITCH) == LOW && gameActive){
  display.clearDisplay();
  numberToDisplay -= 1;
  display.setCursor(0,0);
  display.print(numberToDisplay);
  display.display();
  }else{
    display.clearDisplay();
    numberToDisplay = 100;
      display.setCursor(0,0);
  display.print(numberToDisplay);
  display.display();
  }
  if(numberToDisplay <= 0){
    gameActive = false;
    ServoOff = true;
    servoDown();
}
}