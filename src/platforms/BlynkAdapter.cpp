/*
 * BlynkAdapter.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "BlynkAdapter.h"

namespace AmelTech {

BlynkPinMap::BlynkPinMap()
    : temperature(-1), humidity(-1), pressure(-1), dewPoint(-1), apparentTemperature(-1),
      rain(-1), precipitation(-1), windSpeed(-1), windDirection(-1), windGusts(-1),
      cloudCover(-1), uvIndex(-1), solarRadiation(-1), soilTemperature(-1), soilMoisture(-1) {
}

BlynkAdapter::BlynkAdapter()
    : _hasCredentials(false), _began(false), _errorManager(nullptr) {
}

void BlynkAdapter::attachErrorManager(ErrorManager* errorManager) {
    _errorManager = errorManager;
}

void BlynkAdapter::setError(ErrorCode code, const char* msg) {
    if (_errorManager) _errorManager->setError(code, msg);
}

void BlynkAdapter::begin(const char* authToken, const char* wifiSsid, const char* wifiPassword) {
    if (authToken == nullptr || strlen(authToken) == 0) {
        setError(ErrorCode::PLATFORM_ERROR, "Blynk auth token is empty");
        _hasCredentials = false;
        return;
    }

    // Blynk.begin() is itself a blocking call that waits for the
    // cloud connection - this mirrors the library's own documented
    // usage pattern (called once during setup(), same as
    // ClimateCore::bootStageWiFi's one-time blocking wait during
    // begin()). Runtime reconnection afterward is handled by
    // Blynk.run() inside update(), which is non-blocking.
    Blynk.begin(authToken, wifiSsid, wifiPassword);

    _hasCredentials = true;
    _began = true;
}

void BlynkAdapter::mapTemperature(uint8_t virtualPin) { _pins.temperature = (int8_t)virtualPin; }
void BlynkAdapter::mapHumidity(uint8_t virtualPin) { _pins.humidity = (int8_t)virtualPin; }
void BlynkAdapter::mapPressure(uint8_t virtualPin) { _pins.pressure = (int8_t)virtualPin; }
void BlynkAdapter::mapDewPoint(uint8_t virtualPin) { _pins.dewPoint = (int8_t)virtualPin; }
void BlynkAdapter::mapApparentTemperature(uint8_t virtualPin) { _pins.apparentTemperature = (int8_t)virtualPin; }
void BlynkAdapter::mapRain(uint8_t virtualPin) { _pins.rain = (int8_t)virtualPin; }
void BlynkAdapter::mapPrecipitation(uint8_t virtualPin) { _pins.precipitation = (int8_t)virtualPin; }
void BlynkAdapter::mapWindSpeed(uint8_t virtualPin) { _pins.windSpeed = (int8_t)virtualPin; }
void BlynkAdapter::mapWindDirection(uint8_t virtualPin) { _pins.windDirection = (int8_t)virtualPin; }
void BlynkAdapter::mapWindGusts(uint8_t virtualPin) { _pins.windGusts = (int8_t)virtualPin; }
void BlynkAdapter::mapCloudCover(uint8_t virtualPin) { _pins.cloudCover = (int8_t)virtualPin; }
void BlynkAdapter::mapUVIndex(uint8_t virtualPin) { _pins.uvIndex = (int8_t)virtualPin; }
void BlynkAdapter::mapSolarRadiation(uint8_t virtualPin) { _pins.solarRadiation = (int8_t)virtualPin; }
void BlynkAdapter::mapSoilTemperature(uint8_t virtualPin) { _pins.soilTemperature = (int8_t)virtualPin; }
void BlynkAdapter::mapSoilMoisture(uint8_t virtualPin) { _pins.soilMoisture = (int8_t)virtualPin; }

void BlynkAdapter::writeIfMapped(int8_t pin, bool hasValue, float value) {
    if (pin < 0 || !hasValue) return; // unmapped or unavailable - skip, never write a fabricated value
    Blynk.virtualWrite(pin, value);
}

bool BlynkAdapter::publishClimateData(const ClimateData& data) {
    if (!isConnected()) {
        setError(ErrorCode::PLATFORM_ERROR, "Blynk not connected");
        return false;
    }

    writeIfMapped(_pins.temperature, data.hasTemperature, data.temperature);
    writeIfMapped(_pins.humidity, data.hasRelativeHumidity, data.relativeHumidity);
    writeIfMapped(_pins.pressure, data.hasAtmosphericPressure, data.atmosphericPressure);
    writeIfMapped(_pins.dewPoint, data.hasDewPoint, data.dewPoint);
    writeIfMapped(_pins.apparentTemperature, data.hasApparentTemperature, data.apparentTemperature);
    writeIfMapped(_pins.rain, data.hasRain, data.rain);
    writeIfMapped(_pins.precipitation, data.hasPrecipitation, data.precipitation);
    writeIfMapped(_pins.windSpeed, data.hasWindSpeed, data.windSpeed);
    writeIfMapped(_pins.windDirection, data.hasWindDirection, data.windDirection);
    writeIfMapped(_pins.windGusts, data.hasWindGusts, data.windGusts);
    writeIfMapped(_pins.cloudCover, data.hasCloudCover, data.cloudCover);
    writeIfMapped(_pins.uvIndex, data.hasUvIndex, data.uvIndex);
    writeIfMapped(_pins.solarRadiation, data.hasSolarRadiation, data.solarRadiation);
    writeIfMapped(_pins.soilTemperature, data.hasSoilTemperature, data.soilTemperature);
    writeIfMapped(_pins.soilMoisture, data.hasSoilMoisture, data.soilMoisture);

    return true;
}

bool BlynkAdapter::connect() {
    // Blynk's own begin() already establishes the connection; this
    // just reports current status rather than re-initiating.
    return isConnected();
}

void BlynkAdapter::disconnect() {
    Blynk.disconnect();
}

bool BlynkAdapter::isConnected() const {
    return _began && Blynk.connected();
}

void BlynkAdapter::update() {
    if (_began) {
        Blynk.run();
    }
}

} // namespace AmelTech
