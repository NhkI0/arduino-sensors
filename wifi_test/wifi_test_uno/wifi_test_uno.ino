#include <SoftwareSerial.h>
SoftwareSerial esp(10, 11);

const long bauds[] = {9600, 115200, 57600, 38400};

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    esp.begin(bauds[i]);
    delay(100);
    esp.println("AT");
    delay(1000);
    String r = "";
    while (esp.available()) r += (char)esp.read();
    Serial.print("Baud ");
    Serial.print(bauds[i]);
    Serial.println(": [" + r + "]");
  }
}

void loop() {}