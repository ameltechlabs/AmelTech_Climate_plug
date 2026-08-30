/*
 * CacheManager.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * Top-level cache facade matching the spec's CACHE SYSTEM function
 * list exactly. Combines MemoryCache (always active when caching is
 * enabled) with an optional PersistentCache (flash-backed, so the
 * last snapshot survives reboot). "Never present stale data as live"
 * (spec rule) is enforced here: getCachedData() always returns data
 * paired with its real CacheDataState, and callers (SerialDisplay,
 * AmelTechClimate getters) must check that state rather than
 * assuming freshness.
 */

#ifndef AMELTECH_CACHE_MANAGER_H
#define AMELTECH_CACHE_MANAGER_H

#include <Arduino.h>
#include "MemoryCache.h"
#include "PersistentCache.h"
#include "../diagnostics/ErrorManager.h"
#include "../core/ClimateEvents.h"

namespace AmelTech {

class CacheManager {
public:
    CacheManager();

    void attachErrorManager(ErrorManager* errorManager);
    void attachEvents(ClimateEvents* events);

    // Opens the persistent cache's NVS namespace. Call once during
    // library begin(), regardless of whether persistence ends up
    // being used, so isCacheAvailable() can reflect any snapshot
    // from a previous boot immediately.
    bool begin();

    void enableCache();
    void disableCache();
    bool isCacheEnabled() const;

    void setCacheLifetime(unsigned long ms);
    unsigned long getCacheLifetime() const;

    // Stores `data` into the in-memory cache (always) and, if
    // persistence is enabled, also into flash. Emits CACHE_UPDATED.
    void saveCache(const ClimateData& data);

    // Loads from memory if valid; else falls back to the persistent
    // snapshot (if available) - matching spec's "loadCache" intent
    // of recovering the best available data after e.g. a WiFi outage
    // or reboot.
    bool loadCache(ClimateData& out);

    void clearCache();

    bool isCacheAvailable() const;
    bool isCacheValid() const;
    bool isCacheExpired() const;

    const ClimateData& getCachedData() const;
    unsigned long getCacheAge() const;

    CacheDataState getState() const;

    // "useCache" / "forceRefresh" are policy flags read by
    // ClimateController to decide whether a scheduled update may be
    // served from cache instead of making a network request, versus
    // always forcing a fresh fetch.
    void useCache();
    void forceRefresh();
    bool shouldUseCache() const;

    void enablePersistence();
    void disablePersistence();
    bool isPersistenceEnabled() const;

private:
    MemoryCache _memoryCache;
    PersistentCache _persistentCache;

    bool _enabled;
    bool _persistenceEnabled;
    bool _forceRefreshFlag;

    ErrorManager* _errorManager;
    ClimateEvents* _events;
};

} // namespace AmelTech

#endif // AMELTECH_CACHE_MANAGER_H
