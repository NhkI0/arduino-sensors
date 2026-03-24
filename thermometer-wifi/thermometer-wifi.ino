#include <LiquidCrystal.h>
#include <DHT.h>
// Using Serial1 (hardware serial) instead of SoftwareSerial
// Module TX → MEGA D19 (RX1), Module RX → MEGA D18 (TX1)
#include <ArduinoJson.h>

#define TEMP_PIN     A0
#define SERIES_R     10000.0   // 10kΩ series resistor
#define THERM_R0     100000.0  // thermistor resistance at 25°C (100kΩ NTC)
#define THERM_B      3950.0    // Beta value
#define T0           298.15    // 25°C in Kelvin
#define DHT_PIN   6
#define DHT_TYPE  DHT11

DHT dht(DHT_PIN, DHT_TYPE);
#define esp Serial1
//                BS  E  D4 D5  D6 D7
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// const char* SSID     = "ESAIP";
// const char* WIFI_USERNAME = "nguengant.ing2027";
// const char* PASSWORD = "password";

const char* SSID     = "Test261";
const char* WIFI_USERNAME = "";
const char* PASSWORD = "yobx5224";
const char* HOST     = "webhook.site";
const char* ENDPOINT = "/ca9dcc0d-34c0-4391-a079-c886fcef2976";
const int   PORT     = 80;

int loopCounter = 0;

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


void connectWiFi() {
  Serial.println("Connecting to WiFi...");

  sendAT("AT", "OK");
  sendAT("AT+CWMODE=1", "OK");

  String cmd;
  if (strlen(WIFI_USERNAME) > 0) {
    // WPA2-Enterprise (PEAP)
    Serial.println("Mode: WPA2-Enterprise");
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
    Serial.println("Mode: WPA2-PSK");
    cmd = "AT+CWJAP=\"";
    cmd += SSID;
    cmd += "\",\"";
    cmd += PASSWORD;
    cmd += "\"";
  }

  if (!sendAT(cmd.c_str(), "WIFI GOT IP", 20000)) {
    Serial.println("WiFi failed! Check credentials.");
    while (true);
  }

  Serial.println("WiFi connected!");
}


String buildPayload(float temperature, float humidity){
  StaticJsonDocument<200> doc;
  doc["temperature"] = temperature;
  doc["humidity"]    = humidity;

  String payload;
  serializeJson(doc, payload);

  Serial.println("Payload: " + payload);
  return payload;
}


void postSensorData(String payload) {
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
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("   Activ'ESAIP   ");
  lcd.setCursor(0, 1);
  lcd.print("   Patientez...   ");
  pinMode(DHT_PIN, INPUT_PULLUP);
  dht.begin();
  connectWiFi();
  delay(2000);
}
void loop()
{
  int raw = analogRead(TEMP_PIN);

  if (raw == 0) {
    lcd.setCursor(0, 0);
    lcd.print("  Read error!   ");
    delay(500);
    return;
  }

  // Thermistor on top: 5V → thermistor → A0 → 10kΩ → GND
  double R = SERIES_R * (1024.0 / raw - 1.0);
  // Beta equation
  double tempK = 1.0 / (1.0 / T0 + (1.0 / THERM_B) * log(R / THERM_R0));
  float tempC = tempK - 273.15;
  float tempF = (tempC * 9.0) / 5.0 + 32.0;

  lcd.setCursor(0, 0);
  lcd.print("Temp:       C  ");
  lcd.setCursor(7, 0);
  lcd.print(tempC, 1);


  float humidity = dht.readHumidity();
  float tempCDHT    = dht.readTemperature();
  float tempFDHT    = dht.readTemperature(true);

  lcd.setCursor(0, 1);
  lcd.print("Humi:       %  ");
  lcd.setCursor(7, 1);
  lcd.print(humidity);
  
  if (loopCounter == 0){
    postSensorData(buildPayload(tempC, humidity));
    loopCounter = 20;
  }
  
  delay(500);
  loopCounter += -1;
}
