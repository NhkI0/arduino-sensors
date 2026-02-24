#include <LiquidCrystal.h>
#include <DHT.h>

#define TEMP_PIN     A0
#define NOMINAL_R    10000.0 
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

  double tempK = log(NOMINAL_R * (1024.0 / raw - 1.0));
  tempK = 1.0 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempK * tempK)) * tempK);
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

  delay(500);
}
