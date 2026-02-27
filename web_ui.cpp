#include "web_ui.h"
#include "settings_storage.h"
#include "core_system.h"

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

static WebServer server(80);

static const char index_html[] = R"rawliteral(
<!doctype html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <title>MultiTranslator Setup</title>
  <style>
    body { font-family: Arial, Helvetica, sans-serif; margin: 12px; }
    label { display:block; margin:8px 0; }
    input, select { width:100%; padding:6px; margin-top:4px; }
    button { padding:8px 12px; margin-top:10px; }
    .row { display:flex; gap:8px; }
  </style>
</head>
<body>
  <h2>MultiTranslator Configuration</h2>
  <div>
    <label>OpenAI API Key
      <input id="openaiKey" type="password" placeholder="sk-...">
    </label>
    <label>Claude API Key
      <input id="claudeKey" type="password" placeholder="claude-key">
    </label>
    <label>Wi-Fi SSID (for API connectivity)
      <input id="wifiSSID" type="text" placeholder="MyWiFiSSID">
    </label>
    <label>Wi-Fi Password
      <input id="wifiPass" type="password" placeholder="supersecret">
    </label>
    <label>Input Language
      <select id="inputLanguage"></select>
    </label>
    <label>Output Language
      <select id="outputLanguage"></select>
    </label>
    <label>Hub Language
      <select id="hubLanguage"></select>
    </label>
    <label>Routing Mode
      <select id="routingMode"><option value="0">One-to-One</option><option value="1">One-to-Many</option><option value="2">Hub</option></select>
    </label>
    <label>Input Source
      <select id="inputSource"><option value="0">Mic</option><option value="1">Bluetooth</option></select>
    </label>
    <label>Output Device
      <select id="outputDevice"><option value="0">I2S DAC</option><option value="1">Bluetooth</option></select>
    </label>
    <label>Volume <span id="volval">75</span>
      <input id="volume" type="range" min="0" max="100" value="75" oninput="document.getElementById('volval').innerText=this.value">
    </label>
    <div class="row">
      <button onclick="saveConfig()">Save</button>
      <button onclick="resetAll()">Reset</button>
    </div>
    <div id="status" style="margin-top:10px;color:green"></div>
  </div>

<script>
function makeLangOptions(sel) {
  const langs = ['Swedish','English','Spanish','French','German','Auto'];
  for (let i=0;i<langs.length;i++){
    const o = document.createElement('option'); o.value = i; o.text = langs[i]; sel.add(o);
  }
}

function populateForm(cfg) {
  document.getElementById('openaiKey').value = cfg.openaiKey || '';
  document.getElementById('claudeKey').value = cfg.claudeKey || '';
  document.getElementById('wifiSSID').value = cfg.wifiSSID || '';
  document.getElementById('wifiPass').value = cfg.wifiPass || '';
  document.getElementById('inputLanguage').value = cfg.inputLanguage;
  document.getElementById('outputLanguage').value = cfg.outputLanguage;
  document.getElementById('hubLanguage').value = cfg.hubLanguage;
  document.getElementById('routingMode').value = cfg.routingMode;
  document.getElementById('inputSource').value = cfg.inputSource;
  document.getElementById('outputDevice').value = cfg.outputDevice;
  document.getElementById('volume').value = cfg.volume;
  document.getElementById('volval').innerText = cfg.volume;
}

function loadConfig(){
  fetch('/settings').then(r=>r.json()).then(cfg=>{ populateForm(cfg); document.getElementById('status').innerText='Loaded'; setTimeout(()=>{document.getElementById('status').innerText=''},2000); }).catch(e=>{document.getElementById('status').style.color='red'; document.getElementById('status').innerText='Failed to load';});
}

function saveConfig(){
  const payload = {
    openaiKey: document.getElementById('openaiKey').value,
    claudeKey: document.getElementById('claudeKey').value,
    wifiSSID: document.getElementById('wifiSSID').value,
    wifiPass: document.getElementById('wifiPass').value,
    inputLanguage: parseInt(document.getElementById('inputLanguage').value),
    outputLanguage: parseInt(document.getElementById('outputLanguage').value),
    hubLanguage: parseInt(document.getElementById('hubLanguage').value),
    routingMode: parseInt(document.getElementById('routingMode').value),
    inputSource: parseInt(document.getElementById('inputSource').value),
    outputDevice: parseInt(document.getElementById('outputDevice').value),
    volume: parseInt(document.getElementById('volume').value)
  };
  fetch('/save', { method: 'POST', headers: {'Content-Type':'application/json'}, body: JSON.stringify(payload) })
    .then(r=>r.json()).then(j=>{ if (j.ok){ document.getElementById('status').style.color='green'; document.getElementById('status').innerText='Saved'; setTimeout(()=>{document.getElementById('status').innerText=''},1500);} else {document.getElementById('status').style.color='red'; document.getElementById('status').innerText='Save failed';}})
    .catch(e=>{ document.getElementById('status').style.color='red'; document.getElementById('status').innerText='Save error'; });
}

function resetAll(){
  if (!confirm('Reset API keys and settings to defaults?')) return;
  fetch('/reset', { method: 'POST' }).then(r=>r.json()).then(j=>{ if (j.ok){ document.getElementById('status').style.color='green'; document.getElementById('status').innerText='Reset'; setTimeout(()=>{document.getElementById('status').innerText=''},1500); loadConfig(); } else {document.getElementById('status').style.color='red'; document.getElementById('status').innerText='Reset failed';}}).catch(e=>{document.getElementById('status').style.color='red'; document.getElementById('status').innerText='Reset error';});
}

window.addEventListener('load', function(){
  makeLangOptions(document.getElementById('inputLanguage'));
  makeLangOptions(document.getElementById('outputLanguage'));
  makeLangOptions(document.getElementById('hubLanguage'));
  loadConfig();
});
</script>
</body>
</html>
)rawliteral";

static void handleRoot() {
  server.send(200, "text/html", index_html);
}

static void handleSettings() {
  DynamicJsonDocument doc(512);
  doc["inputLanguage"] = (uint8_t)appSettings.inputLanguage;
  doc["outputLanguage"] = (uint8_t)appSettings.outputLanguage;
  doc["hubLanguage"] = (uint8_t)appSettings.hubLanguage;
  doc["routingMode"] = (uint8_t)appSettings.routingMode;
  doc["inputSource"] = (uint8_t)appSettings.inputSource;
  doc["outputDevice"] = (uint8_t)appSettings.outputDevice;
  doc["volume"] = appSettings.volume;
  String openai, claude;
  loadAPIKeys(openai, claude);
  doc["openaiKey"] = openai;
  doc["claudeKey"] = claude;
  // Wi-Fi creds
  String wifi_ssid, wifi_pass;
  loadWiFiCredentials(wifi_ssid, wifi_pass);
  doc["wifiSSID"] = wifi_ssid;
  doc["wifiPass"] = wifi_pass;
  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

static void handleSave() {
  String body = server.arg("plain");
  DynamicJsonDocument doc(1024);
  DeserializationError err = deserializeJson(doc, body);
  if (err) { server.send(400, "application/json", "{\"ok\":false,\"err\":\"badjson\"}"); return; }

  String openaiKey = doc["openaiKey"] | "";
  String claudeKey = doc["claudeKey"] | "";

  AppSettings s = appSettings;
  if (doc.containsKey("inputLanguage")) s.inputLanguage = (Language)(uint8_t)doc["inputLanguage"];
  if (doc.containsKey("outputLanguage")) s.outputLanguage = (Language)(uint8_t)doc["outputLanguage"];
  if (doc.containsKey("hubLanguage")) s.hubLanguage = (Language)(uint8_t)doc["hubLanguage"];
  if (doc.containsKey("routingMode")) s.routingMode = (RoutingMode)(uint8_t)doc["routingMode"];
  if (doc.containsKey("inputSource")) s.inputSource = (InputSource)(uint8_t)doc["inputSource"];
  if (doc.containsKey("outputDevice")) s.outputDevice = (OutputDevice)(uint8_t)doc["outputDevice"];
  if (doc.containsKey("volume")) s.volume = (int)doc["volume"];

  String wifiSSID = doc["wifiSSID"] | "";
  String wifiPass = doc["wifiPass"] | "";
  saveAPIKeys(openaiKey, claudeKey);
  saveWiFiCredentials(wifiSSID, wifiPass);
  saveSettings(s);

  server.send(200, "application/json", "{\"ok\":true}");
}

static void handleReset() {
  saveAPIKeys("", "");
  AppSettings s; // defaults
  saveSettings(s);
  server.send(200, "application/json", "{\"ok\":true}");
}

static void webTask(void *param) {
  (void)param;
  // Start in AP+STA mode so the device can both host configuration UI and connect to user's Wi-Fi
  WiFi.mode(WIFI_MODE_APSTA);

  // Attempt to connect as a station if Wi-Fi credentials are stored
  String saved_ssid, saved_pass;
  loadWiFiCredentials(saved_ssid, saved_pass);
  if (saved_ssid.length() > 0) {
    logInfo("Attempting STA connect to WiFi SSID: %s", saved_ssid.c_str());
    WiFi.begin(saved_ssid.c_str(), saved_pass.c_str());
    int maxWait = 100; // 100 * 100ms = 10s
    int waited = 0;
    while (WiFi.status() != WL_CONNECTED && waited < maxWait) {
      vTaskDelay(pdMS_TO_TICKS(100));
      waited++;
    }
    if (WiFi.status() == WL_CONNECTED) {
      IPAddress stIp = WiFi.localIP();
      logInfo("Connected to WiFi (STA): ssid=%s ip=%s", saved_ssid.c_str(), stIp.toString().c_str());
    } else {
      logInfo("WiFi STA connect failed for ssid=%s", saved_ssid.c_str());
    }
  }

  // Always start soft AP for configuration portal with unique SSID
  uint64_t chipid = ESP.getEfuseMac();
  String ap_ssid = String("MultiTrans-") + String((uint32_t)(chipid & 0xFFFF), HEX);
  WiFi.softAP(ap_ssid.c_str());
  IPAddress ip = WiFi.softAPIP();
  logInfo("Web UI AP started: %s ip=%s", ap_ssid.c_str(), ip.toString().c_str());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/settings", HTTP_GET, handleSettings);
  server.on("/save", HTTP_POST, handleSave);
  server.on("/reset", HTTP_POST, handleReset);
  server.onNotFound([](){ server.send(404, "text/plain", "Not found"); });
  server.begin();

  while (true) {
    server.handleClient();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void webUIStart() {
  BaseType_t r = xTaskCreatePinnedToCore(webTask, "webTask", 8*1024, NULL, 1, NULL, 1);
  if (r != pdPASS) {
    logError("Failed to create webTask");
  }
}
