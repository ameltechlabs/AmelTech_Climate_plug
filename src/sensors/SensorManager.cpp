/*
 * SensorManager.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "SensorManager.h"

namespace AmelTech {

SensorManager::SensorManager() : _errorManager(nullptr), _events(nullptr) {
}

void SensorManager::attachErrorManager(ErrorManager* errorManager) {
    _errorManager = errorManager;
}

void SensorManager::attachEvents(ClimateEvents* events) {
    _events = events;
}

void SensorManager::setError(ErrorCode code, const char* msg) {
    if (_errorManager) _errorManager->setError(code, msg);
}

int8_t SensorManager::registerSensor(Sensor* sensor) {
    int8_t idx = _registry.registerSensor(sensor);
    if (idx < 0) {
        setError(ErrorCode::SENSOR_ERROR, "Sensor registry full");
    }
    return idx;
}

bool SensorManager::unregisterSensor(int8_t index) {
    return _registry.unregisterSensor(index);
}

void SensorManager::enableSensor(int8_t index) {
    Sensor* s = _registry.get(index);
    if (s) s->setEnabled(true);
}

void SensorManager::disableSensor(int8_t index) {
    Sensor* s = _registry.get(index);
    if (s) s->setEnabled(false);
}

uint8_t SensorManager::scanSensors() {
    uint8_t successCount = 0;
    for (uint8_t i = 0; i < SensorRegistry::MAX_SENSORS; i++) {
        Sensor* s = _registry.get((int8_t)i);
        if (s == nullptr) continue;

        bool ok = s->begin();
        if (ok) {
            successCount++;
            if (_events) _events->emit(Event::SENSOR_CONNECTED, s->getName());
        } else {
            if (_events) _events->emit(Event::SENSOR_ERROR, s->getName());
        }
    }
    return successCount;
}

uint8_t SensorManager::getSensorCount() const {
    return _registry.count();
}

String SensorManager::getSensorName(int8_t index) const {
    Sensor* s = _registry.get(index);
    return s ? s->getName() : "N/A";
}

SensorType SensorManager::getSensorType(int8_t index) const {
    Sensor* s = _registry.get(index);
    return s ? s->getType() : SensorType::SENSOR_DHT11; // caller should check getSensorName()!="N/A" first
}

SensorStatus SensorManager::getSensorStatus(int8_t index) const {
    Sensor* s = _registry.get(index);
    return s ? s->getStatus() : SensorStatus::NOT_FOUND;
}

bool SensorManager::readSensor(int8_t index, ClimateData& out) {
    Sensor* s = _registry.get(index);
    if (s == nullptr) {
        setError(ErrorCode::SENSOR_NOT_FOUND, "No sensor at that index");
        return false;
    }
    if (!s->isEnabled()) {
        setError(ErrorCode::SENSOR_ERROR, "Sensor is disabled");
        return false;
    }

    bool ok = s->read(out);
    if (!ok) {
        setError(ErrorCode::SENSOR_ERROR, ("Read failed: " + s->getName()).c_str());
        if (_events) _events->emit(Event::SENSOR_ERROR, s->getName());
    }
    return ok;
}

uint8_t SensorManager::readAllSensors(ClimateData& out) {
    uint8_t successCount = 0;
    out.reset();

    for (uint8_t i = 0; i < SensorRegistry::MAX_SENSORS; i++) {
        Sensor* s = _registry.get((int8_t)i);
        if (s == nullptr || !s->isEnabled()) continue;

        ClimateData reading;
        if (s->read(reading)) {
            out.mergeFrom(reading);
            successCount++;
        } else {
            if (_events) _events->emit(Event::SENSOR_ERROR, s->getName());
        }
    }

    if (successCount > 0) {
        out.receivedAt = millis();
        out.source = DataSource::LOCAL;
    }

    return successCount;
}

} // namespace AmelTech
