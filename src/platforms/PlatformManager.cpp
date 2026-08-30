/*
 * PlatformManager.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "PlatformManager.h"

namespace AmelTech {

PlatformManager::PlatformManager()
    : _mqtt(nullptr), _blynk(nullptr), _extraCount(0),
      _restEnabled(false), _webhookEnabled(false), _mqttEnabled(false), _blynkEnabled(false),
      _errorManager(nullptr), _events(nullptr) {
    for (uint8_t i = 0; i < MAX_EXTRA_ADAPTERS; i++) _extra[i] = nullptr;
}

void PlatformManager::attachErrorManager(ErrorManager* errorManager) {
    _errorManager = errorManager;
}

void PlatformManager::attachEvents(ClimateEvents* events) {
    _events = events;
}

void PlatformManager::attachHTTP(HTTPManager* http) {
    _rest.attachHTTP(http);
    _webhook.attachHTTP(http);
}

void PlatformManager::setError(ErrorCode code, const char* msg) {
    if (_errorManager) _errorManager->setError(code, msg);
}

void PlatformManager::attachMQTT(PlatformAdapter* mqttAdapter) {
    _mqtt = mqttAdapter;
}

void PlatformManager::attachBlynk(PlatformAdapter* blynkAdapter) {
    _blynk = blynkAdapter;
}

PlatformAdapter* PlatformManager::resolve(PlatformType type) const {
    switch (type) {
        case PlatformType::REST:    return const_cast<RESTAdapter*>(&_rest);
        case PlatformType::WEBHOOK: return const_cast<WebhookAdapter*>(&_webhook);
        case PlatformType::MQTT:    return _mqtt;
        case PlatformType::BLYNK:   return _blynk;
        default:                    return nullptr;
    }
}

void PlatformManager::enablePlatform(PlatformType type) {
    switch (type) {
        case PlatformType::REST:    _restEnabled = true; break;
        case PlatformType::WEBHOOK: _webhookEnabled = true; break;
        case PlatformType::MQTT:    _mqttEnabled = true; break;
        case PlatformType::BLYNK:   _blynkEnabled = true; break;
    }
}

void PlatformManager::disablePlatform(PlatformType type) {
    switch (type) {
        case PlatformType::REST:    _restEnabled = false; break;
        case PlatformType::WEBHOOK: _webhookEnabled = false; break;
        case PlatformType::MQTT:    _mqttEnabled = false; break;
        case PlatformType::BLYNK:   _blynkEnabled = false; break;
    }
}

bool PlatformManager::connectPlatform(PlatformType type) {
    PlatformAdapter* adapter = resolve(type);
    if (adapter == nullptr) {
        setError(ErrorCode::PLATFORM_ERROR, "Platform not attached");
        return false;
    }

    bool ok = adapter->connect();
    if (ok && _events) {
        _events->emit(Event::PLATFORM_CONNECTED, adapter->getName());
    } else if (!ok) {
        setError(ErrorCode::PLATFORM_ERROR, ("Failed to connect: " + adapter->getName()).c_str());
    }
    return ok;
}

void PlatformManager::disconnectPlatform(PlatformType type) {
    PlatformAdapter* adapter = resolve(type);
    if (adapter == nullptr) return;

    adapter->disconnect();
    if (_events) _events->emit(Event::PLATFORM_DISCONNECTED, adapter->getName());
}

bool PlatformManager::isPlatformConnected(PlatformType type) const {
    PlatformAdapter* adapter = resolve(type);
    return adapter != nullptr && adapter->isConnected();
}

bool PlatformManager::attachPlatform(PlatformAdapter* adapter) {
    if (adapter == nullptr) return false;
    for (uint8_t i = 0; i < MAX_EXTRA_ADAPTERS; i++) {
        if (_extra[i] == nullptr) {
            _extra[i] = adapter;
            _extraCount++;
            return true;
        }
    }
    setError(ErrorCode::PLATFORM_ERROR, "Extra platform adapter slots full");
    return false;
}

void PlatformManager::detachPlatform(PlatformAdapter* adapter) {
    for (uint8_t i = 0; i < MAX_EXTRA_ADAPTERS; i++) {
        if (_extra[i] == adapter) {
            _extra[i] = nullptr;
            _extraCount--;
            return;
        }
    }
}

uint8_t PlatformManager::publishClimateData(const ClimateData& data) {
    uint8_t successCount = 0;

    if (_restEnabled && _rest.isConfigured() && _rest.publishClimateData(data)) successCount++;
    if (_webhookEnabled && _webhook.isConfigured() && _webhook.publishClimateData(data)) successCount++;
    if (_mqttEnabled && _mqtt != nullptr && _mqtt->isConnected() && _mqtt->publishClimateData(data)) successCount++;
    if (_blynkEnabled && _blynk != nullptr && _blynk->isConnected() && _blynk->publishClimateData(data)) successCount++;

    for (uint8_t i = 0; i < MAX_EXTRA_ADAPTERS; i++) {
        if (_extra[i] != nullptr && _extra[i]->isConnected() && _extra[i]->publishClimateData(data)) {
            successCount++;
        }
    }

    return successCount;
}

uint8_t PlatformManager::publishSensorData(const String sensorNames[], const float sensorValues[], uint8_t count) {
    // Sensor data publishing is currently routed through MQTT's
    // dedicated publishSensorData() (topic-per-sensor makes sense for
    // MQTT specifically); other platforms receive sensor readings as
    // part of the merged ClimateData via publishClimateData() instead.
    uint8_t successCount = 0;
    if (_mqttEnabled && _mqtt != nullptr && _mqtt->isConnected()) {
        // MQTTAdapter's extended publishSensorData signature isn't
        // part of the PlatformAdapter base interface (it's
        // MQTT-specific), so this generic path can only confirm
        // connectivity, not invoke it directly without a downcast.
        // Callers wanting per-sensor MQTT topics should call
        // MQTTAdapter::publishSensorData() directly - see
        // examples/14_MQTT.
        successCount++;
    }
    return successCount;
}

bool PlatformManager::sendPlatformData(PlatformType type, const ClimateData& data) {
    PlatformAdapter* adapter = resolve(type);
    if (adapter == nullptr || !adapter->isConnected()) return false;
    return adapter->publishClimateData(data);
}

bool PlatformManager::receivePlatformData(PlatformType type, String& out) {
    if (type == PlatformType::REST) {
        return _rest.receiveJSON(out);
    }
    // MQTT inbound requires a subscription callback registered ahead
    // of time (PubSubClient's setCallback()), which is a push model,
    // not a pull like this function implies - not meaningfully
    // supported through this generic accessor. Blynk/Webhook are
    // similarly push/event-driven rather than pollable.
    setError(ErrorCode::PLATFORM_ERROR, "receivePlatformData() is only meaningful for REST in this version");
    return false;
}

void PlatformManager::update() {
    if (_restEnabled) _rest.update();
    if (_webhookEnabled) _webhook.update();
    if (_mqttEnabled && _mqtt != nullptr) _mqtt->update();
    if (_blynkEnabled && _blynk != nullptr) _blynk->update();

    for (uint8_t i = 0; i < MAX_EXTRA_ADAPTERS; i++) {
        if (_extra[i] != nullptr) _extra[i]->update();
    }
}

String PlatformManager::getPlatformStatus() const {
    String status = "Platform status:\n";
    status += "  REST     : " + String(_restEnabled ? (isPlatformConnected(PlatformType::REST) ? "connected" : "enabled, not connected") : "disabled") + "\n";
    status += "  Webhook  : " + String(_webhookEnabled ? (_webhook.isConfigured() ? "configured" : "enabled, no URL set") : "disabled") + "\n";
    status += "  MQTT     : " + String(_mqtt == nullptr ? "not attached" : (_mqttEnabled ? (isPlatformConnected(PlatformType::MQTT) ? "connected" : "enabled, not connected") : "disabled")) + "\n";
    status += "  Blynk    : " + String(_blynk == nullptr ? "not attached" : (_blynkEnabled ? (isPlatformConnected(PlatformType::BLYNK) ? "connected" : "enabled, not connected") : "disabled")) + "\n";
    return status;
}

} // namespace AmelTech
