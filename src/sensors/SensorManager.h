/*
 * SensorManager.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * Drives SensorRegistry's registered Sensor* instances: scanning
 * (calling begin() on each), reading (calling read() on each and
 * merging results into one ClimateData via mergeFrom()), and
 * per-sensor status/name lookups for the `sensor status` command.
 */

#ifndef AMELTECH_SENSOR_MANAGER_H
#define AMELTECH_SENSOR_MANAGER_H

#include <Arduino.h>
#include "SensorRegistry.h"
#include "../data/ClimateData.h"
#include "../diagnostics/ErrorManager.h"
#include "../core/ClimateEvents.h"

namespace AmelTech {

class SensorManager {
public:
    SensorManager();

    void attachErrorManager(ErrorManager* errorManager);
    void attachEvents(ClimateEvents* events);

    int8_t registerSensor(Sensor* sensor);
    bool unregisterSensor(int8_t index);

    void enableSensor(int8_t index);
    void disableSensor(int8_t index);

    // Calls begin() on every registered sensor. Returns count of
    // sensors that responded successfully.
    uint8_t scanSensors();
    uint8_t detectSensors() { return scanSensors(); } // spec lists both names for the same operation

    uint8_t getSensorCount() const;
    String getSensorName(int8_t index) const;
    SensorType getSensorType(int8_t index) const;
    SensorStatus getSensorStatus(int8_t index) const;

    // Reads one sensor by index into `out`.
    bool readSensor(int8_t index, ClimateData& out);

    // Reads every enabled sensor and merges all results into one
    // ClimateData (later sensors' fields override earlier ones only
    // where the earlier sensor didn't already set that field - see
    // ClimateData::mergeFrom). Returns count of sensors successfully
    // read.
    uint8_t readAllSensors(ClimateData& out);

private:
    SensorRegistry _registry;
    ErrorManager* _errorManager;
    ClimateEvents* _events;

    void setError(ErrorCode code, const char* msg);
};

} // namespace AmelTech

#endif // AMELTECH_SENSOR_MANAGER_H
