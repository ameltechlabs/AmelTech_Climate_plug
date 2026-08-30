/*
 * Sensor.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * Abstract base every local sensor adapter (DHT/BME280/BME680/SHT31)
 * implements, so SensorManager can drive them uniformly and hybrid
 * mode can merge their readings into ClimateData without caring which
 * concrete sensor produced them.
 */

#ifndef AMELTECH_SENSOR_H
#define AMELTECH_SENSOR_H

#include <Arduino.h>
#include "../data/ClimateData.h"
#include "../diagnostics/ErrorManager.h"

namespace AmelTech {

enum class SensorType {
    SENSOR_DHT11,
    SENSOR_DHT22,
    SENSOR_BME280,
    SENSOR_BME680,
    SENSOR_SHT31
};

enum class SensorStatus {
    NOT_INITIALIZED,
    OK,
    ERROR,
    NOT_FOUND
};

const char* sensorTypeToString(SensorType type);
const char* sensorStatusToString(SensorStatus status);

class Sensor {
public:
    virtual ~Sensor() {}

    // Initializes the physical sensor (I2C/OneWire bus setup, etc).
    // Returns true if the sensor responded/was detected.
    virtual bool begin() = 0;

    // Takes a fresh reading and populates the relevant fields on
    // `out` (only the fields this sensor type actually measures -
    // e.g. DHT11 sets hasTemperature/hasRelativeHumidity but not
    // hasAtmosphericPressure). Returns false on read failure.
    virtual bool read(ClimateData& out) = 0;

    virtual SensorType getType() const = 0;
    virtual String getName() const = 0;
    virtual SensorStatus getStatus() const = 0;
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
};

} // namespace AmelTech

#endif // AMELTECH_SENSOR_H
