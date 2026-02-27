#include "history_storage.h"
#include "core_system.h"
#include <Preferences.h>

static const char *NAMESPACE = "multi_hist";

static String escapeStr(const String &s) {
  String r;
  for (unsigned int i = 0; i < s.length(); ++i) {
    char c = s[i];
    if (c == '|') { r += "\\|"; }
    else r += c;
  }
  return r;
}

static String unescapeStr(const String &s) {
  String r;
  for (unsigned int i = 0; i < s.length(); ++i) {
    char c = s[i];
    if (c == '\\' && i + 1 < s.length() && s[i+1] == '|') { r += '|'; ++i; }
    else r += c;
  }
  return r;
}

void addHistoryEntry(const HistoryEntry &e) {
  Preferences prefs;
  prefs.begin(NAMESPACE, false);
  int head = prefs.getInt("head", 0);
  int count = prefs.getInt("count", 0);

  int idx = (head) % HISTORY_MAX_ENTRIES;
  // serialize: timestamp|from|to|original|translated
  String key = String("hist_") + String(idx);
  String payload = String(e.timestamp) + "|" + String((uint8_t)e.from) + "|" + String((uint8_t)e.to) + "|" + escapeStr(e.original) + "|" + escapeStr(e.translated);
  prefs.putString(key.c_str(), payload);

  head = (head + 1) % HISTORY_MAX_ENTRIES;
  if (count < HISTORY_MAX_ENTRIES) ++count;
  prefs.putInt("head", head);
  prefs.putInt("count", count);
  prefs.end();
}

size_t getHistoryCount() {
  Preferences prefs;
  prefs.begin(NAMESPACE, true);
  int count = prefs.getInt("count", 0);
  prefs.end();
  return (size_t)count;
}

bool getHistoryEntry(size_t index, HistoryEntry &out) {
  Preferences prefs;
  prefs.begin(NAMESPACE, true);
  int head = prefs.getInt("head", 0);
  int count = prefs.getInt("count", 0);
  if (index >= (size_t)count) { prefs.end(); return false; }
  // oldest entry index calculation
  int oldest = (head - count + HISTORY_MAX_ENTRIES) % HISTORY_MAX_ENTRIES;
  int idx = (oldest + index) % HISTORY_MAX_ENTRIES;
  String key = String("hist_") + String(idx);
  String payload = prefs.getString(key.c_str(), "");
  prefs.end();
  if (payload.length() == 0) return false;
  // parse
  int p1 = payload.indexOf('|');
  int p2 = payload.indexOf('|', p1+1);
  int p3 = payload.indexOf('|', p2+1);
  int p4 = payload.indexOf('|', p3+1);
  if (p1 < 0 || p2 < 0 || p3 < 0 || p4 < 0) return false;
  out.timestamp = (uint32_t)payload.substring(0,p1).toInt();
  out.from = (Language)payload.substring(p1+1,p2).toInt();
  out.to = (Language)payload.substring(p2+1,p3).toInt();
  out.original = unescapeStr(payload.substring(p3+1,p4));
  out.translated = unescapeStr(payload.substring(p4+1));
  return true;
}
