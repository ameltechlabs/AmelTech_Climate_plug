/*
 * SensorRegistry.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * Fixed-capacity registry of Sensor* pointers. Deliberately does NOT
 * own the sensors (no new/delete here) - the user's sketch
 * constructs concrete adapters (DHTAdapter, BME280Adapter, etc.) as
 * ordinary stack/global objects (since they need constructor
 * parameters like pins/I2C addresses the library can't know), and
 * registers pointers to them. This keeps the optional sensor
 * libraries fully opt-in: AmelTechClimate's core never constructs a
 * DHT or BME280 object itself.
 */

#ifndef AMELTECH_SENSOR_REGISTRY_H
#define AMELTECH_SENSOR_REGISTRY_H

#include <Arduino.h>
#include "Sensor.h"

namespace AmelTech {

class SensorRegistry {
public:
    static const uint8_t MAX_SENSORS = 6;

    SensorRegistry();

    // Returns the slot index (>= 0) or -1 if the registry is full.
    int8_t registerSensor(Sensor* sensor);
    bool unregisterSensor(int8_t index);

    Sensor* get(int8_t index) const;
    uint8_t count() const;

private:
    Sensor* _sensors[MAX_SENSORS];
    uint8_t _count;
};

} // namespace AmelTech

#endif // AMELTECH_SENSOR_REGISTRY_H
