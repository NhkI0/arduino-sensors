#include <DHT.h>
#include <ArduinoJson.h>

// ── Thermistors ──────────────────────────────────────
// Each thermistor needs its own 10kΩ series resistor.
// Wiring per thermistor: 5V -> thermistor -> Ax -> 10kΩ -> GND
const int THERM_PINS[]   = {A0, A1, A2, A3};  // add/remove pins here
const int THERM_COUNT    = sizeof(THERM_PINS) / sizeof(THERM_PINS[0]);
#define SERIES_R     10000.0   // 10kΩ series resistor
#define THERM_R0     100000.0  // thermistor resistance at 25°C (100kΩ NTC)
#define THERM_B      3950.0    // Beta value
#define T0           298.15    // 25°C in Kelvin
#define DHT_PIN      6
#define DHT_TYPE     DHT11
#define WIFI_LED     2   // solid ON when WiFi connected
#define POST_LED     3   // blinks on each POST

DHT dht(DHT_PIN, DHT_TYPE);
#define esp Serial1

const char* SSID     = "ESAIP-EVENT";
const char* PASSWORD = "ActiveESAIP2026";
const char* HOST     = "activesaip.thomasbechu.me";
const char* ENDPOINT = "/temperatures";
const int   PORT     = 80;

int loopCounter = 0;
const int OUTPUT_RATE = 120; // ~60 seconds between POSTs
const int POST_LED_DURATION = 1500;

bool wifiConnected = false;
String wifiIP = "";
unsigned long postLedOff = 0; // millis() time to turn POST LED off


bool sendAT(const char* cmd, const char* expected, int timeout = 3000) {
  esp.println(cmd);
  long t = millis();
  String response = "";

  while (millis() - t < timeout) {
    while (esp.available()) response += (char)esp.read();
    if (response.indexOf(expected) != -1) {
      Serial.println("[AT] " + response);
      return true;
    }
  }
  Serial.println("[AT FAIL] " + response);
  return false;
}


String sendATResponse(const char* cmd, int timeout = 3000) {
  esp.println(cmd);
  long t = millis();
  String response = "";
  while (millis() - t < timeout) {
    while (esp.available()) response += (char)esp.read();
  }
  return response;
}


void connectWiFi() {
  Serial.println("Connecting to WiFi...");
  Serial.print("SSID: ");
  Serial.println(SSID);

  sendAT("AT", "OK");
  sendAT("AT+CWMODE=1", "OK");
  sendAT("AT+CIPSSLSIZE=4096", "OK");

  String cmd;

  Serial.println("Mode: WPA2-PSK");
  cmd = "AT+CWJAP=\"";
  cmd += SSID;
  cmd += "\",\"";
  cmd += PASSWORD;
  cmd += "\"";

  if (!sendAT(cmd.c_str(), "WIFI GOT IP", 20000)) {
    Serial.println("WiFi FAILED! Check credentials.");
    wifiConnected = false;
    digitalWrite(WIFI_LED, LOW);
    return;
  }

  wifiConnected = true;
  digitalWrite(WIFI_LED, HIGH);
  Serial.println("WiFi connected!");

  // Get IP address
  String ipResp = sendATResponse("AT+CIFSR");
  int start = ipResp.indexOf("STAIP,\"");
  if (start != -1) {
    start += 7;
    int end = ipResp.indexOf("\"", start);
    if (end != -1) wifiIP = ipResp.substring(start, end);
  }
  Serial.print("IP: ");
  Serial.println(wifiIP);
}


void checkWiFi() {
  String resp = sendATResponse("AT+CWJAP?");
  if (resp.indexOf("No AP") != -1 || resp.indexOf("ERROR") != -1) {
    wifiConnected = false;
    digitalWrite(WIFI_LED, LOW);
  } else if (resp.indexOf(SSID) != -1) {
    wifiConnected = true;
    digitalWrite(WIFI_LED, HIGH);
  }
}


void printWiFiStatus() {
  checkWiFi();
  Serial.println("── WiFi Status ──");
  Serial.print("  SSID:      ");
  Serial.println(SSID);
  Serial.print("  Connected: ");
  Serial.println(wifiConnected ? "YES" : "NO");
  if (wifiConnected) {
    Serial.print("  IP:        ");
    Serial.println(wifiIP);

    // Query signal strength (RSSI)
    String resp = sendATResponse("AT+CWJAP?");
    int rssiStart = resp.lastIndexOf(",");
    if (rssiStart != -1) {
      int rssiEnd = resp.indexOf("\r", rssiStart);
      if (rssiEnd == -1) rssiEnd = resp.length();
      String rssi = resp.substring(rssiStart + 1, rssiEnd);
      rssi.trim();
      Serial.print("  RSSI:      ");
      Serial.print(rssi);
      Serial.println(" dBm");
    }
  }
  Serial.println("─────────────────");
}


float readThermistor(int pin) {
  int raw = analogRead(pin);
  if (raw == 0 || raw == 1023) return NAN;
  double R = SERIES_R * (1024.0 / raw - 1.0);
  double tempK = 1.0 / (1.0 / T0 + (1.0 / THERM_B) * log(R / THERM_R0));
  return tempK - 273.15;
}


String buildPayload(float temps[], int count, float humidity){
  StaticJsonDocument<300> doc;
  doc["room"] = "E";
  JsonArray arr = doc.createNestedArray("temperatures");
  for (int i = 0; i < count; i++) {
    arr.add(serialized(String(temps[i], 1)));
  }
  doc["humidity"] = humidity;

  String payload;
  serializeJson(doc, payload);

  Serial.println("Payload: " + payload);
  return payload;
}


void postSensorData(String payload) {
  if (!wifiConnected) {
    Serial.println("Skipping POST — WiFi not connected.");
    return;
  }

  String tcpCmd = "AT+CIPSTART=\"TCP\",\"";
  tcpCmd += HOST;
  tcpCmd += "\",";
  tcpCmd += PORT;

  if (!sendAT(tcpCmd.c_str(), "CONNECT", 5000)) {
    Serial.println("TCP connection failed, retrying next loop.");
    return;
  }

  String http  = "POST ";
         http += ENDPOINT;
         http += " HTTP/1.1\r\n";
         http += "Host: ";
         http += HOST;
         http += "\r\n";
         http += "Content-Type: application/json\r\n";
         http += "Content-Length: ";
         http += payload.length();
         http += "\r\n";
         http += "Connection: close\r\n\r\n";
         http += payload;

  String sendCmd = "AT+CIPSEND=";
  sendCmd += http.length();

  if (!sendAT(sendCmd.c_str(), ">", 30000)) {
    Serial.println("CIPSEND failed!");
    sendAT("AT+CIPCLOSE", "OK");
    return;
  }

  esp.print(http);
  digitalWrite(POST_LED, HIGH);
  postLedOff = millis() + POST_LED_DURATION;

  Serial.println("=== API Response ===");
  long t = millis();
  while (millis() - t < 6000) {
    while (esp.available()) {
      Serial.write(esp.read());
      t = millis();
    }
  }

  sendAT("AT+CIPCLOSE", "OK");
  Serial.println("\n=== Request Done ===");
}


void setup()
{
  Serial.begin(9600);
  esp.begin(115200);
  pinMode(WIFI_LED, OUTPUT);
  pinMode(POST_LED, OUTPUT);
  Serial.println("=== Screenless Thermometer ===");
  Serial.println("Waiting for ESP to boot...");
  delay(3000); // give ESP time to boot
  pinMode(DHT_PIN, INPUT_PULLUP);
  dht.begin();
  connectWiFi();
  printWiFiStatus();
  delay(1000);
}

void loop()
{
  // Read all thermistors
  float temps[THERM_COUNT];
  for (int i = 0; i < THERM_COUNT; i++) {
    temps[i] = readThermistor(THERM_PINS[i]);
  }

  float humidity = dht.readHumidity();

  // Serial output: all thermistors
  for (int i = 0; i < THERM_COUNT; i++) {
    Serial.print("T");
    Serial.print(i + 1);
    Serial.print(": ");
    if (isnan(temps[i])) Serial.print("err");
    else Serial.print(temps[i], 1);
    Serial.print("C  ");
  }
  Serial.print("H: ");
  Serial.print(humidity, 1);
  Serial.println("%");

  if (loopCounter == 0){
    if (!wifiConnected) {
      Serial.println("Retrying WiFi...");
      connectWiFi();
    }
    if (wifiConnected) {
      printWiFiStatus();
      postSensorData(buildPayload(temps, THERM_COUNT, humidity));
    }
    loopCounter = OUTPUT_RATE;
  }

  // Turn off POST LED after duration
  if (postLedOff > 0 && millis() >= postLedOff) {
    digitalWrite(POST_LED, LOW);
    postLedOff = 0;
  }

  delay(500);
  loopCounter--;
}
