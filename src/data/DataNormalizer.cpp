/*
 * DataNormalizer.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "DataNormalizer.h"

namespace AmelTech {

DataNormalizer::DataNormalizer() {
}

float DataNormalizer::fahrenheitToCelsius(float f) {
    return (f - 32.0f) * 5.0f / 9.0f;
}

float DataNormalizer::celsiusToFahrenheit(float c) {
    return (c * 9.0f / 5.0f) + 32.0f;
}

float DataNormalizer::mphToKmh(float mph) {
    return mph * 1.60934f;
}

float DataNormalizer::msToKmh(float ms) {
    return ms * 3.6f;
}

float DataNormalizer::knotsToKmh(float knots) {
    return knots * 1.852f;
}

float DataNormalizer::inchesToMm(float inches) {
    return inches * 25.4f;
}

float DataNormalizer::hPaToInHg(float hPa) {
    return hPa * 0.02953f;
}

float DataNormalizer::normalizeWindDirection(float degrees) {
    float d = fmodf(degrees, 360.0f);
    if (d < 0) d += 360.0f;
    return d;
}

// WMO Weather interpretation codes, as used by Open-Meteo.
// Reference: open-meteo.com/en/docs (WMO Weather interpretation codes table)
String DataNormalizer::weatherCodeToCondition(int code) {
    switch (code) {
        case 0:  return "Clear sky";
        case 1:  return "Mainly clear";
        case 2:  return "Partly cloudy";
        case 3:  return "Overcast";
        case 45: return "Fog";
        case 48: return "Depositing rime fog";
        case 51: return "Light drizzle";
        case 53: return "Moderate drizzle";
        case 55: return "Dense drizzle";
        case 56: return "Light freezing drizzle";
        case 57: return "Dense freezing drizzle";
        case 61: return "Slight rain";
        case 63: return "Moderate rain";
        case 65: return "Heavy rain";
        case 66: return "Light freezing rain";
        case 67: return "Heavy freezing rain";
        case 71: return "Slight snowfall";
        case 73: return "Moderate snowfall";
        case 75: return "Heavy snowfall";
        case 77: return "Snow grains";
        case 80: return "Slight rain showers";
        case 81: return "Moderate rain showers";
        case 82: return "Violent rain showers";
        case 85: return "Slight snow showers";
        case 86: return "Heavy snow showers";
        case 95: return "Thunderstorm";
        case 96: return "Thunderstorm, slight hail";
        case 99: return "Thunderstorm, heavy hail";
        default: return "Unknown";
    }
}

void DataNormalizer::normalize(ClimateData& data) {
    if (data.hasWindDirection) {
        data.windDirection = normalizeWindDirection(data.windDirection);
    }
    if (data.hasWeatherCode && !data.hasWeatherCondition) {
        data.weatherCondition = weatherCodeToCondition(data.weatherCode);
        data.hasWeatherCondition = true;
    }
}

} // namespace AmelTech
