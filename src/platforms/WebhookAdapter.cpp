/*
 * WebhookAdapter.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "WebhookAdapter.h"
#include <ArduinoJson.h>

namespace AmelTech {

WebhookAdapter::WebhookAdapter()
    : _http(nullptr), _url(""), _method(URLMethod::POST), _lastTriggerOk(false) {
}

void WebhookAdapter::attachHTTP(HTTPManager* http) {
    _http = http;
}

void WebhookAdapter::setURL(const String& url) {
    _url = url;
}

void WebhookAdapter::setMethod(URLMethod method) {
    _method = method;
}

bool WebhookAdapter::trigger() {
    if (_http == nullptr || _url.length() == 0) {
        _lastTriggerOk = false;
        return false;
    }

    HTTPResponse resp = (_method == URLMethod::GET)
                             ? _http->GET(_url)
                             : _http->POST(_url, "{}", "application/json");

    _lastTriggerOk = resp.success;
    return resp.success;
}

bool WebhookAdapter::sendEvent(const String& eventName, const String& payloadJson) {
    if (_http == nullptr || _url.length() == 0) {
        _lastTriggerOk = false;
        return false;
    }

    DynamicJsonDocument doc(1024);
    doc["event"] = eventName;

    // payloadJson is embedded as a raw nested object if it parses as
    // valid JSON; otherwise stored as a plain string field, so a
    // caller passing non-JSON text doesn't produce a corrupt envelope.
    DynamicJsonDocument payloadDoc(1024);
    if (deserializeJson(payloadDoc, payloadJson) == DeserializationError::Ok) {
        doc["payload"] = payloadDoc.as<JsonVariant>();
    } else {
        doc["payload"] = payloadJson;
    }

    String body;
    serializeJson(doc, body);

    HTTPResponse resp = _http->POST(_url, body, "application/json");
    _lastTriggerOk = resp.success;
    return resp.success;
}

String WebhookAdapter::climateDataToJSON(const ClimateData& data) const {
    DynamicJsonDocument doc(1024);
    if (data.hasTemperature) doc["temperature"] = data.temperature;
    if (data.hasRelativeHumidity) doc["relative_humidity"] = data.relativeHumidity;
    if (data.hasWeatherCondition) doc["condition"] = data.weatherCondition;
    doc["valid"] = data.valid;

    String out;
    serializeJson(doc, out);
    return out;
}

bool WebhookAdapter::sendClimateEvent(const ClimateData& data) {
    return sendEvent("climate_update", climateDataToJSON(data));
}

bool WebhookAdapter::test() {
    return trigger();
}

bool WebhookAdapter::connect() {
    return true; // webhooks are stateless HTTP calls; nothing to persistently "connect"
}

void WebhookAdapter::disconnect() {
    _lastTriggerOk = false;
}

bool WebhookAdapter::isConnected() const {
    return _lastTriggerOk;
}

void WebhookAdapter::update() {
    // No per-loop maintenance needed for fire-and-forget HTTP calls.
}

} // namespace AmelTech
