#include "stt_module.h"
#include "core_system.h"
#include <Arduino.h>

String runSTT(AudioBuffer &buffer, Language lang) {
  // Mock implementation:
  // - If there is very low energy in the buffer, treat as silence and return empty string.
  // - Otherwise return a canned sentence depending on language.
  // Replace the body of this function with actual STT API integration. Example places to insert code:
  //  * Record MP3/WAV from buffer and POST to cloud STT (use HTTPClient)
  //  * Use an on-device model (if available) and call its API
  //  * Use WebSocket based streaming APIs

  // Simple energy check
  uint64_t acc = 0;
  for (size_t i = 0; i < buffer.samples.size(); ++i) {
    int32_t s = buffer.samples[i];
    acc += (uint64_t)(abs(s));
  }
  double avg = 0.0;
  if (!buffer.samples.empty()) avg = (double)acc / buffer.samples.size();

  if (avg < 50.0) {
    // considered silence
    return String("");
  }

  // Mock recognized text based on language hint
  if (lang == Language::Swedish) return String("Detta är ett test");
  if (lang == Language::English) return String("This is a test");
  if (lang == Language::Spanish) return String("Esto es una prueba");
  if (lang == Language::French)  return String("Ceci est un test");
  if (lang == Language::German)  return String("Dies ist ein Test");

  // Auto / default
  return String("This is a test (auto)");
}
