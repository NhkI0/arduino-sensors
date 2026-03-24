# DHT11 Temperature & Humidity

Standalone test sketch for the DHT11 sensor on an Arduino MEGA 2560. Outputs temperature (C/F) and humidity to the Serial Monitor.

## Wiring

| DHT11 Pin | MEGA Pin |
|-----------|----------|
| DATA      | D2       |
| VCC       | 5V       |
| GND       | GND      |

Pin order on this module (left to right, mesh facing you): **DATA - VCC - GND**. No external resistor needed (uses internal pull-up).

## Libraries

- Adafruit DHT sensor library
