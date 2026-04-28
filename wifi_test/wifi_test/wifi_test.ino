/*
 * WiFi Module Test — ESP8266
 * Board: Arduino MEGA 2560
 *
 * Wiring:
 *   Module 5V  ──► MEGA 5V
 *   Module GND ──► MEGA GND
 *   Module TX  ──► MEGA D19 (RX1)
 *   Module RX  ──► MEGA D18 (TX1)
 *
 * Open Serial Monitor at 9600 baud to see results.
 */

#define esp Serial1  // hardware serial on pins 18(TX1) / 19(RX1)
const char* HOST = "webhook.site";



// const char* SSID = "ESAIP";
// const char* WIFI_USERNAME = "nguengant.ing2027";
// const char* PASSWORD = "password";

const char* SSID = "Test261";
const char* WIFI_USERNAME = "";
const char* PASSWORD = "yobx5224";



bool sendAT(const char* cmd, const char* expected, int timeout = 3000) {
  esp.println(cmd);
  long t = millis();
  String response = "";

  while (millis() - t < timeout) {
    while (esp.available()) response += (char)esp.read();
    if (response.indexOf(expected) != -1) {
      Serial.println("[OK] " + response);
      return true;
    }
  }
  Serial.println("[FAIL] " + response);
  return false;
}

void setup() {
  Serial.begin(9600);
  esp.begin(115200);  // most ESP modules default to 115200
  delay(1000);

  Serial.println(F("=== WiFi Module Test ==="));
  Serial.println();

  Serial.println(F("1. Testing AT..."));
  if (!sendAT("AT", "OK")) {
    Serial.println(F(">> Module not responding. Check wiring (5V, GND, TX->D5, RX->D4)."));
    while (true);
  }

  Serial.println(F("2. Setting WiFi mode..."));
  sendAT("AT+CWMODE=1", "OK");

  Serial.println(F("3. Connecting to WiFi..."));
  String cmd;
  if (strlen(WIFI_USERNAME) > 0) {
    // WPA2-Enterprise (PEAP)
    Serial.println(F("   Mode: WPA2-Enterprise"));
    cmd = "AT+CWJEAP=\"";
    cmd += SSID;
    cmd += "\",0,\"";
    cmd += WIFI_USERNAME;
    cmd += "\",\"";
    cmd += WIFI_USERNAME;
    cmd += "\",\"";
    cmd += PASSWORD;
    cmd += "\", 0";
  } else {
    // WPA2-PSK (password only)
    Serial.println(F("   Mode: WPA2-PSK"));
    cmd = "AT+CWJAP=\"";
    cmd += SSID;
    cmd += "\",\"";
    cmd += PASSWORD;
    cmd += "\"";
  }

  if (!sendAT(cmd.c_str(), "WIFI GOT IP", 20000)) {
    Serial.println(F(">> WiFi connection failed. Check SSID and password."));
    while (true);
  }
  Serial.println(F(">> WiFi connected!"));

  Serial.println(F("4. Getting IP address..."));
  sendAT("AT+CIFSR", "OK", 3000);

  Serial.println(F("5. Pinging webhook.site..."));
  String pingCmd = "AT+PING=\"";
  pingCmd += HOST;
  pingCmd += "\"";

  if (sendAT(pingCmd.c_str(), "OK", 5000)) {
    Serial.println(F(">> Ping successful!"));
  } else {
    Serial.println(F(">> Ping failed. WiFi connected but no internet?"));
  }

  Serial.println();
  Serial.println(F("=== Test Complete ==="));
}

void loop() {
  // Nothing, all tests run once in setup
}
