/*
 * DataValidator.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * Physically-plausible range checks for each climate parameter.
 * These are sanity bounds (catch corrupted/garbage responses), not
 * meteorological limits - e.g. temperature range is generous enough
 * to cover any inhabited location on Earth.
 */

#ifndef AMELTECH_DATA_VALIDATOR_H
#define AMELTECH_DATA_VALIDATOR_H

#include "ClimateData.h"

namespace AmelTech {

class DataValidator {
public:
    DataValidator();

    // Runs all applicable sub-validators against populated (hasX==true)
    // fields only. Sets data.valid accordingly and returns the result.
    bool validateClimateData(ClimateData& data);

    // Individual field validators. Each takes the raw value (not the
    // struct) so they're reusable outside of a full ClimateData, e.g.
    // by CustomProvider parsing or unit tests.
    bool validateTemperature(float celsius) const;
    bool validateHumidity(float percent) const;
    bool validatePressure(float hPa) const;
    bool validateWind(float speedKmh) const;
    bool validatePrecipitation(float mm) const;
    bool validateSolar(float wm2) const;
    bool validateUV(float index) const;
    bool validateSoilData(float value) const;

    // Convenience wrappers matching the spec's exact function names.
    bool isTemperatureValid(float celsius) const { return validateTemperature(celsius); }
    bool isHumidityValid(float percent) const { return validateHumidity(percent); }
    bool isPressureValid(float hPa) const { return validatePressure(hPa); }
    bool isWindValid(float speedKmh) const { return validateWind(speedKmh); }
    bool isRainValid(float mm) const { return validatePrecipitation(mm); }
    bool isUVValid(float index) const { return validateUV(index); }
    bool isSoilDataValid(float value) const { return validateSoilData(value); }

    bool isDataValid(const ClimateData& data) const;
    bool isDataStale(const ClimateData& data, unsigned long lifetimeMs) const;

private:
    // Bounds, chosen generously to avoid rejecting legitimate extreme
    // weather while still catching garbage (e.g. NaN-derived or
    // corrupted-response values).
    static constexpr float TEMP_MIN_C = -90.0f;
    static constexpr float TEMP_MAX_C = 60.0f;
    static constexpr float HUMIDITY_MIN = 0.0f;
    static constexpr float HUMIDITY_MAX = 100.0f;
    static constexpr float PRESSURE_MIN_HPA = 850.0f;
    static constexpr float PRESSURE_MAX_HPA = 1085.0f;
    static constexpr float WIND_MIN_KMH = 0.0f;
    static constexpr float WIND_MAX_KMH = 500.0f;
    static constexpr float PRECIP_MIN_MM = 0.0f;
    static constexpr float PRECIP_MAX_MM = 1000.0f;
    static constexpr float SOLAR_MIN_WM2 = 0.0f;
    static constexpr float SOLAR_MAX_WM2 = 1500.0f;
    static constexpr float UV_MIN = 0.0f;
    static constexpr float UV_MAX = 20.0f;
    static constexpr float SOIL_MIN = -50.0f;
    static constexpr float SOIL_MAX = 80.0f;
};

} // namespace AmelTech

#endif // AMELTECH_DATA_VALIDATOR_H
