// Created By Krishna Rajput UP61
// Waveshare Sound Sensor + ESP32 + WebSocket Audio Streaming

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* WIFI_SSID = "Your_SSID";
const char* WIFI_PASS = "Your_PASSWORD";

#define SOUND_PIN 33      // Analog input pin for Sound Sensor
#define SAMPLE_RATE 16000 // Sample rate for audio

AsyncWebServer server(80);
AsyncWebSocket ws("/audio");

// HTML Page
const char index_html[] PROGMEM = R"rawliteral(
<!doctype html>
<html>
<head>
  <meta charset="utf-8"/>
  <title>ESP32 Sound Stream</title>
</head>
<body>
  <h2>ESP32 Waveshare Sound Stream</h2>
  <p id="status">Connecting...</p>
  <button id="startBtn">Start</button>
  <button id="stopBtn" disabled>Stop</button>
  <script>
    let socket;
    let audioCtx;
    let scriptNode;
    const sampleRate = 16000;
    const statusEl = document.getElementById('status');
    const startBtn = document.getElementById('startBtn');
    const stopBtn = document.getElementById('stopBtn');

    function setupAudio() {
      audioCtx = new (window.AudioContext || window.webkitAudioContext)({sampleRate: sampleRate});
      const bufferSize = 1024;
      scriptNode = audioCtx.createScriptProcessor(bufferSize, 1, 1);
      let queue = [];
      scriptNode.onaudioprocess = function(e) {
        const output = e.outputBuffer.getChannelData(0);
        if (queue.length === 0) { output.fill(0); return; }
        const src = queue.shift();
        output.set(src.subarray(0, output.length));
      };
      scriptNode.connect(audioCtx.destination);
      return { push: (samples) => queue.push(samples) };
    }

    let player;
    startBtn.onclick = () => {
      startBtn.disabled = true;
      stopBtn.disabled = false;
      statusEl.textContent = "Connecting websocket...";
      socket = new WebSocket((location.protocol === 'https:' ? 'wss://' : 'ws://') + location.host + "/audio");
      socket.binaryType = "arraybuffer";
      player = setupAudio();
      socket.onopen = () => statusEl.textContent = "Connected, streaming...";
      socket.onmessage = (evt) => {
        const view = new DataView(evt.data);
        const len = view.byteLength / 2;
        const float32 = new Float32Array(len);
        for (let i=0;i<len;i++) float32[i] = view.getInt16(i*2, true)/32768;
        for (let i=0;i<float32.length;i+=1024)
          player.push(float32.subarray(i, i+1024));
      };
      socket.onclose = () => {
        statusEl.textContent = "WebSocket closed";
        startBtn.disabled = false;
        stopBtn.disabled = true;
      };
      socket.onerror = () => statusEl.textContent = "WebSocket error";
    };

    stopBtn.onclick = () => {
      if (socket) socket.close();
      startBtn.disabled = false;
      stopBtn.disabled = true;
      statusEl.textContent = "Stopped";
    };
  </script>
</body>
</html>
)rawliteral";

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type,
               void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT) Serial.printf("Client %u connected\n", client->id());
  else if(type == WS_EVT_DISCONNECT) Serial.printf("Client %u disconnected\n", client->id());
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nConnected, IP: " + WiFi.localIP().toString());

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
  server.begin();
  Serial.println("Server started");
}

void loop() {
  if (ws.count() == 0) { delay(50); return; }

  const int READ_SAMPLES = 1024;
  static int16_t outbuf[READ_SAMPLES];
  
  for (int i = 0; i < READ_SAMPLES; i++) {
    int raw = analogRead(SOUND_PIN); // 0-4095 range
    raw -= 2048;                     // Center around 0
    outbuf[i] = (int16_t)(raw << 4); // Convert to 16-bit audio
    delayMicroseconds(1000000 / SAMPLE_RATE);
  }

  ws.binaryAll((uint8_t*)outbuf, READ_SAMPLES * sizeof(int16_t));
}
