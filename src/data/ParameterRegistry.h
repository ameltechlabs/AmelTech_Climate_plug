/*
 * ParameterRegistry.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * Maps each ClimateData field to its corresponding Open-Meteo
 * "current" API variable name (per open-meteo.com/en/docs). This is
 * the single source of truth OpenMeteoProvider's RequestBuilder and
 * ResponseManager use, so parameter names live in one place.
 *
 * Only variables Open-Meteo actually documents are listed here - see
 * spec rule "Do not invent unsupported parameters."
 */

#ifndef AMELTECH_PARAMETER_REGISTRY_H
#define AMELTECH_PARAMETER_REGISTRY_H

#include <Arduino.h>

namespace AmelTech {

enum class ClimateParameter {
    TEMPERATURE_2M,
    RELATIVE_HUMIDITY_2M,
    DEW_POINT_2M,
    APPARENT_TEMPERATURE,
    PRESSURE_MSL,
    PRECIPITATION,
    RAIN,
    SNOWFALL,
    WEATHER_CODE,
    CLOUD_COVER,
    WIND_SPEED_10M,
    WIND_DIRECTION_10M,
    WIND_GUSTS_10M,
    SHORTWAVE_RADIATION,
    UV_INDEX,
    EVAPOTRANSPIRATION,
    SOIL_TEMPERATURE_0CM,
    SOIL_MOISTURE_0_1CM,
    FREEZING_LEVEL_HEIGHT,
    VISIBILITY,
    CAPE,

    PARAMETER_COUNT // sentinel, not a real parameter
};

/*
 * ParameterRegistry
 *
 * Static lookup table. No instance state needed since the mapping
 * is fixed by the Open-Meteo API contract.
 */
class ParameterRegistry {
public:
    // The exact query-string token Open-Meteo expects, e.g. "temperature_2m".
    static const char* toQueryToken(ClimateParameter param);

    // Whether this parameter is available on Open-Meteo's "current"
    // block (as opposed to hourly/daily-only). All PARAMETER_COUNT
    // entries here are current-block-eligible as of this library's
    // design; CAPE and freezing_level_height are hourly-only on some
    // Open-Meteo deployments, so OpenMeteoProvider requests them via
    // the hourly array and reads index 0 (see OpenMeteoProvider.cpp).
    static bool isHourlyOnly(ClimateParameter param);

    // Total number of known parameters.
    static uint8_t count();
};

} // namespace AmelTech

#endif // AMELTECH_PARAMETER_REGISTRY_H
