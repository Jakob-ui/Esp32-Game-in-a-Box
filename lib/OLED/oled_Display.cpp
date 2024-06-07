#include <oled_Display.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void oledSetup(){
  Serial.begin(9600);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();
  
  display.setTextSize(4);    
  display.setTextColor(SSD1306_WHITE); 
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds("J", 0, 0, &x1, &y1, &w, &h); 
  int16_t x = (SCREEN_WIDTH - w) / 2; 
  int16_t y = (SCREEN_HEIGHT - h) / 2;
  display.setCursor(x, y);    
  display.write('J');
  display.display(); 
  delay(4000);
  
  display.clearDisplay();
  display.display();
}


void displayCenteredText(const char* text, uint8_t textSize) {
  display.clearDisplay();
  display.setTextSize(textSize);
  display.setTextColor(WHITE);
  
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

  int16_t x = (SCREEN_WIDTH - w) / 2;
  int16_t y = (SCREEN_HEIGHT - h) / 2;

  display.setCursor(x, y);
  display.println(text);
  display.display();
}

void turnOffDisplay(){
  display.ssd1306_command(SSD1306_DISPLAYOFF);
}

void turnONDisplay(){
  display.ssd1306_command(SSD1306_DISPLAYON);
}

void updateDisplay(){
  display.display();
}