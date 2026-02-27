#include "ui_oled.h"
#include "settings_storage.h"
#include "history_storage.h"
#include "core_system.h"

#include <Arduino.h>
#if __has_include(<Adafruit_SSD1306.h>)
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
  #define USE_SSD1306
#endif

#ifdef USE_SSD1306
static Adafruit_SSD1306 display(128, 64, &Wire, -1);
#endif

static String statusLine = "Ready";
static int menuIndex = 0;
static unsigned long lastUpdate = 0;

void uiInit() {
#ifdef USE_SSD1306
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    logError("SSD1306 allocation failed");
  }
  display.clearDisplay();
  display.display();
  logInfo("OLED initialized");
#else
  logInfo("OLED not available; using Serial for UI");
#endif
}

static void drawMainScreen() {
#ifdef USE_SSD1306
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.printf("Input: %s\n", appSettings.inputSource == InputSource::Mic ? "Mic" : "Bluetooth");
  display.printf("InLang: %s\n", languageToString(appSettings.inputLanguage));
  display.printf("Output: %s Vol:%d\n", appSettings.outputDevice == OutputDevice::I2S_DAC ? "I2S" : "BT", appSettings.volume);
  display.printf("OutLang: %s\n", languageToString(appSettings.outputLanguage));
  display.printf("Mode: %s\n", (appSettings.routingMode==RoutingMode::OneToOne)?"1:1":(appSettings.routingMode==RoutingMode::OneToMany)?"1:Many":"Hub");
  display.setCursor(0,54);
  display.print(statusLine);
  display.display();
#else
  Serial.println("=== UI ===");
  Serial.printf("Input: %s\n", appSettings.inputSource == InputSource::Mic ? "Mic" : "Bluetooth");
  Serial.printf("InLang: %s\n", languageToString(appSettings.inputLanguage));
  Serial.printf("Output: %s Vol:%d\n", appSettings.outputDevice == OutputDevice::I2S_DAC ? "I2S" : "BT", appSettings.volume);
  Serial.printf("OutLang: %s\n", languageToString(appSettings.outputLanguage));
  Serial.printf("Mode: %s\n", (appSettings.routingMode==RoutingMode::OneToOne)?"1:1":(appSettings.routingMode==RoutingMode::OneToMany)?"1:Many":"Hub");
  Serial.println(statusLine);
#endif
}

void uiUpdate() {
  unsigned long now = millis();
  if (now - lastUpdate < 250) return;
  lastUpdate = now;
  drawMainScreen();
}

void uiSetStatus(const String &s) {
  statusLine = s;
}

