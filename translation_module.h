#ifndef TRANSLATION_MODULE_H
#define TRANSLATION_MODULE_H

#include "config.h"
#include <vector>

// Translate text from 'from' to 'to'.
// This is a mock implementation — replace with calls to an external translation API.
String translateText(const String &input, Language from, Language to);

// Helper: translate to multiple targets
std::vector<String> translateMany(const String &input, Language from, const std::vector<Language> &targets);

#endif // TRANSLATION_MODULE_H
