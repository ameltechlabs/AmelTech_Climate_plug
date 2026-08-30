/*
 * ParameterRegistry.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "ParameterRegistry.h"

namespace AmelTech {

const char* ParameterRegistry::toQueryToken(ClimateParameter param) {
    switch (param) {
        case ClimateParameter::TEMPERATURE_2M:        return "temperature_2m";
        case ClimateParameter::RELATIVE_HUMIDITY_2M:  return "relative_humidity_2m";
        case ClimateParameter::DEW_POINT_2M:          return "dew_point_2m";
        case ClimateParameter::APPARENT_TEMPERATURE:  return "apparent_temperature";
        case ClimateParameter::PRESSURE_MSL:          return "pressure_msl";
        case ClimateParameter::PRECIPITATION:         return "precipitation";
        case ClimateParameter::RAIN:                  return "rain";
        case ClimateParameter::SNOWFALL:               return "snowfall";
        case ClimateParameter::WEATHER_CODE:          return "weather_code";
        case ClimateParameter::CLOUD_COVER:           return "cloud_cover";
        case ClimateParameter::WIND_SPEED_10M:        return "wind_speed_10m";
        case ClimateParameter::WIND_DIRECTION_10M:    return "wind_direction_10m";
        case ClimateParameter::WIND_GUSTS_10M:        return "wind_gusts_10m";
        case ClimateParameter::SHORTWAVE_RADIATION:   return "shortwave_radiation";
        case ClimateParameter::UV_INDEX:              return "uv_index";
        case ClimateParameter::EVAPOTRANSPIRATION:    return "evapotranspiration";
        case ClimateParameter::SOIL_TEMPERATURE_0CM:  return "soil_temperature_0cm";
        case ClimateParameter::SOIL_MOISTURE_0_1CM:   return "soil_moisture_0_to_1cm";
        case ClimateParameter::FREEZING_LEVEL_HEIGHT: return "freezing_level_height";
        case ClimateParameter::VISIBILITY:            return "visibility";
        case ClimateParameter::CAPE:                  return "cape";
        default:                                      return "";
    }
}

bool ParameterRegistry::isHourlyOnly(ClimateParameter param) {
    // As of Open-Meteo's documented "current" weather variable list,
    // these are not guaranteed in the `current` block on all model
    // configurations and are requested via `hourly` instead, with
    // OpenMeteoProvider reading the first (current-hour) index.
    switch (param) {
        case ClimateParameter::FREEZING_LEVEL_HEIGHT:
        case ClimateParameter::VISIBILITY:
        case ClimateParameter::CAPE:
        case ClimateParameter::EVAPOTRANSPIRATION:
        case ClimateParameter::SOIL_TEMPERATURE_0CM:
        case ClimateParameter::SOIL_MOISTURE_0_1CM:
            return true;
        default:
            return false;
    }
}

uint8_t ParameterRegistry::count() {
    return (uint8_t)ClimateParameter::PARAMETER_COUNT;
}

} // namespace AmelTech
