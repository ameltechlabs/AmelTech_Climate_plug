/*
 * ConfigurationManager.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * Builds on StorageManager to persist the specific configuration
 * domains the spec's STORAGE section lists: WiFi, locations, custom
 * URLs, sensor config, platform config. Each domain gets its own NVS
 * namespace (kept under 15 chars per ESP32 constraint) so clearing
 * one domain (e.g. factoryReset on just locations) doesn't touch
 * others.
 *
 * SECURITY NOTE: WiFi password is stored via NVS, which on ESP32 is
 * NOT encrypted by default (unless the user has enabled NVS
 * encryption at the flash/partition level, which is outside this
 * library's control). This is documented in docs/Security.md rather
 * than silently assumed safe - see that file for guidance on
 * enabling flash encryption for genuinely sensitive deployments.
 */

#ifndef AMELTECH_CONFIGURATION_MANAGER_H
#define AMELTECH_CONFIGURATION_MANAGER_H

#include <Arduino.h>
#include "StorageManager.h"
#include "ConfigVersion.h"
#include "ConfigMigration.h"
#include "../location/Location.h"
#include "../urls/URLRecord.h"
#include "../diagnostics/ErrorManager.h"

namespace AmelTech {

class ConfigurationManager {
public:
    static const uint8_t MAX_PERSISTED_LOCATIONS = 10;
    static const uint8_t MAX_PERSISTED_URLS = 10;

    ConfigurationManager();

    void attachErrorManager(ErrorManager* errorManager);

    bool begin();

    bool save(); // no-op passthrough for spec API completeness; each domain saves independently via saveX() below
    bool load(); // no-op passthrough; see loadX() below
    bool remove(const char* key);
    void clear(); // clears ALL configuration domains

    bool saveWiFi(const String& ssid, const String& password);
    bool loadWiFi(String& ssidOut, String& passwordOut);

    bool saveLocations(const Location locations[], uint8_t count, int activeId);
    uint8_t loadLocations(Location outBuffer[], uint8_t maxCount, int& activeIdOut);

    bool saveURLs(const URLRecord records[], uint8_t count, int activeId);
    uint8_t loadURLs(URLRecord outBuffer[], uint8_t maxCount, int& activeIdOut);

    // Sensor config persisted here is limited to which sensor TYPES
    // were enabled/registered (SensorType + enabled flag) - pin/I2C
    // address assignments are NOT persisted since those are
    // constructor parameters the user's sketch supplies each boot
    // (see sensors/SensorRegistry.h design note on why sensors
    // aren't library-owned objects).
    bool saveSensors(const uint8_t sensorTypes[], const bool enabledFlags[], uint8_t count);
    uint8_t loadSensors(uint8_t typesOut[], bool enabledOut[], uint8_t maxCount);

    // Platform config: which platforms are enabled + their
    // non-secret settings (broker host/port, REST endpoint, webhook
    // URL). Auth tokens/passwords are intentionally NOT persisted
    // here by default - see docs/Security.md for rationale; advanced
    // users needing that can extend this domain themselves.
    bool savePlatforms(bool restEnabled, bool webhookEnabled, bool mqttEnabled, bool blynkEnabled);
    bool loadPlatforms(bool& restEnabled, bool& webhookEnabled, bool& mqttEnabled, bool& blynkEnabled);

    bool factoryReset(); // clears every domain + resets config version

    uint16_t getConfigVersion() const;
    void setConfigVersion(uint16_t version);
    bool isMigrationRequired() const;
    bool migrateConfiguration();

    bool validateConfiguration() const; // basic sanity check across all domains
    bool repairConfiguration();          // clears any domain that fails validation, leaving others intact

private:
    StorageManager _wifiStore;
    StorageManager _locationStore;
    StorageManager _urlStore;
    StorageManager _sensorStore;
    StorageManager _platformStore;
    StorageManager _metaStore; // holds config version

    bool _began;
    ErrorManager* _errorManager;

    void setError(ErrorCode code, const char* msg);
};

} // namespace AmelTech

#endif // AMELTECH_CONFIGURATION_MANAGER_H
