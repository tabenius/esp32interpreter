#ifndef CORE_SYSTEM_H
#define CORE_SYSTEM_H

#include "config.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

// Queues used for inter-task communication
extern QueueHandle_t audioQueue; // AudioBuffer* from input -> pipeline
extern QueueHandle_t ttsQueue;   // AudioBuffer* from TTS -> output

void coreInit();
void startSystemTasks();

void logInfo(const char *fmt, ...);
void logError(const char *fmt, ...);

#endif // CORE_SYSTEM_H
