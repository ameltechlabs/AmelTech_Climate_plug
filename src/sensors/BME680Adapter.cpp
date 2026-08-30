/*
 * BME680Adapter.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "BME680Adapter.h"

namespace AmelTech {

BME680Adapter::BME680Adapter(uint8_t i2cAddress)
    : _address(i2cAddress), _status(SensorStatus::NOT_INITIALIZED), _enabled(true), _began(false),
      _lastGasResistance(0.0f) {
}

bool BME680Adapter::begin() {
    _began = _bme.begin(_address);
    if (_began) {
        // Standard oversampling/filter configuration recommended by
        // Adafruit's example sketches for stable readings.
        _bme.setTemperatureOversampling(BME680_OS_8X);
        _bme.setHumidityOversampling(BME680_OS_2X);
        _bme.setPressureOversampling(BME680_OS_4X);
        _bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
        _bme.setGasHeater(320, 150); // 320*C for 150 ms
    }
    _status = _began ? SensorStatus::OK : SensorStatus::NOT_FOUND;
    return _began;
}

bool BME680Adapter::read(ClimateData& out) {
    if (!_began) {
        _status = SensorStatus::NOT_INITIALIZED;
        return false;
    }
    if (!_enabled) return false;

    if (!_bme.performReading()) {
        _status = SensorStatus::ERROR;
        return false;
    }

    out.temperature = _bme.temperature; // Celsius
    out.hasTemperature = true;
    out.relativeHumidity = _bme.humidity; // %
    out.hasRelativeHumidity = true;
    out.atmosphericPressure = _bme.pressure / 100.0f; // Pa -> hPa
    out.hasAtmosphericPressure = true;
    out.receivedAt = millis();
    out.source = DataSource::LOCAL;

    _lastGasResistance = _bme.gas_resistance; // ohms - not mapped into ClimateData, see header note

    _status = SensorStatus::OK;
    return true;
}

} // namespace AmelTech
