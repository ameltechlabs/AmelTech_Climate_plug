/*
 * TXManager.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "TXManager.h"
#include <ArduinoJson.h>

namespace AmelTech {

TXManager::TXManager()
    : _http(nullptr), _errorManager(nullptr), _url(""),
      _method(URLMethod::POST), _format(URLFormat::JSON), _enabled(false) {
}

void TXManager::attachHTTP(HTTPManager* http) {
    _http = http;
}

void TXManager::attachErrorManager(ErrorManager* errorManager) {
    _errorManager = errorManager;
}

void TXManager::setError(ErrorCode code, const char* msg) {
    if (_errorManager) _errorManager->setError(code, msg);
}

void TXManager::setTXURL(const String& url) {
    _url = url;
}

String TXManager::getTXURL() const {
    return _url;
}

void TXManager::setTXMethod(URLMethod method) {
    _method = method;
}

URLMethod TXManager::getTXMethod() const {
    return _method;
}

void TXManager::setTXFormat(URLFormat format) {
    _format = format;
}

URLFormat TXManager::getTXFormat() const {
    return _format;
}

bool TXManager::isTXEnabled() const {
    return _enabled;
}

void TXManager::enableTX() {
    _enabled = true;
}

void TXManager::disableTX() {
    _enabled = false;
}

bool TXManager::send(const String& body, const String& contentType) {
    if (!_enabled) {
        setError(ErrorCode::CONFIG_ERROR, "TX is disabled - call enableTX() first");
        return false;
    }
    if (_url.length() == 0) {
        setError(ErrorCode::URL_INVALID, "No TX URL configured");
        return false;
    }
    if (_http == nullptr) {
        setError(ErrorCode::HTTP_ERROR, "No HTTPManager attached to TXManager");
        return false;
    }

    HTTPResponse resp;
    switch (_method) {
        case URLMethod::POST:  resp = _http->POST(_url, body, contentType); break;
        case URLMethod::PUT:   resp = _http->PUT(_url, body, contentType); break;
        case URLMethod::PATCH: resp = _http->PATCH(_url, body, contentType); break;
        case URLMethod::GET:
        default:
            // GET has no body in HTTP semantics; treat as a plain GET,
            // ignoring `body` rather than silently misusing the verb.
            resp = _http->GET(_url);
            break;
    }

    if (!resp.success) {
        setError(resp.error, "TX send failed");
        return false;
    }
    return true;
}

String TXManager::climateDataToJSON(const ClimateData& data) const {
    DynamicJsonDocument doc(2048);

    if (data.hasTemperature) doc["temperature"] = data.temperature;
    if (data.hasRelativeHumidity) doc["relative_humidity"] = data.relativeHumidity;
    if (data.hasDewPoint) doc["dew_point"] = data.dewPoint;
    if (data.hasApparentTemperature) doc["apparent_temperature"] = data.apparentTemperature;
    if (data.hasAtmosphericPressure) doc["pressure"] = data.atmosphericPressure;
    if (data.hasPrecipitation) doc["precipitation"] = data.precipitation;
    if (data.hasRain) doc["rain"] = data.rain;
    if (data.hasSnowfall) doc["snowfall"] = data.snowfall;
    if (data.hasWeatherCode) doc["weather_code"] = data.weatherCode;
    if (data.hasWeatherCondition) doc["weather_condition"] = data.weatherCondition;
    if (data.hasCloudCover) doc["cloud_cover"] = data.cloudCover;
    if (data.hasWindSpeed) doc["wind_speed"] = data.windSpeed;
    if (data.hasWindDirection) doc["wind_direction"] = data.windDirection;
    if (data.hasWindGusts) doc["wind_gusts"] = data.windGusts;
    if (data.hasSolarRadiation) doc["solar_radiation"] = data.solarRadiation;
    if (data.hasUvIndex) doc["uv_index"] = data.uvIndex;
    if (data.hasEvapotranspiration) doc["evapotranspiration"] = data.evapotranspiration;
    if (data.hasSoilTemperature) doc["soil_temperature"] = data.soilTemperature;
    if (data.hasSoilMoisture) doc["soil_moisture"] = data.soilMoisture;
    if (data.hasFreezingLevel) doc["freezing_level"] = data.freezingLevel;
    if (data.hasVisibility) doc["visibility"] = data.visibility;
    if (data.hasCape) doc["cape"] = data.cape;

    doc["source"] = dataSourceToString(data.source);
    doc["valid"] = data.valid;
    doc["stale"] = data.stale;

    String out;
    serializeJson(doc, out);
    return out;
}

bool TXManager::sendClimateData(const ClimateData& data) {
    return send(climateDataToJSON(data), "application/json");
}

bool TXManager::sendSensorData(const String sensorNames[], const float sensorValues[], uint8_t count) {
    DynamicJsonDocument doc(1024);
    for (uint8_t i = 0; i < count; i++) {
        doc[sensorNames[i]] = sensorValues[i];
    }
    String out;
    serializeJson(doc, out);
    return send(out, "application/json");
}

} // namespace AmelTech
