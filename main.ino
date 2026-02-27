#include "config.h"
#include "core_system.h"
#include "audio_input.h"
#include "audio_output.h"
#include "stt_module.h"
#include "translation_module.h"
#include "tts_module.h"
#include "ui_oled.h"
#include "settings_storage.h"
#include "history_storage.h"
#include "routing_engine.h"
#include "web_ui.h"

void setup() {
  coreInit();

  // Load settings
  AppSettings s;
  loadSettings(s);

  // Initialize peripherals & modules
  uiInit();
  audioOutputInit();
  audioInputInit();

  // Start background tasks and services
  startSystemTasks();
  audioOutputStart();
  audioInputStart();

  // Start web configuration UI (AP + HTTP server)
  webUIStart();

  uiSetStatus("System ready");
}

void loop() {
  // UI handling on the main loop (lightweight)
  uiUpdate();
  delay(10);
}
