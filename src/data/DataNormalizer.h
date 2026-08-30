/*
 * DataNormalizer.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * Normalizes provider-specific units/encodings into the fixed units
 * ClimateData uses internally (metric: Celsius, hPa, km/h, mm, %).
 * Open-Meteo already returns metric by default when requested that
 * way, but this module exists so a future non-metric provider or a
 * custom user endpoint can still populate ClimateData consistently.
 */

#ifndef AMELTECH_DATA_NORMALIZER_H
#define AMELTECH_DATA_NORMALIZER_H

#include "ClimateData.h"

namespace AmelTech {

class DataNormalizer {
public:
    DataNormalizer();

    // Unit conversions. Pure functions - safe to call standalone.
    static float fahrenheitToCelsius(float f);
    static float celsiusToFahrenheit(float c);

    static float mphToKmh(float mph);
    static float msToKmh(float ms);
    static float knotsToKmh(float knots);

    static float inchesToMm(float inches);

    static float hPaToInHg(float hPa);

    // Maps an Open-Meteo WMO weather code to a short human-readable
    // condition string (e.g. 61 -> "Slight rain"). Returns "Unknown"
    // for unrecognized codes rather than guessing.
    static String weatherCodeToCondition(int code);

    // Normalizes wind direction to the [0, 360) range.
    static float normalizeWindDirection(float degrees);

    // Applies normalization in-place to fields already flagged hasX.
    // Currently a pass-through hook for future non-metric providers;
    // Open-Meteo values are already in the units ClimateData expects.
    void normalize(ClimateData& data);
};

} // namespace AmelTech

#endif // AMELTECH_DATA_NORMALIZER_H
