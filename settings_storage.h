#ifndef SETTINGS_STORAGE_H
#define SETTINGS_STORAGE_H

#include "config.h"

extern AppSettings appSettings; // runtime settings accessible by modules

void loadSettings(AppSettings &s);
void saveSettings(const AppSettings &s);

// API keys storage
void saveAPIKeys(const String &openaiKey, const String &claudeKey);
void loadAPIKeys(String &openaiKey, String &claudeKey);
String getOpenAIKey();
String getClaudeKey();

#endif // SETTINGS_STORAGE_H
