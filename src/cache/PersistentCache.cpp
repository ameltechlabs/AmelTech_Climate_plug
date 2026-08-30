/*
 * PersistentCache.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "PersistentCache.h"

namespace AmelTech {

const char* PersistentCache::NVS_NAMESPACE = "amt_cache";

PersistentCache::PersistentCache()
    : _began(false), _hasStoredData(false), _errorManager(nullptr) {
}

void PersistentCache::attachErrorManager(ErrorManager* errorManager) {
    _errorManager = errorManager;
}

void PersistentCache::setError(ErrorCode code, const char* msg) {
    if (_errorManager) _errorManager->setError(code, msg);
}

bool PersistentCache::begin() {
    _began = _prefs.begin(NVS_NAMESPACE, false); // false = read/write mode
    if (!_began) {
        setError(ErrorCode::STORAGE_ERROR, "Failed to open NVS namespace for cache");
        return false;
    }

    _hasStoredData = _prefs.isKey("hasData") && _prefs.getBool("hasData", false);
    return true;
}

bool PersistentCache::save(const ClimateData& data) {
    if (!_began) {
        setError(ErrorCode::STORAGE_ERROR, "PersistentCache::begin() was not called");
        return false;
    }

    // Only the fields needed to reconstruct a usable ClimateData are
    // persisted, using has-flags packed into a single bitmask to
    // avoid ~20 separate NVS keys (each NVS key/value pair has
    // overhead; a packed approach is friendlier to flash wear and
    // matches spec's "avoid unnecessary...allocation" spirit).
    uint32_t flags = 0;
    if (data.hasTemperature)         flags |= (1UL << 0);
    if (data.hasRelativeHumidity)    flags |= (1UL << 1);
    if (data.hasDewPoint)            flags |= (1UL << 2);
    if (data.hasApparentTemperature) flags |= (1UL << 3);
    if (data.hasAtmosphericPressure) flags |= (1UL << 4);
    if (data.hasPrecipitation)       flags |= (1UL << 5);
    if (data.hasRain)                flags |= (1UL << 6);
    if (data.hasSnowfall)            flags |= (1UL << 7);
    if (data.hasWeatherCode)         flags |= (1UL << 8);
    if (data.hasCloudCover)          flags |= (1UL << 9);
    if (data.hasWindSpeed)           flags |= (1UL << 10);
    if (data.hasWindDirection)       flags |= (1UL << 11);
    if (data.hasWindGusts)           flags |= (1UL << 12);
    if (data.hasSolarRadiation)      flags |= (1UL << 13);
    if (data.hasUvIndex)             flags |= (1UL << 14);

    _prefs.putUInt("flags", flags);
    _prefs.putFloat("temp", data.temperature);
    _prefs.putFloat("hum", data.relativeHumidity);
    _prefs.putFloat("dew", data.dewPoint);
    _prefs.putFloat("appTemp", data.apparentTemperature);
    _prefs.putFloat("press", data.atmosphericPressure);
    _prefs.putFloat("precip", data.precipitation);
    _prefs.putFloat("rain", data.rain);
    _prefs.putFloat("snow", data.snowfall);
    _prefs.putInt("wcode", data.weatherCode);
    _prefs.putFloat("cloud", data.cloudCover);
    _prefs.putFloat("wspeed", data.windSpeed);
    _prefs.putFloat("wdir", data.windDirection);
    _prefs.putFloat("wgust", data.windGusts);
    _prefs.putFloat("solar", data.solarRadiation);
    _prefs.putFloat("uv", data.uvIndex);
    _prefs.putULong("storedAt", millis()); // relative reference point; see load() note
    _prefs.putBool("hasData", true);

    _hasStoredData = true;
    return true;
}

bool PersistentCache::load(ClimateData& out) {
    if (!_began) {
        setError(ErrorCode::STORAGE_ERROR, "PersistentCache::begin() was not called");
        return false;
    }
    if (!_hasStoredData) {
        return false;
    }

    out.reset();

    uint32_t flags = _prefs.getUInt("flags", 0);

    if (flags & (1UL << 0))  { out.temperature = _prefs.getFloat("temp", 0); out.hasTemperature = true; }
    if (flags & (1UL << 1))  { out.relativeHumidity = _prefs.getFloat("hum", 0); out.hasRelativeHumidity = true; }
    if (flags & (1UL << 2))  { out.dewPoint = _prefs.getFloat("dew", 0); out.hasDewPoint = true; }
    if (flags & (1UL << 3))  { out.apparentTemperature = _prefs.getFloat("appTemp", 0); out.hasApparentTemperature = true; }
    if (flags & (1UL << 4))  { out.atmosphericPressure = _prefs.getFloat("press", 0); out.hasAtmosphericPressure = true; }
    if (flags & (1UL << 5))  { out.precipitation = _prefs.getFloat("precip", 0); out.hasPrecipitation = true; }
    if (flags & (1UL << 6))  { out.rain = _prefs.getFloat("rain", 0); out.hasRain = true; }
    if (flags & (1UL << 7))  { out.snowfall = _prefs.getFloat("snow", 0); out.hasSnowfall = true; }
    if (flags & (1UL << 8))  { out.weatherCode = _prefs.getInt("wcode", -1); out.hasWeatherCode = true; }
    if (flags & (1UL << 9))  { out.cloudCover = _prefs.getFloat("cloud", 0); out.hasCloudCover = true; }
    if (flags & (1UL << 10)) { out.windSpeed = _prefs.getFloat("wspeed", 0); out.hasWindSpeed = true; }
    if (flags & (1UL << 11)) { out.windDirection = _prefs.getFloat("wdir", 0); out.hasWindDirection = true; }
    if (flags & (1UL << 12)) { out.windGusts = _prefs.getFloat("wgust", 0); out.hasWindGusts = true; }
    if (flags & (1UL << 13)) { out.solarRadiation = _prefs.getFloat("solar", 0); out.hasSolarRadiation = true; }
    if (flags & (1UL << 14)) { out.uvIndex = _prefs.getFloat("uv", 0); out.hasUvIndex = true; }

    // IMPORTANT: the stored "storedAt" was a millis() value from a
    // PREVIOUS boot, which is meaningless after a reboot resets
    // millis() to 0. We therefore mark restored data as receivedAt=0,
    // which DataValidator::isDataStale() and SerialDisplay both
    // already treat as "never/unknown freshness" - i.e. this data is
    // correctly surfaced as needing a fresh fetch, never as LIVE.
    out.receivedAt = 0;
    out.source = DataSource::CACHE;
    out.stale = true; // explicit: restored-from-flash data is always presented as stale until re-fetched

    return true;
}

bool PersistentCache::isAvailable() const {
    return _hasStoredData;
}

void PersistentCache::clear() {
    if (_began) {
        _prefs.clear();
    }
    _hasStoredData = false;
}

} // namespace AmelTech
