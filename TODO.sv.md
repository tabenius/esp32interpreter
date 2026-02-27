# TODO / Färdplan (Svenska)

Hög nivå av uppgifter och förbättringar för Multi-Translator.

1. Integrera riktig STT (hög prioritet)
   - Välj leverantör: OpenAI Whisper, Google STT, Vosk m.fl.
   - Implementera streaming eller uppladdning från `audio_input` till STT-klienten.

2. Integrera översättningstjänst
   - Använd OpenAI, Google, DeepL eller Claude.
   - Byt ut mock i `translation_module.cpp`.

3. Integrera TTS
   - Använd en TTS‑leverantör eller lokalt TTS
   - Byt ut `synthesizeSpeech()` så att den fyller `AudioBuffer` med riktig PCM/WAV

4. Bluetooth
   - Implementera A2DP sink/source
   - Gör Bluetooth till valbar input/output i UI

5. UI
   - Fullständig meny för encoder och historik
   - Förbättra OLED-utseende och stöd för pagination

6. Säkerhet
   - Lägg till lösenord eller krav för Web UI
   - Säkrare lagring av nycklar och TLS‑validering

7. Stabilitet & prestanda
   - Watchdog för uppgiftspetning
   - Optimera minne för stora ljudbuffrar

Se `README.sv.md` och `README.md` för mer information.
