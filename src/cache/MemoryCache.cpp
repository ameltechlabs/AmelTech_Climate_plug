/*
 * MemoryCache.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "MemoryCache.h"

namespace AmelTech {

const char* cacheDataStateToString(CacheDataState state) {
    switch (state) {
        case CacheDataState::UNAVAILABLE: return "UNAVAILABLE";
        case CacheDataState::LIVE:        return "LIVE";
        case CacheDataState::CACHED:      return "CACHED";
        case CacheDataState::STALE:       return "STALE";
        default:                          return "UNKNOWN";
    }
}

MemoryCache::MemoryCache(unsigned long lifetimeMs)
    : _hasData(false), _storedAt(0), _lifetimeMs(lifetimeMs) {
}

void MemoryCache::store(const ClimateData& data) {
    _data = data;
    _hasData = true;
    _storedAt = millis();
}

const ClimateData& MemoryCache::retrieve() const {
    return _data; // caller should check getState() to interpret validity
}

void MemoryCache::clear() {
    _data.reset();
    _hasData = false;
    _storedAt = 0;
}

bool MemoryCache::isAvailable() const {
    return _hasData;
}

bool MemoryCache::isValid() const {
    return _hasData && (millis() - _storedAt) <= _lifetimeMs;
}

bool MemoryCache::isExpired() const {
    return _hasData && (millis() - _storedAt) > _lifetimeMs;
}

unsigned long MemoryCache::getAge() const {
    if (!_hasData) return 0;
    return millis() - _storedAt;
}

void MemoryCache::setLifetime(unsigned long ms) {
    _lifetimeMs = ms;
}

unsigned long MemoryCache::getLifetime() const {
    return _lifetimeMs;
}

CacheDataState MemoryCache::getState() const {
    if (!_hasData) return CacheDataState::UNAVAILABLE;
    if (isExpired()) return CacheDataState::STALE;
    return CacheDataState::CACHED;
    // Note: LIVE is a transient state the CACHE doesn't itself claim -
    // it's set by CacheManager immediately after a fresh fetch, before
    // the data has aged at all. See CacheManager.cpp.
}

} // namespace AmelTech
