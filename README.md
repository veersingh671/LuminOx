# LuminOx Arduino Library

Arduino / ESP32 library for **SST Sensing LuminOx** optical oxygen sensors  
(LOX-01, LOX-02, and compatible variants – luminescence quenching technology)

<p align="center">
  <img src="[https://github.com/user-attachments/assets/placeholder-luminox-sensor.jpg](https://www.medicaldevice-network.com/wp-content/uploads/sites/23/2021/10/Optical-Oxygen-Sensors-1.jpg)" alt="LuminOx optical oxygen sensor" width="280">
  <br>
  <small>LuminOx LOX-02 – optical O₂ sensor (image: SST Sensing)</small>
</p>

## Features

- UART 9600 8N1 (factory default – only supported baud rate)
- Poll mode (`M 1`) – recommended for reliable communication
- Automatic detection of LOX-01 vs LOX-02
- Parses full `A` command response:
  - ppO₂ (mbar)
  - O₂ concentration (%) – LOX-02 only
  - Temperature (°C)
  - Barometric pressure (mbar) – LOX-02 only
  - Status code (0000 = good)
- Sensor identity commands (`#0` manufacture date, `#1` serial, `#2` firmware)
- Timeout handling + retry mechanism
- Debug output option
- Works with ESP32, Arduino Uno, Mega, etc.

## Key Specifications (DS-0030 REV 14 / 2019)

| Parameter                  | Value / Range                              |
|----------------------------|--------------------------------------------|
| Technology                 | Luminescence-based optical (non-depleting) |
| Oxygen range (LOX-02)      | 0–25 % O₂                                  |
| ppO₂ range                 | 0–300 mbar                                 |
| Response time (T90)        | < 30 s (typical)                           |
| Accuracy                   | ppO₂ < ±2 % FS                             |
| Supply voltage             | 4.5 – 5.5 V DC                             |
| Supply current             | < 7.5 mA (1 sample/s), < 20 mA peak        |
| Output                     | 3.3 V TTL UART (RX 5 V tolerant)           |
| Operating temperature      | -30 °C to +60 °C                           |
| Storage temperature        | -30 °C to +60 °C                           |
| Humidity                   | 0–99 % RH (non-condensing)                 |
| Barometric pressure range  | LOX-01: 100–1400 mbar<br>LOX-02: 500–1200 mbar |
| Lifetime                   | > 5 years                                  |
| Resolution                 | ppO₂ 0.1 mbar, Temp 0.1 °C, Press 1 mbar, O₂ 0.01 % |

**Notes from datasheet:**
- Factory calibrated – recalibration may be required after extreme temperature fluctuations
- Housing can be cleaned with a damp cloth – **do NOT immerse**
- Contains no hazardous materials – RoHS & REACH compliant
- Maintenance-free (non-depleting sensing principle)

## Pinout & Wiring

**4-pin connector (2.54 mm / 0.1" grid, gold-plated)**

| Pin | Designation          | Connect to                  | Notes                     |
|-----|----------------------|-----------------------------|---------------------------|
| 1   | Vs (+5 V)            | 5 V (4.5–5.5 V)             | Do NOT use 3.3 V          |
| 2   | GND                  | GND                         |                           |
| 3   | Sensor TX (3.3 V)    | Microcontroller RX          | 3.3 V logic level         |
| 4   | Sensor RX (5 V tol.) | Microcontroller TX          | 5 V tolerant input        |

**ESP32 example (using UART2):**

- Pin 3 (TX) → GPIO 16 (RX)
- Pin 4 (RX) → GPIO 17 (TX)

**Important cautions:**

- Always apply power to pins 1 & 2 beforeattempting communication
- Use ESD precautions when handling
- Recommended soldering iron temperature: ≤ 370 °C for < 5 s
- Do NOT solder directly to pins without the PCB washing process

 **Installation

1. Download or clone this repository
2. Copy folder `LuminOx` to your Arduino libraries directory  
   (`~/Documents/Arduino/libraries/` or equivalent)
3. Restart Arduino IDE

Library should now appear in **Sketch → Include Library → LuminOx**

** Basic Example

```cpp
#include <LuminOx.h>

// ESP32 UART2 example
HardwareSerial SensorSerial(2);
LuminOx ox(SensorSerial);

void setup() {
  Serial.begin(115200);
  delay(400);

  // RX=16, TX=17
  SensorSerial.begin(9600, SERIAL_8N1, 16, 17);

  Serial.println("\nLuminOx O₂ Sensor Test\n");

  ox.setDebug(true);     // optional: more verbose output
  ox.begin();            // sets poll mode + shows sensor info
}

void loop() {
  LuminOxReading r;

  if (ox.readAll(r)) {
    Serial.printf("O₂       : %.2f %%\n", r.o2_percent);
    Serial.printf("ppO₂     : %.1f mbar\n", r.ppO2_mbar);
    Serial.printf("Temp     : %.1f °C\n",   r.temperature_C);

    if (r.pressure_mbar >= 0)
      Serial.printf("Pressure : %.0f mbar\n", r.pressure_mbar);

    Serial.printf("Status   : %s (%s)\n\n", r.valid ? "GOOD" : "ERROR", r.status.c_str());
  } else {
    Serial.print("Error: ");
    Serial.println(r.error.isEmpty() ? "no valid response" : r.error);
    Serial.println("Raw: " + r.raw + "\n");
  }

  delay(2500);
}
````

|S.no |Symptom               | Likely cause                | Fix suggestion   
|-----|----------------------|-----------------------------|---------------------------|
|1.   | Always TIMEOUT       | RX/TX swapped               | Swap pins 3 ↔ 4
|2.   |No response/garbage   | Voltage too low (<4.5 V)    | Measure voltage on pin 1–2|
|3.   |Garbled data          | Wrong baud rate             | Must be 9600 8N1 – no other rates|
|4.   |Pressure / %O₂ always -1 | LOX-01 variant           | Normal – only ppO₂ + temp available
|5.   |Status not 0000 | Sensor error (E xx)               | "Check raw response, power cycle"|
|6.   |No sensor info (#0/#1/#2) | Still in stream mode    | Library forces M 1 on begin() |


**References

Official datasheet: DS-0030 REV 14 – LuminOx O₂ Sensors
User's Guide: UG-001 – LuminOx Communication Protocol
Manufacturer: SST Sensing
Technical support: technical@sstsensing.com | +44 (0)1236 459 020

Feedback, bug reports, and pull requests are welcome!
Made with love in India · 2025–2026
