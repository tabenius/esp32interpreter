#ifndef TTS_MODULE_H
#define TTS_MODULE_H

#include "config.h"

// Synthesize speech for the given text and language into out buffer.
// Mock implementation provided; replace with a real TTS API call.
bool synthesizeSpeech(const String &text, Language lang, AudioBuffer &out);

#endif // TTS_MODULE_H
