#ifndef STT_MODULE_H
#define STT_MODULE_H

#include "config.h"

// Run speech-to-text on given audio buffer and produce a UTF-8 string.
// This function is mocked: replace the internals with actual API calls (HTTP/WebSocket or on-device
// recognizer). See stt_module.cpp for the mock implementation and the comment showing where to
// add real API integration.
String runSTT(AudioBuffer &buffer, Language lang);

#endif // STT_MODULE_H
