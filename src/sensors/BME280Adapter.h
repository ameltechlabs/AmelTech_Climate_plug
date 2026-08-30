/*
 * BME280Adapter.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * OPTIONAL DEPENDENCY: requires "Adafruit BME280 Library" (and its
 * dependencies: Adafruit Unified Sensor, Adafruit BusIO). I2C sensor;
 * populates temperature, humidity, and pressure. See examples/09_BME280.
 */

#ifndef AMELTECH_BME280_ADAPTER_H
#define AMELTECH_BME280_ADAPTER_H

#include <Arduino.h>
#include <Adafruit_BME280.h> // OPTIONAL - install separately
#include "Sensor.h"

namespace AmelTech {

class BME280Adapter : public Sensor {
public:
    // i2cAddress: typically 0x76 or 0x77 depending on board wiring.
    explicit BME280Adapter(uint8_t i2cAddress = 0x76);

    bool begin() override;
    bool read(ClimateData& out) override;

    SensorType getType() const override { return SensorType::SENSOR_BME280; }
    String getName() const override { return "BME280"; }
    SensorStatus getStatus() const override { return _status; }
    bool isEnabled() const override { return _enabled; }
    void setEnabled(bool enabled) override { _enabled = enabled; }

private:
    uint8_t _address;
    Adafruit_BME280 _bme;
    SensorStatus _status;
    bool _enabled;
    bool _began;
};

} // namespace AmelTech

#endif // AMELTECH_BME280_ADAPTER_H
