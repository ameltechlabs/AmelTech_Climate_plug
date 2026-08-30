/*
 * PersistentCache.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * Optional flash-backed mirror of MemoryCache using ESP32's
 * Preferences (NVS) API, so the last-known climate data can survive
 * a reboot/power cycle rather than starting from UNAVAILABLE every
 * boot. This is intentionally a SEPARATE, narrower concern from the
 * full configuration persistence spec's storage/StorageManager will
 * own (WiFi creds, locations, URLs, sensor config, etc.) - this
 * class persists ONLY the single latest ClimateData snapshot plus
 * its timestamp, under its own NVS namespace, so it can ship now
 * without waiting on the rest of storage/.
 */

#ifndef AMELTECH_PERSISTENT_CACHE_H
#define AMELTECH_PERSISTENT_CACHE_H

#include <Arduino.h>
#include <Preferences.h>
#include "../data/ClimateData.h"
#include "../diagnostics/ErrorManager.h"

namespace AmelTech {

class PersistentCache {
public:
    PersistentCache();

    void attachErrorManager(ErrorManager* errorManager);

    bool begin(); // opens the NVS namespace; call once during library begin()

    bool save(const ClimateData& data);
    bool load(ClimateData& out); // returns true if a valid snapshot was found

    bool isAvailable() const; // true if NVS has a stored snapshot (checked at begin())
    void clear();

private:
    static const char* NVS_NAMESPACE;

    Preferences _prefs;
    bool _began;
    bool _hasStoredData;
    ErrorManager* _errorManager;

    void setError(ErrorCode code, const char* msg);
};

} // namespace AmelTech

#endif // AMELTECH_PERSISTENT_CACHE_H
