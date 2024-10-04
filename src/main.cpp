#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Wire.h>
#include <BH1750.h>

#define         SDA_PIN 17 
#define         SCL_PIN 18

TFT_eSPI        tft = TFT_eSPI(); 
char            lux_buf[20];
BH1750          lightMeter;

const int pulseA = 43;
const int pulseB = 44;
volatile int lastEncoded = 0;
volatile long encoderValue = 0;
char rotary_buf[20];

IRAM_ATTR void handleRotary() {
  int MSB = digitalRead(pulseA);
  int LSB = digitalRead(pulseB);

  int encoded = (MSB << 1) | LSB;
  int sum  = (lastEncoded << 2) | encoded;
  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue--;
  lastEncoded = encoded;
  if (encoderValue > 255) {
    encoderValue = 255;
  } else if (encoderValue < 0) {
    encoderValue = 0;
  }
}

IRAM_ATTR void buttonClicked() {
  Serial.println("pushed");
}

void setup() {
  Serial.begin(115200);
  pinMode(pulseA, INPUT_PULLUP);
  pinMode(pulseB, INPUT_PULLUP);
  attachInterrupt(pulseA, handleRotary, CHANGE);
  attachInterrupt(pulseB, handleRotary, CHANGE);

  Wire.begin(SDA_PIN, SCL_PIN);
  lightMeter.begin();
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("Lamp Control", 0, 20, 4);
  tft.drawString("Lux : ", 0, 60, 4);
  tft.drawString("Set : ", 0, 100, 4);
}

void loop() {
  float lux = lightMeter.readLightLevel();
  sprintf(lux_buf, "%.2f", lux);
  tft.fillRect(90, 60, 100, 30, TFT_BLACK);  
  tft.drawString(lux_buf, 90, 60, 4);  

  sprintf(rotary_buf, "%ld", encoderValue);
  tft.fillRect(90, 100, 100, 30, TFT_BLACK);  
  tft.drawString(rotary_buf, 90, 100, 4); 

  if (lux < encoderValue) {
    tft.fillCircle(240, 110, 50, TFT_ORANGE);
  } else {
    tft.fillCircle(240, 110, 50, TFT_BLACK); 
  }
  delay(500); 
}