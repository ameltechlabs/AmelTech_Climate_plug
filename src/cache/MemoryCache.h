/*
 * MemoryCache.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * Holds exactly one cached ClimateData snapshot with an expiry
 * lifetime, implementing the spec's cache data states: LIVE (fresh,
 * just fetched - not actually a cache concept, tracked by the
 * caller), CACHED (within lifetime), STALE (past lifetime, still
 * held), UNAVAILABLE (never populated). "Never present stale data as
 * live" is enforced by getCachedData() always returning the data
 * alongside its true state - callers cannot accidentally treat
 * expired data as fresh.
 */

#ifndef AMELTECH_MEMORY_CACHE_H
#define AMELTECH_MEMORY_CACHE_H

#include <Arduino.h>
#include "../data/ClimateData.h"

namespace AmelTech {

enum class CacheDataState {
    UNAVAILABLE,
    LIVE,
    CACHED,
    STALE
};

const char* cacheDataStateToString(CacheDataState state);

class MemoryCache {
public:
    explicit MemoryCache(unsigned long lifetimeMs = 1800000); // 30 min default

    void store(const ClimateData& data);
    const ClimateData& retrieve() const;

    void clear();

    bool isAvailable() const;
    bool isValid() const;   // within lifetime
    bool isExpired() const; // past lifetime

    unsigned long getAge() const; // ms since stored, or 0 if unavailable

    void setLifetime(unsigned long ms);
    unsigned long getLifetime() const;

    CacheDataState getState() const;

private:
    ClimateData _data;
    bool _hasData;
    unsigned long _storedAt;
    unsigned long _lifetimeMs;
};

} // namespace AmelTech

#endif // AMELTECH_MEMORY_CACHE_H
