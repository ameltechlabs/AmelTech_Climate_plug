/*
 * ConfigurationManager.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "ConfigurationManager.h"

namespace AmelTech {

ConfigurationManager::ConfigurationManager() : _began(false), _errorManager(nullptr) {
}

void ConfigurationManager::attachErrorManager(ErrorManager* errorManager) {
    _errorManager = errorManager;
    _wifiStore.attachErrorManager(errorManager);
    _locationStore.attachErrorManager(errorManager);
    _urlStore.attachErrorManager(errorManager);
    _sensorStore.attachErrorManager(errorManager);
    _platformStore.attachErrorManager(errorManager);
    _metaStore.attachErrorManager(errorManager);
}

void ConfigurationManager::setError(ErrorCode code, const char* msg) {
    if (_errorManager) _errorManager->setError(code, msg);
}

bool ConfigurationManager::begin() {
    // Namespace names kept <=15 chars per ESP32 NVS constraint.
    bool ok = true;
    ok &= _wifiStore.begin("amt_wifi", false);
    ok &= _locationStore.begin("amt_loc", false);
    ok &= _urlStore.begin("amt_url", false);
    ok &= _sensorStore.begin("amt_sensor", false);
    ok &= _platformStore.begin("amt_plat", false);
    ok &= _metaStore.begin("amt_meta", false);

    if (!_metaStore.exists("cfgVersion")) {
        _metaStore.save("cfgVersion", (int)CURRENT_CONFIG_VERSION);
    }

    _began = ok;
    if (!ok) {
        setError(ErrorCode::STORAGE_ERROR, "One or more configuration namespaces failed to open");
    }
    return ok;
}

bool ConfigurationManager::save() {
    return _began; // each domain persists independently via its saveX() call
}

bool ConfigurationManager::load() {
    return _began; // each domain loads independently via its loadX() call
}

bool ConfigurationManager::remove(const char* key) {
    // Ambiguous which domain a bare key belongs to without a
    // namespace hint; this generic form removes from the metadata
    // store only, matching its use for one-off flags. Domain-specific
    // removal should go through that domain's saveX() with updated
    // contents instead.
    return _metaStore.remove(key);
}

void ConfigurationManager::clear() {
    _wifiStore.clear();
    _locationStore.clear();
    _urlStore.clear();
    _sensorStore.clear();
    _platformStore.clear();
    // _metaStore (config version) deliberately NOT cleared by a
    // partial clear() - only factoryReset() resets the version.
}

bool ConfigurationManager::saveWiFi(const String& ssid, const String& password) {
    if (!_began) { setError(ErrorCode::STORAGE_ERROR, "ConfigurationManager::begin() not called"); return false; }
    bool ok = _wifiStore.save("ssid", ssid);
    ok &= _wifiStore.save("pass", password);
    return ok;
}

bool ConfigurationManager::loadWiFi(String& ssidOut, String& passwordOut) {
    if (!_began) return false;
    if (!_wifiStore.exists("ssid")) return false;
    ssidOut = _wifiStore.loadString("ssid", "");
    passwordOut = _wifiStore.loadString("pass", "");
    return ssidOut.length() > 0;
}

bool ConfigurationManager::saveLocations(const Location locations[], uint8_t count, int activeId) {
    if (!_began) { setError(ErrorCode::STORAGE_ERROR, "ConfigurationManager::begin() not called"); return false; }

    uint8_t n = (count < MAX_PERSISTED_LOCATIONS) ? count : MAX_PERSISTED_LOCATIONS;
    _locationStore.save("count", (int)n);
    _locationStore.save("activeId", activeId);

    for (uint8_t i = 0; i < n; i++) {
        String prefix = "l" + String(i) + "_";
        _locationStore.save((prefix + "id").c_str(), locations[i].id);
        _locationStore.save((prefix + "name").c_str(), locations[i].name);
        _locationStore.save((prefix + "lat").c_str(), locations[i].latitude);
        _locationStore.save((prefix + "lon").c_str(), locations[i].longitude);
        _locationStore.save((prefix + "en").c_str(), locations[i].enabled);
    }

    return true;
}

uint8_t ConfigurationManager::loadLocations(Location outBuffer[], uint8_t maxCount, int& activeIdOut) {
    if (!_began) return 0;

    int storedCount = _locationStore.loadInt("count", 0);
    activeIdOut = _locationStore.loadInt("activeId", -1);

    uint8_t n = ((uint8_t)storedCount < maxCount) ? (uint8_t)storedCount : maxCount;
    n = (n < MAX_PERSISTED_LOCATIONS) ? n : MAX_PERSISTED_LOCATIONS;

    for (uint8_t i = 0; i < n; i++) {
        String prefix = "l" + String(i) + "_";
        outBuffer[i].id = _locationStore.loadInt((prefix + "id").c_str(), -1);
        outBuffer[i].name = _locationStore.loadString((prefix + "name").c_str(), "");
        outBuffer[i].latitude = _locationStore.loadFloat((prefix + "lat").c_str(), 0.0f);
        outBuffer[i].longitude = _locationStore.loadFloat((prefix + "lon").c_str(), 0.0f);
        outBuffer[i].enabled = _locationStore.loadBool((prefix + "en").c_str(), true);
    }

    return n;
}

bool ConfigurationManager::saveURLs(const URLRecord records[], uint8_t count, int activeId) {
    if (!_began) { setError(ErrorCode::STORAGE_ERROR, "ConfigurationManager::begin() not called"); return false; }

    uint8_t n = (count < MAX_PERSISTED_URLS) ? count : MAX_PERSISTED_URLS;
    _urlStore.save("count", (int)n);
    _urlStore.save("activeId", activeId);

    for (uint8_t i = 0; i < n; i++) {
        String prefix = "u" + String(i) + "_";
        _urlStore.save((prefix + "id").c_str(), records[i].id);
        _urlStore.save((prefix + "name").c_str(), records[i].name);
        _urlStore.save((prefix + "url").c_str(), records[i].url);
        _urlStore.save((prefix + "dir").c_str(), (int)records[i].direction);
        _urlStore.save((prefix + "meth").c_str(), (int)records[i].method);
        _urlStore.save((prefix + "fmt").c_str(), (int)records[i].format);
        _urlStore.save((prefix + "en").c_str(), records[i].enabled);
    }

    return true;
}

uint8_t ConfigurationManager::loadURLs(URLRecord outBuffer[], uint8_t maxCount, int& activeIdOut) {
    if (!_began) return 0;

    int storedCount = _urlStore.loadInt("count", 0);
    activeIdOut = _urlStore.loadInt("activeId", -1);

    uint8_t n = ((uint8_t)storedCount < maxCount) ? (uint8_t)storedCount : maxCount;
    n = (n < MAX_PERSISTED_URLS) ? n : MAX_PERSISTED_URLS;

    for (uint8_t i = 0; i < n; i++) {
        String prefix = "u" + String(i) + "_";
        outBuffer[i].id = _urlStore.loadInt((prefix + "id").c_str(), -1);
        outBuffer[i].name = _urlStore.loadString((prefix + "name").c_str(), "");
        outBuffer[i].url = _urlStore.loadString((prefix + "url").c_str(), "");
        outBuffer[i].direction = (URLDirection)_urlStore.loadInt((prefix + "dir").c_str(), 0);
        outBuffer[i].method = (URLMethod)_urlStore.loadInt((prefix + "meth").c_str(), 0);
        outBuffer[i].format = (URLFormat)_urlStore.loadInt((prefix + "fmt").c_str(), 0);
        outBuffer[i].enabled = _urlStore.loadBool((prefix + "en").c_str(), true);
    }

    return n;
}

bool ConfigurationManager::saveSensors(const uint8_t sensorTypes[], const bool enabledFlags[], uint8_t count) {
    if (!_began) { setError(ErrorCode::STORAGE_ERROR, "ConfigurationManager::begin() not called"); return false; }

    _sensorStore.save("count", (int)count);
    for (uint8_t i = 0; i < count; i++) {
        String prefix = "s" + String(i) + "_";
        _sensorStore.save((prefix + "type").c_str(), (int)sensorTypes[i]);
        _sensorStore.save((prefix + "en").c_str(), enabledFlags[i]);
    }
    return true;
}

uint8_t ConfigurationManager::loadSensors(uint8_t typesOut[], bool enabledOut[], uint8_t maxCount) {
    if (!_began) return 0;

    int storedCount = _sensorStore.loadInt("count", 0);
    uint8_t n = ((uint8_t)storedCount < maxCount) ? (uint8_t)storedCount : maxCount;

    for (uint8_t i = 0; i < n; i++) {
        String prefix = "s" + String(i) + "_";
        typesOut[i] = (uint8_t)_sensorStore.loadInt((prefix + "type").c_str(), 0);
        enabledOut[i] = _sensorStore.loadBool((prefix + "en").c_str(), true);
    }
    return n;
}

bool ConfigurationManager::savePlatforms(bool restEnabled, bool webhookEnabled, bool mqttEnabled, bool blynkEnabled) {
    if (!_began) { setError(ErrorCode::STORAGE_ERROR, "ConfigurationManager::begin() not called"); return false; }

    bool ok = _platformStore.save("rest", restEnabled);
    ok &= _platformStore.save("webhook", webhookEnabled);
    ok &= _platformStore.save("mqtt", mqttEnabled);
    ok &= _platformStore.save("blynk", blynkEnabled);
    return ok;
}

bool ConfigurationManager::loadPlatforms(bool& restEnabled, bool& webhookEnabled, bool& mqttEnabled, bool& blynkEnabled) {
    if (!_began) return false;

    restEnabled = _platformStore.loadBool("rest", false);
    webhookEnabled = _platformStore.loadBool("webhook", false);
    mqttEnabled = _platformStore.loadBool("mqtt", false);
    blynkEnabled = _platformStore.loadBool("blynk", false);
    return true;
}

bool ConfigurationManager::factoryReset() {
    if (!_began) { setError(ErrorCode::STORAGE_ERROR, "ConfigurationManager::begin() not called"); return false; }

    _wifiStore.clear();
    _locationStore.clear();
    _urlStore.clear();
    _sensorStore.clear();
    _platformStore.clear();
    _metaStore.clear();

    _metaStore.save("cfgVersion", (int)CURRENT_CONFIG_VERSION);
    return true;
}

uint16_t ConfigurationManager::getConfigVersion() const {
    return (uint16_t)_metaStore.loadInt("cfgVersion", (int)CURRENT_CONFIG_VERSION);
}

void ConfigurationManager::setConfigVersion(uint16_t version) {
    _metaStore.save("cfgVersion", (int)version);
}

bool ConfigurationManager::isMigrationRequired() const {
    return configNeedsMigration(getConfigVersion());
}

bool ConfigurationManager::migrateConfiguration() {
    // See ConfigMigration.h - no migration steps exist yet since this
    // is the library's first-ever schema version. This call is safe
    // and correctly reports success (nothing to migrate) rather than
    // failure.
    uint16_t stored = getConfigVersion();
    if (!configNeedsMigration(stored)) return true;

    setError(ErrorCode::CONFIG_ERROR, "Migration path not yet implemented for this version gap");
    return false;
}

bool ConfigurationManager::validateConfiguration() const {
    if (!_began) return false;

    // Basic sanity: if a WiFi SSID is stored, it shouldn't be empty;
    // if locations/URLs claim a count > their max, storage is corrupt.
    if (_wifiStore.exists("ssid") && _wifiStore.loadString("ssid", "").length() == 0) return false;

    int locCount = _locationStore.loadInt("count", 0);
    if (locCount < 0 || locCount > MAX_PERSISTED_LOCATIONS) return false;

    int urlCount = _urlStore.loadInt("count", 0);
    if (urlCount < 0 || urlCount > MAX_PERSISTED_URLS) return false;

    return true;
}

bool ConfigurationManager::repairConfiguration() {
    if (!_began) return false;

    bool anyRepaired = false;

    if (_wifiStore.exists("ssid") && _wifiStore.loadString("ssid", "").length() == 0) {
        _wifiStore.clear();
        anyRepaired = true;
    }

    int locCount = _locationStore.loadInt("count", 0);
    if (locCount < 0 || locCount > MAX_PERSISTED_LOCATIONS) {
        _locationStore.clear();
        anyRepaired = true;
    }

    int urlCount = _urlStore.loadInt("count", 0);
    if (urlCount < 0 || urlCount > MAX_PERSISTED_URLS) {
        _urlStore.clear();
        anyRepaired = true;
    }

    return anyRepaired;
}

} // namespace AmelTech
