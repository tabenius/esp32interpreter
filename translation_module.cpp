#include "translation_module.h"
#include "core_system.h"
#include <Arduino.h>

String translateText(const String &input, Language from, Language to) {
  // Mock translation: simply append a tag indicating target language.
  // Replace this block with an actual translation API call (e.g., Google Translate, DeepL, or
  // a self-hosted model). Typical integration steps:
  // 1. Prepare HTTP request body (JSON) with source text, source language, target language.
  // 2. Use WiFiClient/HTTPClient to POST to the translation endpoint.
  // 3. Parse JSON response and return translated text.

  String out = input;
  out += " [translated to ";
  out += languageToString(to);
  out += "]";
  return out;
}

std::vector<String> translateMany(const String &input, Language from, const std::vector<Language> &targets) {
  std::vector<String> results;
  for (Language t : targets) {
    results.push_back(translateText(input, from, t));
  }
  return results;
}
