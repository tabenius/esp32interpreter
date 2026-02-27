#include "settings_storage.h"
#include "core_system.h"
#include <Preferences.h>

AppSettings appSettings; // default-initialized

void loadSettings(AppSettings &s) {
  Preferences prefs;
  prefs.begin("multi_translator", true);
  s.inputLanguage = (Language)prefs.getUChar("inputLang", (uint8_t)s.inputLanguage);
  s.outputLanguage = (Language)prefs.getUChar("outputLang", (uint8_t)s.outputLanguage);
  s.hubLanguage = (Language)prefs.getUChar("hubLang", (uint8_t)s.hubLanguage);
  s.inputSource = (InputSource)prefs.getUChar("inputSrc", (uint8_t)s.inputSource);
  s.outputDevice = (OutputDevice)prefs.getUChar("outputDev", (uint8_t)s.outputDevice);
  s.volume = prefs.getInt("volume", s.volume);
  s.autoDetectLanguage = prefs.getBool("autoDetect", s.autoDetectLanguage);
  s.routingMode = (RoutingMode)prefs.getUChar("routing", (uint8_t)s.routingMode);
  prefs.end();

  // copy into global
  appSettings = s;
  logInfo("Settings loaded: in=%s out=%s vol=%d", languageToString(s.inputLanguage), languageToString(s.outputLanguage), s.volume);
}

void saveSettings(const AppSettings &s) {
  Preferences prefs;
  prefs.begin("multi_translator", false);
  prefs.putUChar("inputLang", (uint8_t)s.inputLanguage);
  prefs.putUChar("outputLang", (uint8_t)s.outputLanguage);
  prefs.putUChar("hubLang", (uint8_t)s.hubLanguage);
  prefs.putUChar("inputSrc", (uint8_t)s.inputSource);
  prefs.putUChar("outputDev", (uint8_t)s.outputDevice);
  prefs.putInt("volume", s.volume);
  prefs.putBool("autoDetect", s.autoDetectLanguage);
  prefs.putUChar("routing", (uint8_t)s.routingMode);
  prefs.end();

  // update global
  appSettings = s;
  logInfo("Settings saved");
}

// API key storage
void saveAPIKeys(const String &openaiKey, const String &claudeKey) {
  Preferences prefs;
  prefs.begin("multi_translator_keys", false);
  prefs.putString("openai", openaiKey);
  prefs.putString("claude", claudeKey);
  prefs.end();
  logInfo("API keys saved (lengths: %u, %u)", openaiKey.length(), claudeKey.length());
}

void loadAPIKeys(String &openaiKey, String &claudeKey) {
  Preferences prefs;
  prefs.begin("multi_translator_keys", true);
  openaiKey = prefs.getString("openai", "");
  claudeKey = prefs.getString("claude", "");
  prefs.end();
}

String getOpenAIKey() {
  String k;
  String dummy;
  loadAPIKeys(k, dummy);
  return k;
}

String getClaudeKey() {
  String k;
  String dummy;
  loadAPIKeys(dummy, k);
  return k;
}

// Wi-Fi credentials storage
void saveWiFiCredentials(const String &ssid, const String &password) {
  Preferences prefs;
  prefs.begin("multi_translator_wifi", false);
  prefs.putString("ssid", ssid);
  prefs.putString("pass", password);
  prefs.end();
  logInfo("WiFi credentials saved (ssid length=%u, pass length=%u)", ssid.length(), password.length());
}

void loadWiFiCredentials(String &ssid, String &password) {
  Preferences prefs;
  prefs.begin("multi_translator_wifi", true);
  String def_ssid = String(DEFAULT_WIFI_SSID);
  String def_pass = String(DEFAULT_WIFI_PASSWORD);
  ssid = prefs.getString("ssid", def_ssid);
  password = prefs.getString("pass", def_pass);
  prefs.end();
}

String getWiFiSSID() {
  String s, p;
  loadWiFiCredentials(s, p);
  return s;
}

String getWiFiPassword() {
  String s, p;
  loadWiFiCredentials(s, p);
  return p;
}
