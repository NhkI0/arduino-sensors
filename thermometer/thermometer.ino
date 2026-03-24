#include <LiquidCrystal.h>
#include <DHT.h>

#define TEMP_PIN     A0
#define SERIES_R     10000.0   // 10kΩ series resistor
#define THERM_R0     100000.0  // thermistor resistance at 25°C (100kΩ NTC)
#define THERM_B      3950.0    // Beta value
#define T0           298.15    // 25°C in Kelvin
#define DHT_PIN   6
#define DHT_TYPE  DHT11

DHT dht(DHT_PIN, DHT_TYPE);
//                BS  E  D4 D5  D6 D7
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

void setup()
{
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("   Activ'ESAIP   ");
  lcd.setCursor(0, 1);
  lcd.print("   Patientez...   ");
  pinMode(DHT_PIN, INPUT_PULLUP);
  dht.begin();
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
  lcd.print("H:      R:      ");
  lcd.setCursor(2, 1);
  lcd.print(humidity, 0);
  lcd.setCursor(10, 1);
  lcd.print(raw);

  delay(500);
}
