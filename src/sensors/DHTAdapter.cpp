/*
 * DHTAdapter.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "DHTAdapter.h"

namespace AmelTech {

DHTAdapter::DHTAdapter(uint8_t pin, bool isDHT22)
    : _pin(pin), _isDHT22(isDHT22),
      _dht(pin, isDHT22 ? DHT22 : DHT11),
      _status(SensorStatus::NOT_INITIALIZED), _enabled(true), _began(false) {
}

bool DHTAdapter::begin() {
    _dht.begin();
    _began = true;

    // DHT sensors need ~1-2s after power-up before their first valid
    // reading; we don't block here (Rule 9) - instead, the first
    // read() call after begin() may return a transient failure, which
    // is reported as SensorStatus::ERROR rather than crashing, and
    // the caller (SensorManager) is expected to retry on its own
    // schedule rather than begin() spinning until ready.
    _status = SensorStatus::OK;
    return true;
}

bool DHTAdapter::read(ClimateData& out) {
    if (!_began) {
        _status = SensorStatus::NOT_INITIALIZED;
        return false;
    }
    if (!_enabled) {
        return false;
    }

    float temp = _dht.readTemperature();  // Celsius
    float hum = _dht.readHumidity();

    if (isnan(temp) || isnan(hum)) {
        _status = SensorStatus::ERROR;
        return false;
    }

    out.temperature = temp;
    out.hasTemperature = true;
    out.relativeHumidity = hum;
    out.hasRelativeHumidity = true;
    out.receivedAt = millis();
    out.source = DataSource::LOCAL;

    _status = SensorStatus::OK;
    return true;
}

} // namespace AmelTech
