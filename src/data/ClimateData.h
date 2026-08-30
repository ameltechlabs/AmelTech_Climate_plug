/*
 * ClimateData.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * RULE 4 (spec): ClimateData is the central normalized data contract.
 * Every provider, sensor, and platform adapter reads/writes through
 * this struct rather than passing raw JSON or sensor values around.
 *
 * RULE 7 (spec): missing values are represented as "unavailable"
 * (via the `has*` flags below), never fabricated as 0.0.
 */

#ifndef AMELTECH_CLIMATE_DATA_H
#define AMELTECH_CLIMATE_DATA_H

#include <Arduino.h>

namespace AmelTech {

enum class DataSource {
    NONE,
    REMOTE,      // came from a Provider (e.g. Open-Meteo)
    LOCAL,       // came from a local sensor
    HYBRID,      // merged remote + local
    CACHE        // served from cache
};

const char* dataSourceToString(DataSource source);

/*
 * ClimateData
 *
 * NOTE ON "has*" FLAGS:
 * Open-Meteo (and other providers) do not return every parameter for
 * every location/endpoint combination. Rather than default a missing
 * field to 0.0 (which would be silently wrong - e.g. "0 mm rain" vs
 * "we don't know"), each field has a matching hasX boolean. Callers
 * MUST check hasX before trusting the value. The public getters in
 * AmelTechClimate enforce this and return NAN / -1 sentinels plus
 * DATA_UNAVAILABLE when hasX is false - see AmelTech_Climate_plug.cpp.
 */
struct ClimateData {
    // --- Temperature group ---
    float temperature;            bool hasTemperature;
    float relativeHumidity;       bool hasRelativeHumidity;
    float dewPoint;               bool hasDewPoint;
    float apparentTemperature;    bool hasApparentTemperature;

    // --- Pressure ---
    float atmosphericPressure;    bool hasAtmosphericPressure;

    // --- Precipitation ---
    float precipitation;          bool hasPrecipitation;
    float rain;                   bool hasRain;
    float snowfall;               bool hasSnowfall;

    // --- Weather description ---
    int   weatherCode;            bool hasWeatherCode;
    String weatherCondition;      bool hasWeatherCondition;

    // --- Sky / wind ---
    float cloudCover;             bool hasCloudCover;
    float windSpeed;              bool hasWindSpeed;
    float windDirection;          bool hasWindDirection;
    float windGusts;              bool hasWindGusts;

    // --- Solar ---
    float solarRadiation;         bool hasSolarRadiation;
    float uvIndex;                bool hasUvIndex;
    float evapotranspiration;     bool hasEvapotranspiration;

    // --- Soil ---
    float soilTemperature;        bool hasSoilTemperature;
    float soilMoisture;           bool hasSoilMoisture;

    // --- Misc atmospheric ---
    float freezingLevel;          bool hasFreezingLevel;
    float visibility;             bool hasVisibility;
    float cape;                   bool hasCape;

    // --- Metadata ---
    unsigned long timestamp;      // provider-reported time (epoch secs), 0 if unknown
    unsigned long receivedAt;     // millis() when this struct was populated
    DataSource source;
    bool valid;                   // passed DataValidator checks
    bool stale;                   // exceeded cache/staleness lifetime

    ClimateData();

    // Reset every field to "unavailable" / defaults.
    void reset();

    // Merge `other` into `this`, only overwriting fields where
    // other.hasX is true. Used by hybrid mode (local sensor over remote).
    void mergeFrom(const ClimateData& other);
};

} // namespace AmelTech

#endif // AMELTECH_CLIMATE_DATA_H
