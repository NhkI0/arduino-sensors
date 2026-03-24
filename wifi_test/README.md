# WiFi Module Test

Isolated diagnostic sketch for an ESP8266 WiFi module on an Arduino MEGA 2560. Runs a 5-step test sequence once at startup: AT check, mode set, WiFi connect, IP address, and ping.

Supports both WPA2-PSK and WPA2-Enterprise via auto-detection (based on whether `WIFI_USERNAME` is set).

## Wiring

| ESP8266 Pin | MEGA Pin    |
|-------------|-------------|
| 5V          | 5V          |
| GND         | GND         |
| TX          | D19 (RX1)   |
| RX          | D18 (TX1)   |

Uses hardware Serial1 at 115200 baud.

## Usage

1. Upload the sketch
2. Open Serial Monitor at 9600 baud
3. Watch the 5-step diagnostic output
