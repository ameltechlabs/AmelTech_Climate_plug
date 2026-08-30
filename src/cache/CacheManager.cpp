/*
 * CacheManager.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "CacheManager.h"

namespace AmelTech {

CacheManager::CacheManager()
    : _enabled(true), _persistenceEnabled(false), _forceRefreshFlag(false),
      _errorManager(nullptr), _events(nullptr) {
}

void CacheManager::attachErrorManager(ErrorManager* errorManager) {
    _errorManager = errorManager;
    _persistentCache.attachErrorManager(errorManager);
}

void CacheManager::attachEvents(ClimateEvents* events) {
    _events = events;
}

bool CacheManager::begin() {
    return _persistentCache.begin();
}

void CacheManager::enableCache() {
    _enabled = true;
}

void CacheManager::disableCache() {
    _enabled = false;
}

bool CacheManager::isCacheEnabled() const {
    return _enabled;
}

void CacheManager::setCacheLifetime(unsigned long ms) {
    _memoryCache.setLifetime(ms);
}

unsigned long CacheManager::getCacheLifetime() const {
    return _memoryCache.getLifetime();
}

void CacheManager::saveCache(const ClimateData& data) {
    if (!_enabled) return;

    _memoryCache.store(data);

    if (_persistenceEnabled) {
        _persistentCache.save(data);
    }

    if (_events) _events->emit(Event::CACHE_UPDATED);
}

bool CacheManager::loadCache(ClimateData& out) {
    if (!_enabled) return false;

    if (_memoryCache.isValid()) {
        out = _memoryCache.retrieve();
        return true;
    }

    if (_persistenceEnabled && _persistentCache.isAvailable()) {
        return _persistentCache.load(out);
    }

    // Memory cache may still hold expired (STALE) data worth
    // returning rather than nothing - the caller decides via
    // getState() whether STALE is acceptable for their use case.
    if (_memoryCache.isAvailable()) {
        out = _memoryCache.retrieve();
        return true;
    }

    return false;
}

void CacheManager::clearCache() {
    _memoryCache.clear();
    if (_persistenceEnabled) {
        _persistentCache.clear();
    }
    if (_events) _events->emit(Event::CACHE_EXPIRED);
}

bool CacheManager::isCacheAvailable() const {
    return _memoryCache.isAvailable() || (_persistenceEnabled && _persistentCache.isAvailable());
}

bool CacheManager::isCacheValid() const {
    return _memoryCache.isValid();
}

bool CacheManager::isCacheExpired() const {
    return _memoryCache.isExpired();
}

const ClimateData& CacheManager::getCachedData() const {
    return _memoryCache.retrieve();
}

unsigned long CacheManager::getCacheAge() const {
    return _memoryCache.getAge();
}

CacheDataState CacheManager::getState() const {
    return _memoryCache.getState();
}

void CacheManager::useCache() {
    _forceRefreshFlag = false;
}

void CacheManager::forceRefresh() {
    _forceRefreshFlag = true;
}

bool CacheManager::shouldUseCache() const {
    if (_forceRefreshFlag) return false;
    return _enabled && _memoryCache.isValid();
}

void CacheManager::enablePersistence() {
    _persistenceEnabled = true;
}

void CacheManager::disablePersistence() {
    _persistenceEnabled = false;
}

bool CacheManager::isPersistenceEnabled() const {
    return _persistenceEnabled;
}

} // namespace AmelTech
