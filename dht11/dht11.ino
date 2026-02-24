/*
 * DHT11 — Temperature & Humidity
 * Board: Arduino MEGA 2560
 *
 * Library: "DHT sensor library" by Adafruit
 *
 * Wiring (3 wires, direct to MEGA — no resistor needed):
 *
 *   DHT11 pin 1 (DATA) ──► MEGA D2
 *   DHT11 pin 2 (VCC)  ──► MEGA 5V
 *   DHT11 pin 3 (GND)  ──► MEGA GND
 *
 *   Pin order confirmed on this module (left→right with mesh facing you):
 *     DATA · VCC · GND
 */

#include <DHT.h>

#define DHT_PIN   2
#define DHT_TYPE  DHT11

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(9600);
  pinMode(DHT_PIN, INPUT_PULLUP); // internal pull-up — no external resistor needed
  dht.begin();
  delay(2000); // DHT11 needs time to stabilize on power-up
  Serial.println(F("DHT11 ready."));
}

void loop() {
  float humidity = dht.readHumidity();
  float tempC    = dht.readTemperature();
  float tempF    = dht.readTemperature(true);

  if (isnan(humidity) || isnan(tempC)) {
    Serial.println(F("Read error — check wiring"));
  } else {
    Serial.print(F("Temp: "));
    Serial.print(tempC, 1);
    Serial.print(F(" C  /  "));
    Serial.print(tempF, 1);
    Serial.print(F(" F    Humidity: "));
    Serial.print(humidity, 1);
    Serial.println(F(" %"));
  }

  delay(2000);
}
