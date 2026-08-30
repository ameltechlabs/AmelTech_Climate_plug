/*
 * DataValidator.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "DataValidator.h"

namespace AmelTech {

DataValidator::DataValidator() {
}

bool DataValidator::validateTemperature(float celsius) const {
    return celsius >= TEMP_MIN_C && celsius <= TEMP_MAX_C;
}

bool DataValidator::validateHumidity(float percent) const {
    return percent >= HUMIDITY_MIN && percent <= HUMIDITY_MAX;
}

bool DataValidator::validatePressure(float hPa) const {
    return hPa >= PRESSURE_MIN_HPA && hPa <= PRESSURE_MAX_HPA;
}

bool DataValidator::validateWind(float speedKmh) const {
    return speedKmh >= WIND_MIN_KMH && speedKmh <= WIND_MAX_KMH;
}

bool DataValidator::validatePrecipitation(float mm) const {
    return mm >= PRECIP_MIN_MM && mm <= PRECIP_MAX_MM;
}

bool DataValidator::validateSolar(float wm2) const {
    return wm2 >= SOLAR_MIN_WM2 && wm2 <= SOLAR_MAX_WM2;
}

bool DataValidator::validateUV(float index) const {
    return index >= UV_MIN && index <= UV_MAX;
}

bool DataValidator::validateSoilData(float value) const {
    return value >= SOIL_MIN && value <= SOIL_MAX;
}

bool DataValidator::validateClimateData(ClimateData& data) {
    bool ok = true;

    if (data.hasTemperature && !validateTemperature(data.temperature)) ok = false;
    if (data.hasApparentTemperature && !validateTemperature(data.apparentTemperature)) ok = false;
    if (data.hasDewPoint && !validateTemperature(data.dewPoint)) ok = false;
    if (data.hasRelativeHumidity && !validateHumidity(data.relativeHumidity)) ok = false;
    if (data.hasAtmosphericPressure && !validatePressure(data.atmosphericPressure)) ok = false;
    if (data.hasWindSpeed && !validateWind(data.windSpeed)) ok = false;
    if (data.hasWindGusts && !validateWind(data.windGusts)) ok = false;
    if (data.hasPrecipitation && !validatePrecipitation(data.precipitation)) ok = false;
    if (data.hasRain && !validatePrecipitation(data.rain)) ok = false;
    if (data.hasSnowfall && !validatePrecipitation(data.snowfall)) ok = false;
    if (data.hasSolarRadiation && !validateSolar(data.solarRadiation)) ok = false;
    if (data.hasUvIndex && !validateUV(data.uvIndex)) ok = false;
    if (data.hasSoilTemperature && !validateTemperature(data.soilTemperature)) ok = false;
    if (data.hasSoilMoisture && !validateSoilData(data.soilMoisture)) ok = false;

    // Cloud cover and humidity share the 0-100 bound.
    if (data.hasCloudCover && !validateHumidity(data.cloudCover)) ok = false;

    // Wind direction must be a compass bearing.
    if (data.hasWindDirection && (data.windDirection < 0.0f || data.windDirection > 360.0f)) ok = false;

    data.valid = ok;
    return ok;
}

bool DataValidator::isDataValid(const ClimateData& data) const {
    return data.valid;
}

bool DataValidator::isDataStale(const ClimateData& data, unsigned long lifetimeMs) const {
    if (data.receivedAt == 0) return true; // never populated
    unsigned long age = millis() - data.receivedAt; // wraps safely with unsigned arithmetic
    return age > lifetimeMs;
}

} // namespace AmelTech
