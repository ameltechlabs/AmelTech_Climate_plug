/*
 * ErrorManager.h
 * AmelTech_Climate_plug
 *
 * Central error code definitions and error-tracking manager.
 * PHASE 1 - IMPLEMENTED
 *
 * All modules in this library report errors through this shared
 * enum + ErrorManager, so behavior is consistent everywhere.
 */

#ifndef AMELTECH_ERROR_MANAGER_H
#define AMELTECH_ERROR_MANAGER_H

#include <Arduino.h>

namespace AmelTech {

enum class ErrorCode {
    OK = 0,

    NOT_INITIALIZED,

    WIFI_ERROR,
    WIFI_TIMEOUT,
    WIFI_DISCONNECTED,

    URL_INVALID,
    URL_ERROR,

    HTTP_ERROR,
    HTTP_TIMEOUT,
    HTTP_STATUS_ERROR,

    TLS_ERROR,

    JSON_ERROR,
    RESPONSE_ERROR,

    DATA_INVALID,
    DATA_UNAVAILABLE,
    DATA_STALE,

    LOCATION_INVALID,

    SENSOR_ERROR,
    SENSOR_NOT_FOUND,

    CACHE_ERROR,
    STORAGE_ERROR,

    PROVIDER_ERROR,
    PLATFORM_ERROR,

    MEMORY_ERROR,
    CONFIG_ERROR
};

// Human-readable string for every ErrorCode. Implemented in .cpp.
const char* errorCodeToString(ErrorCode code);

/*
 * ErrorManager
 *
 * Tracks the single "last error" for the library, plus an optional
 * short rolling history for diagnostics. Not a logger (see Logger.h) -
 * this is specifically for "what went wrong most recently and why".
 */
class ErrorManager {
public:
    ErrorManager();

    // Record a new error. message may be nullptr for default text.
    void setError(ErrorCode code, const char* message = nullptr);

    // Clear the current error back to OK.
    void clearError();

    bool hasError() const;
    ErrorCode getErrorCode() const;
    String getErrorMessage() const;

    // Timestamp (millis()) of when the last error was set.
    unsigned long getErrorTimestamp() const;

private:
    ErrorCode _lastCode;
    String _lastMessage;
    unsigned long _lastTimestamp;
};

} // namespace AmelTech

#endif // AMELTECH_ERROR_MANAGER_H
