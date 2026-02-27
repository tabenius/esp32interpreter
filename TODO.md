# TODO / Roadmap

This TODO contains high-level tasks and development items for the Multi-Translator project.

1. Integrate real STT (high priority)
   - Choose provider: OpenAI Whisper / OpenAI speech API, Google Speech-to-Text, Vosk (on-device) or other.
   - Implement streaming or chunked upload from `audio_input` to the STT client.
   - Use `ai_api_clients.h` helper or create a dedicated client.

2. Integrate real Translation service
   - Use OpenAI/chat completions, Google Translate API, DeepL or Claude.
   - Implement `translation_module.cpp` to call the chosen API and parse responses.
   - Add language autodetection support via API or local heuristics.

3. Integrate TTS
   - Choose TTS provider (OpenAI, Google, Amazon Polly, local TTS).
   - Replace mock `synthesizeSpeech()` to download WAV/PCM and fill `AudioBuffer`.

4. Bluetooth improvements
   - Implement A2DP Sink/Source for receiving and sending audio over Bluetooth.
   - Allow Bluetooth as input/source selection from UI.

5. UI and UX
   - Full rotary-encoder-based menu for all settings and history browsing.
   - Improve OLED layout and pagination of history items.
   - Add OTA update support.

6. Security
   - Protect Web UI (add passphrase to AP or use station mode and secure endpoint)
   - Store API keys securely and consider encrypting keys at rest
   - Replace `client.setInsecure()` with certificate pinning or root CA validation.

7. Performance & Stability
   - Add watchdog petting in tasks where necessary
   - Tune FreeRTOS priorities and stack sizes for stability
   - Memory optimizations for large audio buffers

8. Testing
   - Unit tests for routing logic, history storage and setting persistence
   - End‑to‑end test with one real STT and TTS provider

9. Documentation
   - Expand README with wiring diagrams and photos
   - Add example scripts for batch translation


Refer to `README.md` for compilation and Web UI usage instructions.
