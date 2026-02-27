#ifndef MULTI_TRANSLATOR_CONFIG_H
#define MULTI_TRANSLATOR_CONFIG_H

#include <Arduino.h>
#include <Preferences.h>
#include <vector>

// -----------------------
// Pin definitions
// -----------------------
// I2S microphone (e.g., INMP441)
#define I2S_MIC_BCLK_PIN 14
#define I2S_MIC_LRCLK_PIN 15
#define I2S_MIC_DATA_PIN 32

// I2S DAC output pins (for external I2S DAC)
#define I2S_DAC_BCLK_PIN 26
#define I2S_DAC_LRCLK_PIN 25
#define I2S_DAC_DATA_PIN 33

// OLED (SSD1306) I2C pins
#define OLED_SDA_PIN 21
#define OLED_SCL_PIN 22

// Rotary encoder pins
#define ROTARY_PIN_A 34
#define ROTARY_PIN_B 35
#define ROTARY_BTN_PIN 39

// Status LED
#define STATUS_LED_PIN 2

// -----------------------
// Audio constants
// -----------------------
#define AUDIO_SAMPLE_RATE 16000
#define AUDIO_CHANNELS 1
#define AUDIO_BITS_PER_SAMPLE 16
#define AUDIO_BUFFER_SAMPLES 1024
#define AUDIO_BUFFER_BYTES (AUDIO_BUFFER_SAMPLES * (AUDIO_BITS_PER_SAMPLE/8))

// History
#define HISTORY_MAX_ENTRIES 50

// -----------------------
// Enums and structs
// -----------------------

enum class Language : uint8_t {
  Swedish = 0,
  English = 1,
  Spanish = 2,
  French = 3,
  German = 4,
  Auto = 5
};

enum class InputSource : uint8_t {
  Mic = 0,
  Bluetooth = 1
};

enum class OutputDevice : uint8_t {
  I2S_DAC = 0,
  Bluetooth = 1
};

enum class RoutingMode : uint8_t {
  OneToOne = 0,
  OneToMany = 1,
  Hub = 2
};

struct AppSettings {
  Language inputLanguage = Language::Auto;
  Language outputLanguage = Language::English;
  Language hubLanguage = Language::English;
  InputSource inputSource = InputSource::Mic;
  OutputDevice outputDevice = OutputDevice::I2S_DAC;
  int volume = 75; // 0-100
  bool autoDetectLanguage = true;
  RoutingMode routingMode = RoutingMode::OneToOne;
};

struct AudioBuffer {
  std::vector<int16_t> samples; // mono 16-bit
  int sampleRate = AUDIO_SAMPLE_RATE;
  int channels = AUDIO_CHANNELS;
};

struct HistoryEntry {
  uint32_t timestamp;
  Language from;
  Language to;
  String original;
  String translated;
};

// -----------------------
// Helpers
// -----------------------
inline const char* languageToString(Language l) {
  switch (l) {
    case Language::Swedish: return "Swedish";
    case Language::English: return "English";
    case Language::Spanish: return "Spanish";
    case Language::French:  return "French";
    case Language::German:  return "German";
    case Language::Auto:    return "Auto";
    default: return "Unknown";
  }
}

inline Language stringToLanguage(const String &s) {
  if (s == "Swedish") return Language::Swedish;
  if (s == "English") return Language::English;
  if (s == "Spanish") return Language::Spanish;
  if (s == "French")  return Language::French;
  if (s == "German")  return Language::German;
  return Language::Auto;
}

#endif // MULTI_TRANSLATOR_CONFIG_H
