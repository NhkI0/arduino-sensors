/*
 * NTC Thermistor (10kΩ) + DHT11 — Dual Temperature Sensor
 * Board: Arduino MEGA 2560
 *
 * ── Thermistor ─────────────────────────────────────────
 *
 *   A thermistor is a passive component — it needs a 10kΩ reference
 *   resistor to form a voltage divider. This is mandatory.
 *
 *   Circuit:
 *
 *     MEGA 5V ──[10kΩ resistor]──┬──── MEGA A0
 *                                │
 *                           [Thermistor]
 *                                │
 *     MEGA GND ──────────────────┘
 *
 *   Wiring:
 *     Resistor  leg 1  ──► MEGA 5V
 *     Resistor  leg 2  ──► MEGA A0  (same row as thermistor pin 1)
 *     Thermistor pin 1 ──► MEGA A0  (same row as resistor leg 2)
 *     Thermistor pin 2 ──► MEGA GND
 *
 *   The thermistor has no polarity — either leg can go either way.
 *
 * ── DHT11 ──────────────────────────────────────────────
 *
 *   Library: "DHT sensor library" by Adafruit
 *
 *   Pin order (left→right, mesh facing you): DATA · VCC · GND
 *
 *   Wiring:
 *     DHT11 DATA ──► MEGA D2
 *     DHT11 VCC  ──► MEGA 5V
 *     DHT11 GND  ──► MEGA GND
 */
#include <math.h>
#include <DHT.h>

#define TEMP_PIN     A0
#define SERIES_R     10000.0   // 10kΩ reference resistor
#define NOMINAL_R    10000.0   // thermistor resistance at 25°C
#define READ_INTERVAL_MS 1000

#define DHT_PIN   2
#define DHT_TYPE  DHT11

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(9600);
  pinMode(DHT_PIN, INPUT_PULLUP);
  Serial.println(F("NTC Thermistor ready."));
  dht.begin();
  delay(2000);
  Serial.println(F("DHT11 ready."));
}

void loop() {
  int raw = analogRead(TEMP_PIN);

  if (raw == 0 || raw == 1023) {
    Serial.println(F("Read error — check wiring"));
    delay(READ_INTERVAL_MS);
    return;
  }

  // Steinhart-Hart equation (thermistor between A0 and GND, resistor between 5V and A0)
  double tempK = log(NOMINAL_R * raw / (1024.0 - raw));
  tempK = 1.0 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempK * tempK)) * tempK);

  float tempC = tempK - 273.15;
  float tempF = (tempC * 9.0 / 5.0) + 32.0;

  Serial.println(F("Thermistor:"));
  Serial.print(F("Temp: "));
  Serial.print(tempC, 1);
  Serial.print(F(" C  /  "));
  Serial.print(tempF, 1);
  Serial.println(F(" F"));

  // DHT11

  float humidity = dht.readHumidity();
  float tempCDHT    = dht.readTemperature();
  float tempFDHT    = dht.readTemperature(true);

  Serial.println(F("DHT11:"));
  if (isnan(humidity) || isnan(tempC)) {
    Serial.println(F("Read error — check wiring"));
  } else {
    Serial.print(F("Temp: "));
    Serial.print(tempCDHT, 1);
    Serial.print(F(" C  /  "));
    Serial.print(tempFDHT, 1);
    Serial.print(F(" F    Humidity: "));
    Serial.print(humidity, 1);
    Serial.println(F(" %"));
  }
  delay(READ_INTERVAL_MS);
}
