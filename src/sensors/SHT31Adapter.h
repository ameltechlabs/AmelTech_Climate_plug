/*
 * SHT31Adapter.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * OPTIONAL DEPENDENCY: requires "Adafruit SHT31 Library" (and
 * Adafruit Unified Sensor / Adafruit BusIO). I2C sensor; populates
 * temperature and humidity with high accuracy.
 */

#ifndef AMELTECH_SHT31_ADAPTER_H
#define AMELTECH_SHT31_ADAPTER_H

#include <Arduino.h>
#include <Adafruit_SHT31.h> // OPTIONAL - install separately
#include "Sensor.h"

namespace AmelTech {

class SHT31Adapter : public Sensor {
public:
    explicit SHT31Adapter(uint8_t i2cAddress = 0x44);

    bool begin() override;
    bool read(ClimateData& out) override;

    SensorType getType() const override { return SensorType::SENSOR_SHT31; }
    String getName() const override { return "SHT31"; }
    SensorStatus getStatus() const override { return _status; }
    bool isEnabled() const override { return _enabled; }
    void setEnabled(bool enabled) override { _enabled = enabled; }

private:
    uint8_t _address;
    Adafruit_SHT31 _sht;
    SensorStatus _status;
    bool _enabled;
    bool _began;
};

} // namespace AmelTech

#endif // AMELTECH_SHT31_ADAPTER_H
