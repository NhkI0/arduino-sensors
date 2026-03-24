# Dual Sensor — Thermistor + DHT11

Combines a 10k NTC thermistor and a DHT11 sensor on an Arduino MEGA 2560. Both readings are printed to the Serial Monitor.

## Wiring

### Thermistor
| Connection       | MEGA Pin |
|------------------|----------|
| Resistor leg 1   | 5V       |
| Resistor leg 2 + Thermistor pin 1 | A0 |
| Thermistor pin 2 | GND      |

10k reference resistor between 5V and A0, thermistor between A0 and GND.

### DHT11
| DHT11 Pin | MEGA Pin |
|-----------|----------|
| DATA      | D2       |
| VCC       | 5V       |
| GND       | GND      |

## Libraries

- Adafruit DHT sensor library
