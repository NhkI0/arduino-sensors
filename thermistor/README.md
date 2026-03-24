# NTC Thermistor Temperature

Standalone test sketch for a 10k NTC thermistor on an Arduino MEGA 2560. Uses the Steinhart-Hart equation to convert ADC readings to temperature. Outputs to Serial Monitor.

## Circuit

```
MEGA 5V ────────────────┬──── MEGA A0
                        |
                   [Thermistor]
                        |
MEGA GND ─[10k resistor]┘
```

Thermistor between 5V and A0, 10k reference resistor between A0 and GND.

## Libraries

- math.h (built-in)
