/*
 * DHTAdapter.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * OPTIONAL DEPENDENCY: requires Adafruit's "DHT sensor library"
 * (and its dependency "Adafruit Unified Sensor"). This header is
 * only compiled in if the user's sketch actually includes it - the
 * core library (AmelTechClimate) never includes DHTAdapter.h itself,
 * per spec "Do not force optional dependencies into the core."
 * See examples/08_DHT for usage.
 *
 * DHT11: temperature + humidity only (spec: 20 fields tracked by
 * ClimateData; DHT11/22 populate exactly 2 of them).
 * DHT22: same 2 fields, better accuracy/range than DHT11.
 */

#ifndef AMELTECH_DHT_ADAPTER_H
#define AMELTECH_DHT_ADAPTER_H

#include <Arduino.h>
#include <DHT.h> // Adafruit DHT sensor library - OPTIONAL, install separately
#include "Sensor.h"

namespace AmelTech {

class DHTAdapter : public Sensor {
public:
    // pin: GPIO pin the sensor's data line is connected to.
    // isDHT22: true for DHT22/AM2302, false for DHT11.
    DHTAdapter(uint8_t pin, bool isDHT22 = false);

    bool begin() override;
    bool read(ClimateData& out) override;

    SensorType getType() const override { return _isDHT22 ? SensorType::SENSOR_DHT22 : SensorType::SENSOR_DHT11; }
    String getName() const override { return _isDHT22 ? "DHT22" : "DHT11"; }
    SensorStatus getStatus() const override { return _status; }
    bool isEnabled() const override { return _enabled; }
    void setEnabled(bool enabled) override { _enabled = enabled; }

private:
    uint8_t _pin;
    bool _isDHT22;
    DHT _dht;
    SensorStatus _status;
    bool _enabled;
    bool _began;
};

} // namespace AmelTech

#endif // AMELTECH_DHT_ADAPTER_H
