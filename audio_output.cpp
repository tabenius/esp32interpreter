#include "audio_output.h"
#include "core_system.h"

#include <Arduino.h>
#include <driver/i2s.h>

static const i2s_port_t I2S_OUT_PORT = (i2s_port_t)1;
static int currentVolume = 75; // 0-100

static void ttsPlaybackTask(void *param) {
  (void)param;
  logInfo("TTS playback task started");
  while (true) {
    AudioBuffer *buf = nullptr;
    if (xQueueReceive(ttsQueue, &buf, portMAX_DELAY) == pdTRUE) {
      if (!buf) continue;

      // Convert samples to byte buffer and write to I2S
      size_t bytes_to_write = buf->samples.size() * sizeof(int16_t);
      size_t written = 0;
      // Simple volume scaling applied on-the-fly
      // We'll write in chunks to avoid huge allocations
      const size_t chunkSamples = 512;
      std::vector<int16_t> chunk;
      chunk.reserve(chunkSamples);

      size_t idx = 0;
      while (idx < buf->samples.size()) {
        size_t take = std::min((size_t)chunkSamples, buf->samples.size() - idx);
        chunk.clear();
        for (size_t k = 0; k < take; ++k) {
          int32_t v = buf->samples[idx + k];
          v = (v * currentVolume) / 100;
          if (v > INT16_MAX) v = INT16_MAX;
          if (v < INT16_MIN) v = INT16_MIN;
          chunk.push_back((int16_t)v);
        }

        size_t bytes = take * sizeof(int16_t);
        // i2s_write expects void* so cast
        size_t bytes_written = 0;
        esp_err_t res = i2s_write(I2S_OUT_PORT, chunk.data(), bytes, &bytes_written, portMAX_DELAY);
        if (res != ESP_OK) {
          logError("i2s_write failed: %d", res);
          break;
        }
        idx += take;
      }

      // playback finished
      delete buf;
    }
  }
}

void audioOutputInit() {
  // Configure I2S for DAC output (master, TX)
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = AUDIO_SAMPLE_RATE,
    .bits_per_sample = (i2s_bits_per_sample_t)AUDIO_BITS_PER_SAMPLE,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_MSB,
    .intr_alloc_flags = 0,
    .dma_buf_count = 6,
    .dma_buf_len = 512,
    .use_apll = false
  };

  i2s_driver_install(I2S_OUT_PORT, &i2s_config, 0, NULL);
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_DAC_BCLK_PIN,
    .ws_io_num = I2S_DAC_LRCLK_PIN,
    .data_out_num = I2S_DAC_DATA_PIN,
    .data_in_num = I2S_PIN_NO_CHANGE
  };
  i2s_set_pin(I2S_OUT_PORT, &pin_config);
  logInfo("Audio output initialized (I2S out)");
}

void audioOutputStart() {
  BaseType_t r = xTaskCreatePinnedToCore(ttsPlaybackTask, "ttsPlaybackTask", 8 * 1024, NULL, 2, NULL, 1);
  if (r != pdPASS) {
    logError("Failed to create TTS playback task");
  }
}

void audioSetVolume(int v) {
  currentVolume = constrain(v, 0, 100);
}

