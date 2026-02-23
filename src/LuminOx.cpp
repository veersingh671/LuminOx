#include "LuminOx.h"

LuminOx::LuminOx(Stream &serialPort, uint16_t timeout_ms, uint8_t retries)
    : port(serialPort), timeout(timeout_ms), maxRetries(retries) {}

void LuminOx::flush() {
    while (port.available()) port.read();
}

String LuminOx::sendCommand(const char* cmd) {
    flush();
    port.println(cmd);
    delay(35);
    return readResponse();
}

String LuminOx::readResponse() {
    String line = "";
    unsigned long t0 = millis();

    while (millis() - t0 < timeout) {
        if (port.available()) {
            char c = port.read();
            if (c == '\n') {
                line.trim();
                if (!line.isEmpty() && line[line.length()-1] == '\r')
                    line.remove(line.length()-1);
                if (debug) Serial.println("[LuminOx] → " + line);
                return line;
            }
            if (c != '\r') line += c;
        }
        delay(1);
    }
    if (debug) Serial.println("[LuminOx] TIMEOUT");
    return "TIMEOUT";
}

bool LuminOx::begin(bool printInfo) {
    delay(7500);  // sensor stabilization

    String reply = sendCommand("M 1");
    bool pollOk = reply.startsWith("M 1") || reply.indexOf("M") >= 0;

    if (debug) {
        Serial.println(pollOk ? "[LuminOx] Poll mode set" : "[LuminOx] Poll mode uncertain");
    }

    if (printInfo) {
        Serial.println("\nLuminOx Sensor Information:");
        Serial.print(getSensorInfo());
        Serial.println("───────────────────────────────\n");
    }

    return pollOk;
}

String LuminOx::getSensorInfo() {
    String s;
    s += "Manufacture date : " + sendCommand("# 0") + "\n";
    s += "Serial number    : " + sendCommand("# 1") + "\n";
    s += "Firmware version : " + sendCommand("# 2") + "\n";
    return s;
}

bool LuminOx::readAll(LuminOxReading &reading) {
    reading = LuminOxReading();
    reading.raw = sendCommand("A");

    if (reading.raw == "TIMEOUT" || reading.raw.length() < 20) {
        reading.error = "No valid response";
        return false;
    }

    return parse(reading.raw, reading);
}

bool LuminOx::parse(const String &line, LuminOxReading &r) {
    if (line.startsWith("E ")) {
        r.error = line;
        r.status = "ERROR";
        return false;
    }

    char st[16] = {0};
    int cnt;

    // LOX-02 full format
    cnt = sscanf(line.c_str(), "O %f T %f P %f %% %f e %15s",
                 &r.ppO2_mbar, &r.temperature_C, &r.pressure_mbar, &r.o2_percent, st);

    if (cnt == 5) {
        r.status = st;
        r.valid = (strcmp(st, "0000") == 0) && r.ppO2_mbar > 0.05f;
        return r.valid;
    }

    // LOX-01 or minimal
    cnt = sscanf(line.c_str(), "O %f T %f e %15s",
                 &r.ppO2_mbar, &r.temperature_C, st);

    if (cnt == 3) {
        r.status = st;
        r.pressure_mbar = -1.0f;
        r.o2_percent = -1.0f;
        r.valid = (strcmp(st, "0000") == 0) && r.ppO2_mbar > 0.05f;
        return r.valid;
    }

    // Fallback – contains dashes
    if (line.indexOf("-----") > 0) {
        sscanf(line.c_str(), "O %f T %f", &r.ppO2_mbar, &r.temperature_C);
        r.status = "LOX-01";
        r.valid = r.ppO2_mbar > 0.05f;
        return r.valid;
    }

    r.error = "Parse failed";
    return false;
}