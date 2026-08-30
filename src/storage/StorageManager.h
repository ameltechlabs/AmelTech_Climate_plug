/*
 * StorageManager.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * Low-level, generic key/value persistence wrapper around ESP32's
 * Preferences (NVS) API. ConfigurationManager builds the actual
 * WiFi/location/URL/sensor/platform persistence on top of this - this
 * class itself doesn't know what a "location" is, just namespaces
 * and typed key/value pairs.
 */

#ifndef AMELTECH_STORAGE_MANAGER_H
#define AMELTECH_STORAGE_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>
#include "../diagnostics/ErrorManager.h"

namespace AmelTech {

class StorageManager {
public:
    StorageManager();

    void attachErrorManager(ErrorManager* errorManager);

    // Opens the given NVS namespace (max 15 chars, an ESP32 NVS
    // constraint). Must be called before any save/load/remove call
    // that targets this namespace.
    bool begin(const char* nvsNamespace, bool readOnly = false);
    void end();

    bool save(const char* key, const String& value);
    bool save(const char* key, float value);
    bool save(const char* key, int value);
    bool save(const char* key, bool value);
    bool save(const char* key, uint32_t value);
    bool save(const char* key, unsigned long value);

    String loadString(const char* key, const String& defaultValue = "") const;
    float loadFloat(const char* key, float defaultValue = 0.0f) const;
    int loadInt(const char* key, int defaultValue = 0) const;
    bool loadBool(const char* key, bool defaultValue = false) const;
    uint32_t loadUInt(const char* key, uint32_t defaultValue = 0) const;
    unsigned long loadULong(const char* key, unsigned long defaultValue = 0) const;

    bool exists(const char* key) const;
    bool remove(const char* key);
    void clear(); // wipes the entire currently-open namespace

    bool isOpen() const;

private:
    mutable Preferences _prefs; // mutable: Preferences::getX() is non-const upstream despite being logically read-only
    bool _open;
    bool _readOnly;
    ErrorManager* _errorManager;

    void setError(ErrorCode code, const char* msg);
};

} // namespace AmelTech

#endif // AMELTECH_STORAGE_MANAGER_H
