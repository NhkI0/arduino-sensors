/*
 * NTC Thermistor (10kΩ) — Temperature
 * Board: Arduino MEGA 2560
 *
 * A thermistor is a passive component — it needs a 10kΩ reference
 * resistor to form a voltage divider. This is mandatory.
 *
 * Circuit:
 *
 *   MEGA 5V ──────────────────┬──── MEGA A0
 *                             │
 *                        [Thermistor]
 *                             │
 *   MEGA GND ──[10kΩ resistor]┘
 *
 * Wiring summary:
 *   Thermistor pin 1 ──► MEGA 5V
 *   Thermistor pin 2 ──► MEGA A0  (same row as resistor leg 1)
 *   Resistor  leg 1  ──► MEGA A0  (same row as thermistor pin 2)
 *   Resistor  leg 2  ──► MEGA GND
 *
 * The thermistor has no polarity — either leg can go either way.
 */

#include <math.h>

#define TEMP_PIN     A0
#define SERIES_R     10000.0   // 10kΩ reference resistor
#define NOMINAL_R    10000.0   // thermistor resistance at 25°C
#define READ_INTERVAL_MS 1000

void setup() {
  Serial.begin(9600);
  Serial.println(F("NTC Thermistor ready."));
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

  Serial.print(F("Temp: "));
  Serial.print(tempC, 1);
  Serial.print(F(" C  /  "));
  Serial.print(tempF, 1);
  Serial.println(F(" F"));

  delay(READ_INTERVAL_MS);
}
