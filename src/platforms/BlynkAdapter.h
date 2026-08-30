/*
 * BlynkAdapter.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * OPTIONAL DEPENDENCY: requires "Blynk" library (Blynk IoT, the
 * current-generation Blynk.Console-compatible library). NOT bundled
 * or required by the core - per spec "Do not make Blynk mandatory
 * for users who do not need it."
 *
 * DESIGN: Blynk organizes data by "virtual pins" (V0, V1, V2, ...)
 * rather than named fields, so each climate parameter needs an
 * explicit pin assignment before it can be published. The mapX()
 * functions below configure that assignment; update()/
 * publishClimateData() then writes each mapped field to its pin.
 * Unmapped fields (mapX() never called for them) are simply skipped
 * - never written as 0 (still respecting Rule 7's spirit even though
 * ClimateData's own hasX flags are the primary enforcement point).
 *
 * IMPORTANT BUILD NOTE: the real Blynk library expects
 * BLYNK_TEMPLATE_ID/BLYNK_TEMPLATE_NAME/BLYNK_AUTH_TOKEN to be
 * #defined BEFORE `#include <Blynk.h>` (typically at the very top of
 * the user's .ino, before any AmelTech_Climate_plug includes). This
 * header does not itself define those macros - see
 * examples/13_Blynk/13_Blynk.ino for the required include order.
 */

#ifndef AMELTECH_BLYNK_ADAPTER_H
#define AMELTECH_BLYNK_ADAPTER_H

#include <Arduino.h>
#include <BlynkSimpleEsp32.h> // OPTIONAL - install "Blynk" library separately; requires BLYNK_* macros defined before this include in the user's sketch
#include "PlatformAdapter.h"
#include "../diagnostics/ErrorManager.h"

namespace AmelTech {

// -1 means "not mapped" - that field will be skipped on publish.
struct BlynkPinMap {
    int8_t temperature;
    int8_t humidity;
    int8_t pressure;
    int8_t dewPoint;
    int8_t apparentTemperature;
    int8_t rain;
    int8_t precipitation;
    int8_t windSpeed;
    int8_t windDirection;
    int8_t windGusts;
    int8_t cloudCover;
    int8_t uvIndex;
    int8_t solarRadiation;
    int8_t soilTemperature;
    int8_t soilMoisture;

    BlynkPinMap();
};

class BlynkAdapter : public PlatformAdapter {
public:
    BlynkAdapter();

    void attachErrorManager(ErrorManager* errorManager);

    // spec: begin() - takes the auth token directly (template ID/name
    // are compile-time macros per Blynk's own requirements, see
    // class header note).
    void begin(const char* authToken, const char* wifiSsid, const char* wifiPassword);

    void mapTemperature(uint8_t virtualPin);
    void mapHumidity(uint8_t virtualPin);
    void mapPressure(uint8_t virtualPin);
    void mapDewPoint(uint8_t virtualPin);
    void mapApparentTemperature(uint8_t virtualPin);

    void mapRain(uint8_t virtualPin);
    void mapPrecipitation(uint8_t virtualPin);

    void mapWindSpeed(uint8_t virtualPin);
    void mapWindDirection(uint8_t virtualPin);
    void mapWindGusts(uint8_t virtualPin);

    void mapCloudCover(uint8_t virtualPin);
    void mapUVIndex(uint8_t virtualPin);
    void mapSolarRadiation(uint8_t virtualPin);

    void mapSoilTemperature(uint8_t virtualPin);
    void mapSoilMoisture(uint8_t virtualPin);

    bool publishClimateData(const ClimateData& data) override;

    // PlatformAdapter interface
    bool connect() override;
    void disconnect() override;
    bool isConnected() const override;
    void update() override; // calls Blynk.run()
    PlatformType getType() const override { return PlatformType::BLYNK; }
    String getName() const override { return "Blynk"; }
    bool isConfigured() const override { return _hasCredentials; }

private:
    BlynkPinMap _pins;
    bool _hasCredentials;
    bool _began;
    ErrorManager* _errorManager;

    void setError(ErrorCode code, const char* msg);
    void writeIfMapped(int8_t pin, bool hasValue, float value);
};

} // namespace AmelTech

#endif // AMELTECH_BLYNK_ADAPTER_H
