/*
 * BME280Adapter.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "BME280Adapter.h"

namespace AmelTech {

BME280Adapter::BME280Adapter(uint8_t i2cAddress)
    : _address(i2cAddress), _status(SensorStatus::NOT_INITIALIZED), _enabled(true), _began(false) {
}

bool BME280Adapter::begin() {
    _began = _bme.begin(_address);
    _status = _began ? SensorStatus::OK : SensorStatus::NOT_FOUND;
    return _began;
}

bool BME280Adapter::read(ClimateData& out) {
    if (!_began) {
        _status = SensorStatus::NOT_INITIALIZED;
        return false;
    }
    if (!_enabled) return false;

    float temp = _bme.readTemperature();       // Celsius
    float hum = _bme.readHumidity();            // %
    float pressureHPa = _bme.readPressure() / 100.0f; // Pa -> hPa

    if (isnan(temp) || isnan(hum) || isnan(pressureHPa)) {
        _status = SensorStatus::ERROR;
        return false;
    }

    out.temperature = temp;
    out.hasTemperature = true;
    out.relativeHumidity = hum;
    out.hasRelativeHumidity = true;
    out.atmosphericPressure = pressureHPa;
    out.hasAtmosphericPressure = true;
    out.receivedAt = millis();
    out.source = DataSource::LOCAL;

    _status = SensorStatus::OK;
    return true;
}

} // namespace AmelTech
