/*
 * ErrorManager.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "ErrorManager.h"

namespace AmelTech {

const char* errorCodeToString(ErrorCode code) {
    switch (code) {
        case ErrorCode::OK:                  return "OK";
        case ErrorCode::NOT_INITIALIZED:     return "NOT_INITIALIZED";
        case ErrorCode::WIFI_ERROR:          return "WIFI_ERROR";
        case ErrorCode::WIFI_TIMEOUT:        return "WIFI_TIMEOUT";
        case ErrorCode::WIFI_DISCONNECTED:   return "WIFI_DISCONNECTED";
        case ErrorCode::URL_INVALID:         return "URL_INVALID";
        case ErrorCode::URL_ERROR:           return "URL_ERROR";
        case ErrorCode::HTTP_ERROR:          return "HTTP_ERROR";
        case ErrorCode::HTTP_TIMEOUT:        return "HTTP_TIMEOUT";
        case ErrorCode::HTTP_STATUS_ERROR:   return "HTTP_STATUS_ERROR";
        case ErrorCode::TLS_ERROR:           return "TLS_ERROR";
        case ErrorCode::JSON_ERROR:          return "JSON_ERROR";
        case ErrorCode::RESPONSE_ERROR:      return "RESPONSE_ERROR";
        case ErrorCode::DATA_INVALID:        return "DATA_INVALID";
        case ErrorCode::DATA_UNAVAILABLE:    return "DATA_UNAVAILABLE";
        case ErrorCode::DATA_STALE:          return "DATA_STALE";
        case ErrorCode::LOCATION_INVALID:    return "LOCATION_INVALID";
        case ErrorCode::SENSOR_ERROR:        return "SENSOR_ERROR";
        case ErrorCode::SENSOR_NOT_FOUND:    return "SENSOR_NOT_FOUND";
        case ErrorCode::CACHE_ERROR:         return "CACHE_ERROR";
        case ErrorCode::STORAGE_ERROR:       return "STORAGE_ERROR";
        case ErrorCode::PROVIDER_ERROR:      return "PROVIDER_ERROR";
        case ErrorCode::PLATFORM_ERROR:      return "PLATFORM_ERROR";
        case ErrorCode::MEMORY_ERROR:        return "MEMORY_ERROR";
        case ErrorCode::CONFIG_ERROR:        return "CONFIG_ERROR";
        default:                             return "UNKNOWN_ERROR";
    }
}

ErrorManager::ErrorManager()
    : _lastCode(ErrorCode::OK), _lastMessage(""), _lastTimestamp(0) {
}

void ErrorManager::setError(ErrorCode code, const char* message) {
    _lastCode = code;
    _lastMessage = (message != nullptr) ? String(message) : String(errorCodeToString(code));
    _lastTimestamp = millis();
}

void ErrorManager::clearError() {
    _lastCode = ErrorCode::OK;
    _lastMessage = "";
    _lastTimestamp = millis();
}

bool ErrorManager::hasError() const {
    return _lastCode != ErrorCode::OK;
}

ErrorCode ErrorManager::getErrorCode() const {
    return _lastCode;
}

String ErrorManager::getErrorMessage() const {
    return _lastMessage;
}

unsigned long ErrorManager::getErrorTimestamp() const {
    return _lastTimestamp;
}

} // namespace AmelTech
