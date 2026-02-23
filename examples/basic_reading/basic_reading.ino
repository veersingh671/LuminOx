#include <LuminOx.h>

HardwareSerial SensorSerial(2);   // For ESP32 – change pins / UART number if needed
LuminOx ox(SensorSerial);

void setup() {
  Serial.begin(115200);
  delay(400);
  SensorSerial.begin(9600, SERIAL_8N1, 16, 17);   // ESP32 UART2 example – RX=16, TX=17


  Serial.println("\nLuminOx Basic Reading Example\n");

  ox.setDebug(true);          // show more messages (optional)
  ox.begin();                 // initialize + show sensor info
}

void loop() {
  LuminOxReading r;
  if (ox.readAll(r)) {
    Serial.printf("O₂       = %.2f %%\n", r.o2_percent);
    Serial.printf("ppO₂     = %.1f mbar\n", r.ppO2_mbar);
    Serial.printf("Temp     = %.1f °C\n", r.temperature_C);
    if (r.pressure_mbar > 0)
      Serial.printf("Pressure = %.0f mbar\n", r.pressure_mbar);
    Serial.printf("Status   = %s  (%s)\n\n", r.valid ? "VALID" : "INVALID", r.status.c_str());
  } else {
    Serial.print("Failed → ");
    Serial.println(r.error.isEmpty() ? "unknown" : r.error);
    Serial.println("Raw: " + r.raw + "\n");
  }

  delay(2500);
}