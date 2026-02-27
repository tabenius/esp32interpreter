#include "core_system.h"
#include "stt_module.h"
#include "translation_module.h"
#include "tts_module.h"
#include "routing_engine.h"
#include "history_storage.h"
#include "settings_storage.h"
#include "audio_output.h"

#include <Arduino.h>
#include <Wire.h>
#include <esp_task_wdt.h>
#include <stdarg.h>

QueueHandle_t audioQueue = nullptr;
QueueHandle_t ttsQueue = nullptr;

static void pipelineTask(void *param) {
  (void)param;
  logInfo("Pipeline task started");
  for (;;) {
    AudioBuffer *inBuf = nullptr;
    if (xQueueReceive(audioQueue, &inBuf, portMAX_DELAY) == pdTRUE) {
      if (!inBuf) continue;
      logInfo("Pipeline: received audio buffer (samples=%u)", (unsigned)inBuf->samples.size());

      // Run STT (mocked)
      Language inputLang = appSettings.inputLanguage;
      if (appSettings.autoDetectLanguage) inputLang = Language::Auto;
      String recognized = runSTT(*inBuf, inputLang);

      if (recognized.length() > 0) {
        logInfo("STT: %s", recognized.c_str());
        // Determine routing targets
        std::vector<Language> targets = determineTargets(appSettings.inputLanguage, appSettings.outputLanguage, appSettings.routingMode, appSettings.hubLanguage);

        // For each target language, translate -> TTS -> enqueue for playback
        for (Language target : targets) {
          // If hub mode requires intermediate translation, routing engine already handles returned list semantics
          String translated = translateText(recognized, appSettings.inputLanguage, target);
          logInfo("Translated (%s->%s): %s", languageToString(appSettings.inputLanguage), languageToString(target), translated.c_str());

          // Synthesize
          AudioBuffer *outBuf = new AudioBuffer();
          if (synthesizeSpeech(translated, target, *outBuf)) {
            // attach metadata? not necessary for playback
            // enqueue for playback
            if (xQueueSend(ttsQueue, &outBuf, pdMS_TO_TICKS(100)) != pdTRUE) {
              logError("Failed to enqueue TTS buffer");
              delete outBuf;
            }
          } else {
            logError("TTS failed for language %s", languageToString(target));
            delete outBuf;
          }

          // Store history (best-effort)
          HistoryEntry he;
          he.timestamp = (uint32_t)time(nullptr);
          he.from = appSettings.inputLanguage;
          he.to = target;
          he.original = recognized;
          he.translated = translated;
          addHistoryEntry(he);
        }
      } else {
        logInfo("STT produced no text (silence or unrecognized)");
      }

      // free input buffer
      delete inBuf;
    }
  }
}

void coreInit() {
  Serial.begin(115200);
  delay(20);
  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  // Initialize watchdog with 10s timeout; tasks should pet if needed
  esp_task_wdt_init(10, true);
  esp_task_wdt_add(NULL); // add current task (loop)

  logInfo("Core initialized");
}

void startSystemTasks() {
  // Create queues
  audioQueue = xQueueCreate(6, sizeof(AudioBuffer*));
  ttsQueue = xQueueCreate(6, sizeof(AudioBuffer*));

  if (!audioQueue || !ttsQueue) {
    logError("Failed to create queues");
    return;
  }

  // Start pipeline task
  BaseType_t r = xTaskCreatePinnedToCore(pipelineTask, "pipelineTask", 12 * 1024, NULL, 1, NULL, 1);
  if (r != pdPASS) {
    logError("Failed to start pipeline task");
  } else {
    logInfo("Pipeline task created");
  }

  // Let other modules create their own tasks (audio input / output)
}

void logInfo(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  Serial.print("[INFO] ");
  Serial.vprintf(fmt, ap);
  Serial.println();
  va_end(ap);
}

void logError(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  Serial.print("[ERROR] ");
  Serial.vprintf(fmt, ap);
  Serial.println();
  va_end(ap);
}

