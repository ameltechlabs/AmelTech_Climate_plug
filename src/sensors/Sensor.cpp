/*
 * Sensor.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "Sensor.h"

namespace AmelTech {

const char* sensorTypeToString(SensorType type) {
    switch (type) {
        case SensorType::SENSOR_DHT11:  return "DHT11";
        case SensorType::SENSOR_DHT22:  return "DHT22";
        case SensorType::SENSOR_BME280: return "BME280";
        case SensorType::SENSOR_BME680: return "BME680";
        case SensorType::SENSOR_SHT31:  return "SHT31";
        default:                 return "UNKNOWN";
    }
}

const char* sensorStatusToString(SensorStatus status) {
    switch (status) {
        case SensorStatus::NOT_INITIALIZED: return "NOT_INITIALIZED";
        case SensorStatus::OK:              return "OK";
        case SensorStatus::ERROR:           return "ERROR";
        case SensorStatus::NOT_FOUND:       return "NOT_FOUND";
        default:                            return "UNKNOWN";
    }
}

} // namespace AmelTech
