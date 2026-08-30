/*
 * BME680Adapter.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * OPTIONAL DEPENDENCY: requires "Adafruit BME680 Library" (and
 * Adafruit Unified Sensor, Adafruit BusIO). I2C sensor.
 *
 * NOTE ON GAS RESISTANCE: the BME680 also measures gas resistance
 * (used for air-quality/VOC estimation), but ClimateData has no
 * corresponding field - the spec's 20 tracked parameters are all
 * conventional weather/soil variables, and air quality was not
 * listed. Per spec rule "Do not invent unsupported parameters" (that
 * rule targets Open-Meteo variables specifically, but the same
 * discipline applies here), this adapter does NOT repurpose an
 * unrelated ClimateData field to smuggle gas resistance through.
 * getGasResistance() is exposed as a BME680Adapter-specific extra
 * for advanced users who want it directly, separate from the
 * ClimateData contract.
 */

#ifndef AMELTECH_BME680_ADAPTER_H
#define AMELTECH_BME680_ADAPTER_H

#include <Arduino.h>
#include <Adafruit_BME680.h> // OPTIONAL - install separately
#include "Sensor.h"

namespace AmelTech {

class BME680Adapter : public Sensor {
public:
    explicit BME680Adapter(uint8_t i2cAddress = 0x76);

    bool begin() override;
    bool read(ClimateData& out) override;

    SensorType getType() const override { return SensorType::SENSOR_BME680; }
    String getName() const override { return "BME680"; }
    SensorStatus getStatus() const override { return _status; }
    bool isEnabled() const override { return _enabled; }
    void setEnabled(bool enabled) override { _enabled = enabled; }

    // BME680-specific extra, not part of the Sensor interface or
    // ClimateData contract (see class header note).
    float getGasResistanceOhms() const { return _lastGasResistance; }

private:
    uint8_t _address;
    Adafruit_BME680 _bme;
    SensorStatus _status;
    bool _enabled;
    bool _began;
    float _lastGasResistance;
};

} // namespace AmelTech

#endif // AMELTECH_BME680_ADAPTER_H
