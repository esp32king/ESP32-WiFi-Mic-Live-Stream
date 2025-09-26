# 🎤 ESP32 Waveshare Sound Sensor Audio Streaming

Stream audio from a Waveshare Sound Sensor connected to an ESP32 over WiFi using WebSockets. View and play audio directly in a web browser!

## ⚡ Features

- Real-time audio streaming via WebSocket
- Works with Waveshare Sound Sensor
- Plays audio in the browser using Web Audio API
- Simple HTML interface with start/stop buttons
- Sample rate: 16 kHz mono

## 🛠️ Components Required

<br>| Component | Description |</br>
|-----------|-------------|
<br>| ESP32 Dev Board | Main microcontroller |</br>
| Waveshare Sound Sensor | Audio input (analog) |
<br>| Jumper wires | For connections |</br>
| USB Cable | For programming ESP32 |

## 🔌 Wiring Diagram

Waveshare Sound Sensor ESP32

<br>VCC 3.3V</br>
<br>GND GND</br>
A0 (Analog Out) GPIO 33

vbnet
Copy code

> Note: `SOUND_PIN` in code is set to GPIO 33. Change if you wire differently.

## 💻 Setup Instructions

1. Install Arduino IDE with ESP32 support.
2. Install required libraries:
   - ESPAsyncWebServer
   - AsyncTCP
3. Copy the Arduino sketch into IDE.
4. Set your WiFi SSID and password in the code:

``cpp
const char* WIFI_SSID = "Your_SSID";
const char* WIFI_PASS = "Your_PASSWORD";``
## Upload the code to ESP32.

Open Serial Monitor to see the ESP32 IP address.

Open browser and navigate to http://<ESP32_IP>/

## 🌐 How to Use
Click Start to begin audio streaming.

Click Stop to end streaming.

Browser will play audio in real-time.

## ⚙️ How It Works
ESP32 reads analog audio signal from Waveshare sensor.

Converts raw signal (0–4095) to 16-bit PCM audio.

Sends audio chunks via WebSocket to connected browser clients.

Browser receives PCM data and plays using Web Audio API.

## 📝 Notes
Only works on local network (WiFi).

Sampling rate: 16 kHz, mono audio.

Delay and buffer size can be adjusted in scriptNode (browser) or READ_SAMPLES (ESP32).

## 📸 Example Screenshot
sql
Copy code
``+---------------------------+
| ESP32 Waveshare Sound Stream |
+---------------------------+
| Status: Connecting...      |
| [Start] [Stop]            |
+---------------------------+``
## 🔗 References
Waveshare Sound Sensor

ESPAsyncWebServer GitHub

## 😎 Author
Krishna Rajput UP61
