/*
 * ClimateData.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "ClimateData.h"

namespace AmelTech {

const char* dataSourceToString(DataSource source) {
    switch (source) {
        case DataSource::NONE:   return "NONE";
        case DataSource::REMOTE: return "REMOTE";
        case DataSource::LOCAL:  return "LOCAL";
        case DataSource::HYBRID: return "HYBRID";
        case DataSource::CACHE:  return "CACHE";
        default:                 return "UNKNOWN";
    }
}

ClimateData::ClimateData() {
    reset();
}

void ClimateData::reset() {
    temperature = 0.0f;            hasTemperature = false;
    relativeHumidity = 0.0f;       hasRelativeHumidity = false;
    dewPoint = 0.0f;                hasDewPoint = false;
    apparentTemperature = 0.0f;    hasApparentTemperature = false;

    atmosphericPressure = 0.0f;    hasAtmosphericPressure = false;

    precipitation = 0.0f;          hasPrecipitation = false;
    rain = 0.0f;                   hasRain = false;
    snowfall = 0.0f;                hasSnowfall = false;

    weatherCode = -1;              hasWeatherCode = false;
    weatherCondition = "";         hasWeatherCondition = false;

    cloudCover = 0.0f;              hasCloudCover = false;
    windSpeed = 0.0f;                hasWindSpeed = false;
    windDirection = 0.0f;           hasWindDirection = false;
    windGusts = 0.0f;                hasWindGusts = false;

    solarRadiation = 0.0f;          hasSolarRadiation = false;
    uvIndex = 0.0f;                  hasUvIndex = false;
    evapotranspiration = 0.0f;      hasEvapotranspiration = false;

    soilTemperature = 0.0f;         hasSoilTemperature = false;
    soilMoisture = 0.0f;             hasSoilMoisture = false;

    freezingLevel = 0.0f;           hasFreezingLevel = false;
    visibility = 0.0f;                hasVisibility = false;
    cape = 0.0f;                       hasCape = false;

    timestamp = 0;
    receivedAt = 0;
    source = DataSource::NONE;
    valid = false;
    stale = false;
}

void ClimateData::mergeFrom(const ClimateData& other) {
    if (other.hasTemperature)         { temperature = other.temperature; hasTemperature = true; }
    if (other.hasRelativeHumidity)    { relativeHumidity = other.relativeHumidity; hasRelativeHumidity = true; }
    if (other.hasDewPoint)            { dewPoint = other.dewPoint; hasDewPoint = true; }
    if (other.hasApparentTemperature) { apparentTemperature = other.apparentTemperature; hasApparentTemperature = true; }

    if (other.hasAtmosphericPressure) { atmosphericPressure = other.atmosphericPressure; hasAtmosphericPressure = true; }

    if (other.hasPrecipitation)       { precipitation = other.precipitation; hasPrecipitation = true; }
    if (other.hasRain)                { rain = other.rain; hasRain = true; }
    if (other.hasSnowfall)            { snowfall = other.snowfall; hasSnowfall = true; }

    if (other.hasWeatherCode)         { weatherCode = other.weatherCode; hasWeatherCode = true; }
    if (other.hasWeatherCondition)    { weatherCondition = other.weatherCondition; hasWeatherCondition = true; }

    if (other.hasCloudCover)          { cloudCover = other.cloudCover; hasCloudCover = true; }
    if (other.hasWindSpeed)           { windSpeed = other.windSpeed; hasWindSpeed = true; }
    if (other.hasWindDirection)       { windDirection = other.windDirection; hasWindDirection = true; }
    if (other.hasWindGusts)           { windGusts = other.windGusts; hasWindGusts = true; }

    if (other.hasSolarRadiation)      { solarRadiation = other.solarRadiation; hasSolarRadiation = true; }
    if (other.hasUvIndex)             { uvIndex = other.uvIndex; hasUvIndex = true; }
    if (other.hasEvapotranspiration)  { evapotranspiration = other.evapotranspiration; hasEvapotranspiration = true; }

    if (other.hasSoilTemperature)     { soilTemperature = other.soilTemperature; hasSoilTemperature = true; }
    if (other.hasSoilMoisture)        { soilMoisture = other.soilMoisture; hasSoilMoisture = true; }

    if (other.hasFreezingLevel)       { freezingLevel = other.freezingLevel; hasFreezingLevel = true; }
    if (other.hasVisibility)          { visibility = other.visibility; hasVisibility = true; }
    if (other.hasCape)                { cape = other.cape; hasCape = true; }

    // Metadata: take the newer receivedAt, and mark HYBRID if sources differ.
    if (other.receivedAt > receivedAt) {
        timestamp = other.timestamp;
        receivedAt = other.receivedAt;
    }
    if (source == DataSource::NONE) {
        source = other.source;
    } else if (source != other.source) {
        source = DataSource::HYBRID;
    }
    valid = valid || other.valid;
    stale = stale && other.stale; // only stale if BOTH contributions are stale
}

} // namespace AmelTech
