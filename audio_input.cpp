#include "audio_input.h"
#include "core_system.h"
#include "settings_storage.h"

#include <Arduino.h>
#include <driver/i2s.h>

static const i2s_port_t I2S_PORT = (i2s_port_t)0;
static float inputGain = 1.0f;
static InputSource currentSource = InputSource::Mic;

static void audioTask(void *param) {
  (void)param;
  logInfo("Audio input task started");

  // Configure I2S for microphone (PDM/PCM depending on microphone)
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = AUDIO_SAMPLE_RATE,
    .bits_per_sample = (i2s_bits_per_sample_t)AUDIO_BITS_PER_SAMPLE,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = 0,
    .dma_buf_count = 4,
    .dma_buf_len = 512,
    .use_apll = false
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_MIC_BCLK_PIN,
    .ws_io_num = I2S_MIC_LRCLK_PIN,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_DATA_PIN
  };
  i2s_set_pin(I2S_PORT, &pin_config);

  // Temporary buffer (bytes)
  size_t bytes_read = 0;
  const int bufBytes = AUDIO_BUFFER_BYTES;
  uint8_t *i2s_rb = (uint8_t*)malloc(bufBytes);
  if (!i2s_rb) {
    logError("Failed to allocate I2S read buffer");
    vTaskDelete(NULL);
    return;
  }

  while (true) {
    if (currentSource == InputSource::Mic) {
      esp_err_t res = i2s_read(I2S_PORT, i2s_rb, bufBytes, &bytes_read, portMAX_DELAY);
      if (res == ESP_OK && bytes_read > 0) {
        // Convert bytes to int16_t samples
        int samples = bytes_read / 2;
        AudioBuffer *buf = new AudioBuffer();
        buf->samples.resize(samples);
        for (int i = 0; i < samples; ++i) {
          int16_t s = (int16_t)((i2s_rb[2*i+1] << 8) | i2s_rb[2*i]);
          // apply gain
          int32_t v = (int32_t)(s * inputGain);
          if (v > INT16_MAX) v = INT16_MAX;
          if (v < INT16_MIN) v = INT16_MIN;
          buf->samples[i] = (int16_t)v;
        }
        buf->sampleRate = AUDIO_SAMPLE_RATE;
        buf->channels = AUDIO_CHANNELS;

        // Enqueue to pipeline
        if (audioQueue) {
          if (xQueueSend(audioQueue, &buf, pdMS_TO_TICKS(50)) != pdTRUE) {
            logError("audioInput: queue full, dropping buffer");
            delete buf;
          }
        } else {
          delete buf;
        }
      }
    } else {
      // TODO: Implement Bluetooth audio input (A2DP Source) integration here.
      // For now, sleep briefly and loop.
      vTaskDelay(pdMS_TO_TICKS(50));
    }
  }

  // cleanup
  free(i2s_rb);
  vTaskDelete(NULL);
}

void audioInputInit() {
  // Nothing heavyweight here; the I2S driver is installed in task
  pinMode(I2S_MIC_BCLK_PIN, INPUT);
  pinMode(I2S_MIC_LRCLK_PIN, INPUT);
  pinMode(I2S_MIC_DATA_PIN, INPUT);
  logInfo("Audio input initialized (pins: bclk=%d lr=%d data=%d)", I2S_MIC_BCLK_PIN, I2S_MIC_LRCLK_PIN, I2S_MIC_DATA_PIN);
}

void audioInputStart() {
  BaseType_t r = xTaskCreatePinnedToCore(audioTask, "audioInputTask", 8*1024, NULL, 2, NULL, 1);
  if (r != pdPASS) {
    logError("Failed to create audio input task");
  }
}

void audioSetGain(float g) {
  inputGain = g;
}

void audioSetInputSource(InputSource s) {
  currentSource = s;
}
