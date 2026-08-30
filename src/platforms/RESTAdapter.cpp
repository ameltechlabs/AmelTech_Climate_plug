/*
 * RESTAdapter.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "RESTAdapter.h"
#include <ArduinoJson.h>

namespace AmelTech {

RESTAdapter::RESTAdapter()
    : _http(nullptr), _endpoint(""), _method(URLMethod::POST),
      _contentType("application/json"), _headersJson(""), _lastConnectOk(false) {
}

void RESTAdapter::attachHTTP(HTTPManager* http) {
    _http = http;
}

void RESTAdapter::begin() {
    // Nothing stateful to initialize for a REST client; present for
    // API symmetry with the other platform adapters' begin().
}

void RESTAdapter::setEndpoint(const String& url) {
    _endpoint = url;
}

void RESTAdapter::setMethod(URLMethod method) {
    _method = method;
}

void RESTAdapter::setHeaders(const String& headersJson) {
    _headersJson = headersJson;
}

void RESTAdapter::setContentType(const String& contentType) {
    _contentType = contentType;
}

bool RESTAdapter::GET(String& responseOut) {
    if (_http == nullptr || _endpoint.length() == 0) return false;
    HTTPResponse resp = _http->GET(_endpoint);
    responseOut = resp.body;
    return resp.success;
}

bool RESTAdapter::POST(const String& body, String& responseOut) {
    if (_http == nullptr || _endpoint.length() == 0) return false;
    HTTPResponse resp = _http->POST(_endpoint, body, _contentType);
    responseOut = resp.body;
    return resp.success;
}

bool RESTAdapter::PUT(const String& body, String& responseOut) {
    if (_http == nullptr || _endpoint.length() == 0) return false;
    HTTPResponse resp = _http->PUT(_endpoint, body, _contentType);
    responseOut = resp.body;
    return resp.success;
}

bool RESTAdapter::PATCH(const String& body, String& responseOut) {
    if (_http == nullptr || _endpoint.length() == 0) return false;
    HTTPResponse resp = _http->PATCH(_endpoint, body, _contentType);
    responseOut = resp.body;
    return resp.success;
}

bool RESTAdapter::sendJSON(const String& json) {
    String discard;
    switch (_method) {
        case URLMethod::PUT:   return PUT(json, discard);
        case URLMethod::PATCH: return PATCH(json, discard);
        case URLMethod::GET:   return GET(discard); // GET ignores body per HTTP semantics
        case URLMethod::POST:
        default:                return POST(json, discard);
    }
}

bool RESTAdapter::receiveJSON(String& out) {
    return GET(out);
}

bool RESTAdapter::testConnection() {
    if (_http == nullptr || _endpoint.length() == 0) {
        _lastConnectOk = false;
        return false;
    }
    HTTPResponse resp = _http->GET(_endpoint);
    // Any HTTP response (even a 4xx/5xx status) proves the endpoint
    // is reachable; only a transport-level failure means "not
    // connected". resp.success is only true for 2xx, so we check the
    // status code directly rather than resp.success here.
    _lastConnectOk = resp.statusCode > 0;
    return _lastConnectOk;
}

bool RESTAdapter::connect() {
    return testConnection();
}

void RESTAdapter::disconnect() {
    _lastConnectOk = false;
}

bool RESTAdapter::isConnected() const {
    return _lastConnectOk;
}

void RESTAdapter::update() {
    // REST is stateless per-request; nothing to maintain per loop().
}

String RESTAdapter::climateDataToJSON(const ClimateData& data) const {
    DynamicJsonDocument doc(2048);
    if (data.hasTemperature) doc["temperature"] = data.temperature;
    if (data.hasRelativeHumidity) doc["relative_humidity"] = data.relativeHumidity;
    if (data.hasAtmosphericPressure) doc["pressure"] = data.atmosphericPressure;
    if (data.hasWindSpeed) doc["wind_speed"] = data.windSpeed;
    if (data.hasUvIndex) doc["uv_index"] = data.uvIndex;
    if (data.hasWeatherCondition) doc["condition"] = data.weatherCondition;
    doc["source"] = dataSourceToString(data.source);
    doc["valid"] = data.valid;

    String out;
    serializeJson(doc, out);
    return out;
}

bool RESTAdapter::publishClimateData(const ClimateData& data) {
    return sendJSON(climateDataToJSON(data));
}

} // namespace AmelTech
