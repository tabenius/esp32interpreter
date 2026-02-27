# Multi-Translator (ESP32) — Svenska

Multi-Translator är ett firmwareprojekt för ESP32 som fångar tal från en I2S-mikrofon (eller Bluetooth), transkriberar (STT), översätter text till ett eller flera målspråk, genererar tal (TTS) och spelar upp ljud via I2S DAC eller Bluetooth. Projektet är gjort för Arduino IDE och innehåller mockade STT/Translation/TTS-moduler samt en Web UI för att ange API-nycklar och inställningar.

## Funktioner
- I2S-mikrofon (t.ex. INMP441)
- I2S DAC-utgång för 3.5 mm-uttag
- Bluetooth‑ljud (stubs; A2DP-integration kvar som TODO)
- OLED (SSD1306) och roterande encoder-meny
- Persistenta inställningar och API-nycklar i Preferences
- Soft‑AP Web UI för att ange OpenAI / Claude API-nycklar och runtime-inställningar

## Snabbstart (Arduino IDE)
1. Installera Arduino IDE (senaste stabila) och ESP32-stöd via Boards Manager.
2. Installera bibliotek via Library Manager:
   - ArduinoJson (version 6.x)
   - Adafruit SSD1306 och Adafruit GFX
3. Öppna katalogen som innehåller `main.ino` i Arduino IDE.
4. Välj rätt ESP32-kort (t.ex. "ESP32 Dev Module") och ladda upp.

## Hårdvaru‑anslutningar (standard)
- I2S-mikrofon: BCLK -> pin 14, LRCLK -> pin 15, DATA -> pin 32
- I2S DAC-utgång: BCLK -> pin 26, LRCLK -> pin 25, DATA -> pin 33
- OLED (SSD1306) I2C: SDA -> 21, SCL -> 22
- Rotary encoder: A -> 34, B -> 35, knapp -> 39
- Status LED -> pin 2

Ändra `config.h` vid behov.

## Web UI
Efter uppstart startar en Soft-AP med SSID `MultiTrans-XXXX`.
- Anslut till AP och öppna `http://192.168.4.1`.
- I Web UI kan du ange OpenAI/Claude-nycklar och ändra språk, volym, routing-läge med mera.
- Nycklar sparas i Preferences under namnutrymmet `multi_translator_keys`.

Varning: AP och Web UI är oskyddade (HTTP). Var försiktig med nycklar och nätverk.

## Integrera riktiga API:er
- STT: Byt ut `runSTT()` i `stt_module.cpp` mot en implementation som anropar din STT‑leverantör.
- Översättning: Uppdatera `translateText()` i `translation_module.cpp`.
- TTS: Uppdatera `synthesizeSpeech()` i `tts_module.cpp` för att använda verklig TTS.
- Exempelhjälpare: `ai_api_clients.h/.cpp` visar hur OpenAI / Anthropic kan anropas från ESP32.

## Filer
Se `README.md` för en engelsk översikt och TODO-lista.


---
(Filerna innehåller mock‑implementationer; läsa kommentarerna i koden för var API‑anrop ska läggas in.)
