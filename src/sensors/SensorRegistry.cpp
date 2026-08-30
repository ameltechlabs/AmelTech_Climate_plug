/*
 * SensorRegistry.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "SensorRegistry.h"

namespace AmelTech {

SensorRegistry::SensorRegistry() : _count(0) {
    for (uint8_t i = 0; i < MAX_SENSORS; i++) _sensors[i] = nullptr;
}

int8_t SensorRegistry::registerSensor(Sensor* sensor) {
    if (sensor == nullptr) return -1;

    for (uint8_t i = 0; i < MAX_SENSORS; i++) {
        if (_sensors[i] == nullptr) {
            _sensors[i] = sensor;
            _count++;
            return (int8_t)i;
        }
    }
    return -1; // full
}

bool SensorRegistry::unregisterSensor(int8_t index) {
    if (index < 0 || index >= (int8_t)MAX_SENSORS) return false;
    if (_sensors[index] == nullptr) return false;

    _sensors[index] = nullptr;
    _count--;
    return true;
}

Sensor* SensorRegistry::get(int8_t index) const {
    if (index < 0 || index >= (int8_t)MAX_SENSORS) return nullptr;
    return _sensors[index];
}

uint8_t SensorRegistry::count() const {
    return _count;
}

} // namespace AmelTech
