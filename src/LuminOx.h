#ifndef LUMINOX_H
#define LUMINOX_H

#include <Arduino.h>

struct LuminOxReading {
    float  ppO2_mbar      = 0.0f;
    float  o2_percent     = -1.0f;
    float  temperature_C  = 0.0f;
    float  pressure_mbar  = -1.0f;
    String status         = "";
    bool   valid          = false;
    String raw            = "";
    String error          = "";
};

class LuminOx {
public:
    LuminOx(Stream &serialPort, uint16_t timeout_ms = 2200, uint8_t retries = 3);

    bool   begin(bool printInfo = true);
    bool   readAll(LuminOxReading &reading);

    String getSensorInfo();

    // Quick access after successful read
    float  getO2Percent()     const { return lastReading.o2_percent; }
    float  getPpO2()          const { return lastReading.ppO2_mbar; }
    float  getTemperature()   const { return lastReading.temperature_C; }
    float  getPressure()      const { return lastReading.pressure_mbar; }
    String getStatus()        const { return lastReading.status; }
    bool   isValid()          const { return lastReading.valid; }
    String getLastError()     const { return lastReading.error; }

    void   setDebug(bool on)  { debug = on; }

private:
    Stream       &port;
    uint16_t      timeout;
    uint8_t       maxRetries;
    bool          debug = false;
    LuminOxReading lastReading;

    void   flush();
    String sendCommand(const char* cmd);
    String readResponse();
    bool   parse(const String &line, LuminOxReading &r);
};

#endif