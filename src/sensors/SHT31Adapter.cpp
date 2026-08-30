/*
 * SHT31Adapter.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "SHT31Adapter.h"

namespace AmelTech {

SHT31Adapter::SHT31Adapter(uint8_t i2cAddress)
    : _address(i2cAddress), _status(SensorStatus::NOT_INITIALIZED), _enabled(true), _began(false) {
}

bool SHT31Adapter::begin() {
    _began = _sht.begin(_address);
    _status = _began ? SensorStatus::OK : SensorStatus::NOT_FOUND;
    return _began;
}

bool SHT31Adapter::read(ClimateData& out) {
    if (!_began) {
        _status = SensorStatus::NOT_INITIALIZED;
        return false;
    }
    if (!_enabled) return false;

    float temp = _sht.readTemperature(); // Celsius
    float hum = _sht.readHumidity();      // %

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
