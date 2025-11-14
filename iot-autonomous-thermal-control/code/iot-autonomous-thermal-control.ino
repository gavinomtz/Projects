#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "notleakingmyIP";
const char* password = "notleakingmyPassword";

// Pin definitions
#define DHTPIN 25
#define DHTTYPE DHT11
#define RELAY_PIN 26

// FUNCTION DECLARATIONS
void handleRoot();
void handleData();
void handleControl();
void handleSettings();

// Control parameters
float TEMP_THRESHOLD = 22.0;  // Can be changed via web interface
const float HYSTERESIS = 1.0;
const unsigned long UPDATE_INTERVAL = 2000;  // 2 seconds

// Initialize devices
DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

// System state
bool fanOn = false;
bool manualOverride = false;
float currentTemp = 0.0;
float currentHumidity = 0.0;
unsigned long lastUpdate = 0;
unsigned long uptimeSeconds = 0;

// Data logging arrays (last 60 readings = 2 minutes at 2s interval)
#define DATA_POINTS 60
float tempHistory[DATA_POINTS];
int dataIndex = 0;

void setup() {
  Serial.begin(115200);
  
  // Initialize pins
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  
  // Initialize DHT sensor
  dht.begin();
  
  // Connect to WiFi
  connectWiFi();
  
  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/control", handleControl);
  server.on("/settings", handleSettings);
  server.begin();
  
  Serial.println("System ready!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();
  
  unsigned long currentMillis = millis();
  
  // Update sensor readings every 2 seconds
  if (currentMillis - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = currentMillis;
    uptimeSeconds += 2;
    
    readSensors();
    
    // Autonomous control (unless manual override)
    if (!manualOverride) {
      autonomousControl();
    }
    
    logData();
    printStatus();
  }
}

void connectWiFi() {
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    delay(3000);
  } else {
    Serial.println("\nWiFi Failed!");
  }
}

void readSensors() {
  currentTemp = dht.readTemperature();
  currentHumidity = dht.readHumidity();
  

}

void autonomousControl() {
  if (currentTemp == -999) return;  // Skip if sensor error
  
  if (currentTemp >= TEMP_THRESHOLD && !fanOn) {
    digitalWrite(RELAY_PIN, HIGH);
    fanOn = true;
    Serial.println(">>> FAN TURNED ON (AUTO) <<<");
  } 
  else if (currentTemp < (TEMP_THRESHOLD - HYSTERESIS) && fanOn) {
    digitalWrite(RELAY_PIN, LOW);
    fanOn = false;
    Serial.println(">>> FAN TURNED OFF (AUTO) <<<");
  }
}

void logData() {
  if (currentTemp != -999) {
    tempHistory[dataIndex] = currentTemp;
    dataIndex = (dataIndex + 1) % DATA_POINTS;
  }
}

void printStatus() {
  Serial.print("Temp: ");
  Serial.print(currentTemp);
  Serial.print("C | Humid: ");
  Serial.print(currentHumidity);
  Serial.print("% | Fan: ");
  Serial.print(fanOn ? "ON" : "OFF");
  if (manualOverride) Serial.print(" (MANUAL)");
  Serial.println();
}

// Web server handlers
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Thermal Control Dashboard</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
            color: #fff;
        }
        .container {
            max-width: 1200px;
            margin: 0 auto;
        }
        header {
            text-align: center;
            margin-bottom: 30px;
        }
        h1 {
            font-size: 2.5em;
            margin-bottom: 10px;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
        }
        .subtitle {
            font-size: 1.1em;
            opacity: 0.9;
        }
        .status-bar {
            background: rgba(255,255,255,0.1);
            backdrop-filter: blur(10px);
            border-radius: 15px;
            padding: 15px 25px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 25px;
            border: 1px solid rgba(255,255,255,0.2);
        }
        .status-item {
            display: flex;
            align-items: center;
            gap: 10px;
        }
        .status-dot {
            width: 12px;
            height: 12px;
            border-radius: 50%;
            animation: pulse 2s infinite;
        }
        .status-dot.online {
            background: #00ff88;
            box-shadow: 0 0 10px #00ff88;
        }
        .status-dot.offline {
            background: #ff4444;
            box-shadow: 0 0 10px #ff4444;
        }
        @keyframes pulse {
            0%, 100% { opacity: 1; }
            50% { opacity: 0.5; }
        }
        .dashboard-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
            margin-bottom: 20px;
        }
        .card {
            background: rgba(255,255,255,0.1);
            backdrop-filter: blur(10px);
            border-radius: 20px;
            padding: 25px;
            border: 1px solid rgba(255,255,255,0.2);
            box-shadow: 0 8px 32px rgba(0,0,0,0.1);
            transition: transform 0.3s ease;
        }
        .card:hover {
            transform: translateY(-5px);
        }
        .card-title {
            font-size: 1.2em;
            margin-bottom: 20px;
            display: flex;
            align-items: center;
            gap: 10px;
        }
        .card-icon {
            font-size: 1.5em;
        }
        .temp-display {
            font-size: 4em;
            font-weight: bold;
            text-align: center;
            margin: 20px 0;
            text-shadow: 2px 2px 8px rgba(0,0,0,0.3);
        }
        .temp-unit {
            font-size: 0.4em;
            opacity: 0.7;
        }
        .metric-row {
            display: flex;
            justify-content: space-between;
            padding: 15px;
            background: rgba(255,255,255,0.05);
            border-radius: 10px;
            margin-bottom: 10px;
        }
        .metric-label {
            opacity: 0.8;
        }
        .metric-value {
            font-weight: bold;
        }
        .fan-status {
            text-align: center;
            padding: 30px;
        }
        .fan-icon {
            font-size: 5em;
            margin-bottom: 20px;
            transition: all 0.3s ease;
        }
        .fan-icon.spinning {
            animation: spin 2s linear infinite;
        }
        @keyframes spin {
            from { transform: rotate(0deg); }
            to { transform: rotate(360deg); }
        }
        .fan-label {
            font-size: 1.5em;
            font-weight: bold;
            margin-bottom: 10px;
        }
        .fan-label.on {
            color: #00ff88;
            text-shadow: 0 0 10px #00ff88;
        }
        .fan-label.off {
            color: #ff4444;
        }
        .control-buttons {
            display: flex;
            flex-direction: column;
            gap: 15px;
            margin-top: 20px;
        }
        .btn {
            padding: 15px 30px;
            border: none;
            border-radius: 12px;
            font-size: 1.1em;
            font-weight: bold;
            cursor: pointer;
            transition: all 0.3s ease;
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        .btn:hover {
            transform: scale(1.05);
            box-shadow: 0 5px 20px rgba(0,0,0,0.3);
        }
        .btn:active {
            transform: scale(0.98);
        }
        .btn-on {
            background: linear-gradient(135deg, #00ff88 0%, #00cc6a 100%);
            color: #000;
        }
        .btn-off {
            background: linear-gradient(135deg, #ff4444 0%, #cc0000 100%);
            color: #fff;
        }
        .btn-auto {
            background: linear-gradient(135deg, #4488ff 0%, #0066ff 100%);
            color: #fff;
        }
        .settings-section {
            margin-top: 20px;
            padding-top: 20px;
            border-top: 1px solid rgba(255,255,255,0.2);
        }
        .slider-container {
            margin: 20px 0;
        }
        .slider-label {
            display: flex;
            justify-content: space-between;
            margin-bottom: 10px;
        }
        .slider {
            width: 100%;
            height: 8px;
            border-radius: 5px;
            background: rgba(255,255,255,0.2);
            outline: none;
            -webkit-appearance: none;
        }
        .slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 20px;
            height: 20px;
            border-radius: 50%;
            background: #00ff88;
            cursor: pointer;
            box-shadow: 0 0 10px rgba(0,255,136,0.5);
        }
        .slider::-moz-range-thumb {
            width: 20px;
            height: 20px;
            border-radius: 50%;
            background: #00ff88;
            cursor: pointer;
            border: none;
            box-shadow: 0 0 10px rgba(0,255,136,0.5);
        }
        .chart-container {
            grid-column: 1 / -1;
            background: rgba(255,255,255,0.1);
            backdrop-filter: blur(10px);
            border-radius: 20px;
            padding: 25px;
            border: 1px solid rgba(255,255,255,0.2);
        }
        canvas {
            max-height: 300px;
        }
        .mode-badge {
            display: inline-block;
            padding: 5px 15px;
            border-radius: 20px;
            font-size: 0.9em;
            font-weight: bold;
            margin-left: 10px;
        }
        .mode-badge.auto {
            background: #4488ff;
        }
        .mode-badge.manual {
            background: #ff9500;
        }
        @media (max-width: 768px) {
            h1 {
                font-size: 1.8em;
            }
            .dashboard-grid {
                grid-template-columns: 1fr;
            }
            .status-bar {
                flex-direction: column;
                gap: 10px;
            }
            .temp-display {
                font-size: 3em;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>🌡️ Thermal Control System</h1>
            <p class="subtitle">Autonomous Environmental Management</p>
        </header>
        <div class="status-bar">
            <div class="status-item">
                <div class="status-dot online" id="connectionStatus"></div>
                <span id="connectionText">Connected</span>
            </div>
            <div class="status-item">
                <span>⏱️ Uptime: <strong id="uptime">--:--:--</strong></span>
            </div>
            <div class="status-item">
                <span>Mode: <span class="mode-badge auto" id="modeBadge">AUTO</span></span>
            </div>
        </div>
        <div class="dashboard-grid">
            <div class="card">
                <div class="card-title">
                    <span class="card-icon">🌡️</span>
                    Temperature
                </div>
                <div class="temp-display" id="temperature">
                    --.- <span class="temp-unit">°C</span>
                </div>
                <div class="metric-row">
                    <span class="metric-label">Humidity</span>
                    <span class="metric-value" id="humidity">--%</span>
                </div>
                <div class="metric-row">
                    <span class="metric-label">Threshold</span>
                    <span class="metric-value" id="thresholdDisplay">--°C</span>
                </div>
            </div>
            <div class="card">
                <div class="card-title">
                    <span class="card-icon">💨</span>
                    Fan Control
                </div>
                <div class="fan-status">
                    <div class="fan-icon" id="fanIcon">🌀</div>
                    <div class="fan-label" id="fanLabel">OFF</div>
                </div>
                <div class="control-buttons">
                    <button class="btn btn-on" onclick="controlFan('on')">⚡ Turn ON</button>
                    <button class="btn btn-off" onclick="controlFan('off')">⛔ Turn OFF</button>
                    <button class="btn btn-auto" onclick="controlFan('auto')">🤖 Auto Mode</button>
                </div>
            </div>
            <div class="card">
                <div class="card-title">
                    <span class="card-icon">⚙️</span>
                    Settings
                </div>
                <div class="settings-section">
                    <div class="slider-container">
                        <div class="slider-label">
                            <span>Temperature Threshold</span>
                            <span><strong id="sliderValue">26</strong>°C</span>
                        </div>
                        <input type="range" min="18" max="35" value="26" step="0.5" 
                               class="slider" id="thresholdSlider" 
                               oninput="updateSliderValue(this.value)">
                    </div>
                    <button class="btn btn-auto" onclick="updateThreshold()" style="width: 100%;">💾 Save Threshold</button>
                </div>
            </div>
            <div class="chart-container">
                <div class="card-title">
                    <span class="card-icon">📊</span>
                    Temperature History (Last 2 Minutes)
                </div>
                <canvas id="tempChart"></canvas>
            </div>
        </div>
    </div>
    <script>
        let chart;
        let updateTimer;
        function initChart() {
            const ctx = document.getElementById('tempChart').getContext('2d');
            chart = new Chart(ctx, {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [{
                        label: 'Temperature (°C)',
                        data: [],
                        borderColor: '#00ff88',
                        backgroundColor: 'rgba(0, 255, 136, 0.1)',
                        tension: 0.4,
                        fill: true
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: true,
                    plugins: {
                        legend: {
                            labels: { color: '#fff', font: { size: 14 } }
                        }
                    },
                    scales: {
                        x: {
                            ticks: { color: '#fff' },
                            grid: { color: 'rgba(255,255,255,0.1)' }
                        },
                        y: {
                            ticks: { color: '#fff' },
                            grid: { color: 'rgba(255,255,255,0.1)' }
                        }
                    }
                }
            });
        }
        async function updateData() {
            try {
                const response = await fetch('/data');
                const data = await response.json();
                document.getElementById('temperature').innerHTML = `${data.temp.toFixed(1)} <span class="temp-unit">°C</span>`;
                document.getElementById('humidity').textContent = `${data.humidity.toFixed(1)}%`;
                document.getElementById('thresholdDisplay').textContent = `${data.threshold.toFixed(1)}°C`;
                document.getElementById('sliderValue').textContent = data.threshold.toFixed(1);
                document.getElementById('thresholdSlider').value = data.threshold;
                updateFanStatus(data.fanOn);
                updateMode(data.manual);
                document.getElementById('uptime').textContent = formatUptime(data.uptime);
                updateChart(data.history);
            } catch (error) {
                console.error('Error:', error);
            }
        }
        async function controlFan(action) {
            try {
                await fetch('/control?action=' + action);
                setTimeout(updateData, 200);
            } catch (error) {
                console.error('Error:', error);
            }
        }
        async function updateThreshold() {
            const newThreshold = document.getElementById('thresholdSlider').value;
            try {
                await fetch('/settings?threshold=' + newThreshold);
                setTimeout(updateData, 200);
            } catch (error) {
                console.error('Error:', error);
            }
        }
        function updateSliderValue(value) {
            document.getElementById('sliderValue').textContent = parseFloat(value).toFixed(1);
        }
        function updateFanStatus(isOn) {
            const fanIcon = document.getElementById('fanIcon');
            const fanLabel = document.getElementById('fanLabel');
            if (isOn) {
                fanIcon.classList.add('spinning');
                fanLabel.textContent = 'ON';
                fanLabel.classList.add('on');
                fanLabel.classList.remove('off');
            } else {
                fanIcon.classList.remove('spinning');
                fanLabel.textContent = 'OFF';
                fanLabel.classList.add('off');
                fanLabel.classList.remove('on');
            }
        }
        function updateMode(isManual) {
            const modeBadge = document.getElementById('modeBadge');
            if (isManual) {
                modeBadge.textContent = 'MANUAL';
                modeBadge.classList.remove('auto');
                modeBadge.classList.add('manual');
            } else {
                modeBadge.textContent = 'AUTO';
                modeBadge.classList.add('auto');
                modeBadge.classList.remove('manual');
            }
        }
        function updateChart(history) {
            const labels = history.map((_, i) => `${i * 2}s`);
            chart.data.labels = labels;
            chart.data.datasets[0].data = history;
            chart.update('none');
        }
        function formatUptime(seconds) {
            const h = Math.floor(seconds / 3600);
            const m = Math.floor((seconds % 3600) / 60);
            const s = seconds % 60;
            return `${h.toString().padStart(2, '0')}:${m.toString().padStart(2, '0')}:${s.toString().padStart(2, '0')}`;
        }
        window.onload = function() {
            initChart();
            updateData();
            updateTimer = setInterval(updateData, 2000);
        };
    </script>
</body>
</html>
)rawliteral";
  
  server.send(200, "text/html", html);
}

void handleData() {
  String json = "{";
  json += "\"temp\":" + String(currentTemp) + ",";
  json += "\"humidity\":" + String(currentHumidity) + ",";
  json += "\"fanOn\":" + String(fanOn ? "true" : "false") + ",";
  json += "\"manual\":" + String(manualOverride ? "true" : "false") + ",";
  json += "\"threshold\":" + String(TEMP_THRESHOLD) + ",";
  json += "\"uptime\":" + String(uptimeSeconds) + ",";
  json += "\"history\":[";
  for (int i = 0; i < DATA_POINTS; i++) {
    json += String(tempHistory[i]);
    if (i < DATA_POINTS - 1) json += ",";
  }
  json += "]}";
  
  server.send(200, "application/json", json);
}

void handleControl() {
  if (server.hasArg("action")) {
    String action = server.arg("action");
    
    if (action == "on") {
      digitalWrite(RELAY_PIN, HIGH);
      fanOn = true;
      manualOverride = true;
      Serial.println(">>> FAN TURNED ON (MANUAL) <<<");
    }
    else if (action == "off") {
      digitalWrite(RELAY_PIN, LOW);
      fanOn = false;
      manualOverride = true;
      Serial.println(">>> FAN TURNED OFF (MANUAL) <<<");
    }
    else if (action == "auto") {
      manualOverride = false;
      Serial.println(">>> AUTO MODE ENABLED <<<");
    }
  }
  
  server.send(200, "text/plain", "OK");
}

void handleSettings() {
  if (server.hasArg("threshold")) {
    float newThreshold = server.arg("threshold").toFloat();
    if (newThreshold > 0 && newThreshold < 50) {
      TEMP_THRESHOLD = newThreshold;
      Serial.print("Threshold updated to: ");
      Serial.println(TEMP_THRESHOLD);
    }
  }
  
  server.send(200, "text/plain", "OK");
}