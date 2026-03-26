#define esp Serial1

void setup() {
  Serial.begin(9600);
  esp.begin(115200); // ESP default baud
  Serial.println("=== ESP Passthrough Ready ===");
  Serial.println("Type AT to test connection.");
}

void loop() {
  // Forward PC -> ESP
  while (Serial.available()) {
    esp.write(Serial.read());
  }
  // Forward ESP -> PC
  while (esp.available()) {
    Serial.write(esp.read());
  }
}
