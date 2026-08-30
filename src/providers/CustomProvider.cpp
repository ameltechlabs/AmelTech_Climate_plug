/*
 * CustomProvider.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "CustomProvider.h"
#include <ArduinoJson.h>

namespace AmelTech {

CustomProvider::CustomProvider() : _endpoint("") {
}

void CustomProvider::setEndpoint(const String& url) {
    _endpoint = url;
}

String CustomProvider::getEndpoint() const {
    return _endpoint;
}

String CustomProvider::buildRequestURL(float latitude, float longitude) const {
    if (_endpoint.length() == 0) return "";

    // If the endpoint already has query params, append with &, else ?.
    String sep = (_endpoint.indexOf('?') == -1) ? "?" : "&";
    String url = _endpoint + sep + "lat=" + String(latitude, 6) + "&lon=" + String(longitude, 6);
    return url;
}

bool CustomProvider::tryReadFloat(const String& jsonBody, const char* const* candidates, uint8_t count, float& out) const {
    DynamicJsonDocument doc(8192);
    if (deserializeJson(doc, jsonBody)) return false;

    for (uint8_t i = 0; i < count; i++) {
        if (doc.containsKey(candidates[i])) {
            out = doc[candidates[i]].as<float>();
            return true;
        }
    }
    return false;
}

bool CustomProvider::parseResponse(const String& jsonBody, ClimateData& out, ErrorCode& errorOut) const {
    out.reset();

    DynamicJsonDocument doc(8192);
    DeserializationError err = deserializeJson(doc, jsonBody);
    if (err) {
        errorOut = ErrorCode::JSON_ERROR;
        return false;
    }

    if (doc.isNull() || doc.size() == 0) {
        errorOut = ErrorCode::RESPONSE_ERROR;
        return false;
    }

    // Best-effort field detection across common naming variants.
    const char* tempKeys[] = {"temperature", "temp", "temperature_2m"};
    const char* humKeys[]  = {"humidity", "relative_humidity", "relative_humidity_2m"};
    const char* pressKeys[] = {"pressure", "atmospheric_pressure", "pressure_msl"};
    const char* windKeys[] = {"wind_speed", "windSpeed", "wind"};

    float v;
    if (tryReadFloat(jsonBody, tempKeys, 3, v)) { out.temperature = v; out.hasTemperature = true; }
    if (tryReadFloat(jsonBody, humKeys, 3, v))  { out.relativeHumidity = v; out.hasRelativeHumidity = true; }
    if (tryReadFloat(jsonBody, pressKeys, 3, v)) { out.atmosphericPressure = v; out.hasAtmosphericPressure = true; }
    if (tryReadFloat(jsonBody, windKeys, 3, v)) { out.windSpeed = v; out.hasWindSpeed = true; }

    out.receivedAt = millis();
    out.source = DataSource::REMOTE;

    if (!out.hasTemperature && !out.hasRelativeHumidity && !out.hasAtmosphericPressure && !out.hasWindSpeed) {
        // Nothing recognizable was found - not necessarily an error in
        // the JSON itself, but unusable as climate data.
        errorOut = ErrorCode::DATA_UNAVAILABLE;
        return false;
    }

    errorOut = ErrorCode::OK;
    return true;
}

} // namespace AmelTech
