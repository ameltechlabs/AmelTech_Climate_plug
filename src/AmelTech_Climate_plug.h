/*
 * AmelTech_Climate_plug.h
 * ============================================================
 * AmelTech_Climate_plug - ESP32 Climate Monitoring Library
 * ============================================================
 *
 * PHASE STATUS: every module in the spec's architecture (core,
 * network, providers, request, data, location, urls, sensors, cache,
 * serial, platforms, storage, security, diagnostics) has a real
 * implementation, and this top-level class wires all of them
 * together. Functions are marked below as either:
 *   [WIRED] - backed by a real, tested module instance
 *   [PENDING] - declared for API completeness but genuinely not yet
 *               implemented anywhere in the codebase; calling these
 *               returns a documented "not yet implemented" result
 *               rather than silently doing nothing.
 *
 * Remaining [PENDING] items are narrow: multi-location/URL flash
 * persistence across reboots is wired via ConfigurationManager but
 * not yet auto-invoked on every add/edit (call saveLocationsToFlash()/
 * saveURLsToFlash() explicitly - see docs/API.md), and the Serial
 * command layer's location/url/sensor/cache/platform sub-commands are
 * still registered as pending text rather than dispatching to the now-
 * wired managers (see serial/CommandManager.cpp for the next step).
 *
 * See README.md and docs/Architecture.md for the full design.
 * ============================================================
 */

#ifndef AMELTECH_CLIMATE_PLUG_H
#define AMELTECH_CLIMATE_PLUG_H

#include <Arduino.h>
#include <functional>

#include "core/ClimateCore.h"
#include "core/ClimateController.h"
#include "core/ClimateState.h"
#include "core/ClimateScheduler.h"
#include "core/ClimateEvents.h"
#include "data/ClimateData.h"
#include "diagnostics/ErrorManager.h"
#include "serial/CommandManager.h"
#include "location/LocationManager.h"
#include "urls/URLManager.h"
#include "urls/TXManager.h"
#include "urls/RXManager.h"
#include "sensors/SensorManager.h"
#include "cache/CacheManager.h"
#include "platforms/PlatformManager.h"
#include "storage/ConfigurationManager.h"
#include "security/SecurityManager.h"

namespace AmelTech {

// Configuration struct for the begin(configuration) overload.
struct ClimateConfiguration {
    String ssid;
    String password;
    unsigned long wifiTimeoutMs;
    unsigned long updateIntervalMs;
    bool autoStart; // if true, starts the scheduler immediately in begin()
    bool enablePersistence; // if true, loads/saves WiFi+location+URL config to flash via ConfigurationManager

    ClimateConfiguration()
        : ssid(""), password(""), wifiTimeoutMs(15000),
          updateIntervalMs(600000), autoStart(true), enablePersistence(false) {}
};

typedef std::function<void(const ClimateData&)> DataUpdateCallback;
typedef std::function<void(ErrorCode, const String&)> ErrorCallback;
typedef std::function<void(const EventData&)> GeneralEventCallback;

class AmelTechClimate {
public:
    AmelTechClimate();

    // ===================== LIFECYCLE ===================== [WIRED]
    bool begin();
    bool begin(const char* ssid, const char* password);
    bool begin(const ClimateConfiguration& configuration);

    void end();
    void loop();

    void restart();
    void reset();
    void factoryReset(); // [WIRED] now genuinely clears flash config via ConfigurationManager::factoryReset(), not just in-memory state

    bool isInitialized() const;
    bool isReady() const;

    SystemState getState() const;
    String getVersion() const;
    unsigned long getUptime() const;

    // ===================== DATA UPDATES ===================== [WIRED]
    void update();
    bool updateNow();
    bool request();
    void cancelRequest();

    bool isUpdating() const;

    const ClimateData& getClimateData() const;

    // ===================== CALLBACKS ===================== [WIRED]
    void onDataUpdate(DataUpdateCallback callback);
    void onError(ErrorCallback callback);
    void onEvent(GeneralEventCallback callback);

    // ===================== WI-FI ===================== [WIRED]
    void setWiFi(const char* ssid, const char* password);
    void setWiFiCredentials(const char* ssid, const char* password);
    void setSSID(const char* ssid);
    void setPassword(const char* password);

    bool connectWiFi();
    void disconnectWiFi();
    bool reconnectWiFi();

    bool isWiFiConnected() const;
    WiFiState getWiFiStatus() const;

    int32_t getRSSI() const;
    String getIP() const;
    String getGateway() const;
    String getSubnet() const;
    String getDNS() const;

    void setWiFiTimeout(unsigned long ms);
    unsigned long getWiFiTimeout() const;

    void setReconnectInterval(unsigned long ms);
    unsigned long getReconnectInterval() const;

    void enableAutoReconnect();
    void disableAutoReconnect();

    // ===================== PROVIDER ===================== [WIRED]
    void useOpenMeteo();
    void useCustomProvider(); // [WIRED] now routes through URLManager's active URL if one is selected
    Provider* getProvider();
    String getProviderName() const;
    String getProviderStatus() const;
    bool isOpenMeteo() const;
    String getOpenMeteoConfiguration() const;
    void setOpenMeteoVariables(const bool enabledFlags[(int)ClimateParameter::PARAMETER_COUNT]);
    void resetOpenMeteoVariables();

    // ===================== CLIMATE GETTERS ===================== [WIRED]
    float getTemperature();
    float getRelativeHumidity();
    float getDewPoint();
    float getApparentTemperature();

    float getAtmosphericPressure();

    float getPrecipitation();
    float getRain();
    float getSnowfall();

    int getWeatherCode();
    String getWeatherCondition();

    float getCloudCover();

    float getWindSpeed();
    float getWindDirection();
    float getWindGusts();

    float getSolarRadiation();
    float getUVIndex();

    float getEvapotranspiration();

    float getSoilTemperature();
    float getSoilMoisture();

    float getFreezingLevel();
    float getVisibility();
    float getCAPE();

    // ===================== DATA VALIDATION ===================== [WIRED]
    bool validateClimateData();
    bool isDataValid() const;
    bool isDataStale() const;

    // ===================== LOCATION ===================== [WIRED]
    // setLocation() sets/updates the single ACTIVE location (used by
    // update()/updateNow()) via LocationManager. The multi-location
    // CRUD functions below now operate on LocationManager's real
    // fixed-capacity list (see location/LocationManager.h,
    // MAX_LOCATIONS = 10).
    void setLocation(const char* name, float latitude, float longitude);
    float getLatitude() const;
    float getLongitude() const;
    String getLocationName() const;

    bool addLocation(const char* name, float latitude, float longitude);
    bool editLocation(int id, const char* name, float latitude, float longitude);
    bool deleteLocation(int id);
    bool selectLocation(int id);
    int getLocationCount() const;

    // Persists the current in-memory location list to flash via
    // ConfigurationManager. Not called automatically on every
    // add/edit (that would wear flash on every command) - call this
    // explicitly after making the changes you want to keep.
    bool saveLocationsToFlash();
    bool loadLocationsFromFlash();

    // ===================== CUSTOM URLS / TX / RX ===================== [WIRED]
    bool addURL(const char* name, const char* url, URLDirection direction = URLDirection::TX,
                URLMethod method = URLMethod::GET, URLFormat format = URLFormat::JSON);
    bool editURL(int id, const char* name, const char* url);
    bool deleteURL(int id);
    bool selectURL(int id);
    int getURLCount() const;
    String getActiveURLName() const;

    bool saveURLsToFlash();
    bool loadURLsFromFlash();

    bool sendClimateDataTX();          // sends current ClimateData to the configured TX URL
    bool sendJSON(const char* json);   // raw TX send
    void setTXURL(const char* url);
    void enableTX();
    void disableTX();

    bool receiveRX();                  // fetches from the configured RX URL
    bool hasReceivedData() const;
    String readReceivedData() const;
    void setRXURL(const char* url);
    void enableRX();
    void disableRX();

    // ===================== SENSORS ===================== [WIRED]
    // Sensor objects (DHTAdapter, BME280Adapter, etc.) are constructed
    // by the user's sketch (they need pin/I2C params this library
    // can't know) and registered here as pointers - see
    // examples/08_DHT through 11_SHT31.
    int8_t registerSensor(Sensor* sensor);
    uint8_t scanSensors();
    uint8_t getSensorCount() const;
    bool readAllSensorsIntoLocalData(); // merges all enabled sensor readings into the internal "local" ClimateData snapshot used by hybrid mode

    // ===================== HYBRID MODE ===================== [WIRED]
    void setMode(DataSource mode); // REMOTE, LOCAL, or HYBRID (any other value is treated as REMOTE)
    DataSource getMode() const;
    void enableHybrid();
    void disableHybrid();
    void useRemoteData();
    void useLocalData();

    // ===================== CACHE ===================== [WIRED]
    void enableCache();
    void disableCache();
    void setCacheLifetime(unsigned long ms);
    unsigned long getCacheLifetime() const;
    void clearCache();
    bool isCacheValid() const;
    unsigned long getCacheAge() const;
    void forceRefresh();

    // ===================== PLATFORMS ===================== [WIRED]
    // REST and Webhook work immediately (no optional dependency).
    // MQTT/Blynk require the user's sketch to construct the adapter
    // (optional library) and attach it - see examples/13_Blynk,
    // examples/14_MQTT.
    void setRESTEndpoint(const char* url);
    void enableRESTPlatform();
    void disableRESTPlatform();

    void setWebhookURL(const char* url);
    void enableWebhookPlatform();
    void disableWebhookPlatform();
    bool triggerWebhook();

    void attachMQTTAdapter(PlatformAdapter* mqttAdapter);
    void attachBlynkAdapter(PlatformAdapter* blynkAdapter);
    void enablePlatform(PlatformType type);
    void disablePlatform(PlatformType type);

    uint8_t publishToAllPlatforms(); // publishes current ClimateData to every enabled+connected platform
    String getPlatformStatus() const;

    // ===================== SECURITY ===================== [WIRED]
    void setCredential(const char* name, const char* value);
    String getMaskedCredential(const char* name) const;

    // ===================== ERROR HANDLING ===================== [WIRED]
    bool hasError() const;
    ErrorCode getLastError() const;
    ErrorCode getErrorCode() const;
    String getErrorMessage() const;
    void clearError();

    // ===================== SERIAL COMMAND INTERFACE ===================== [WIRED - P1 command set]
    void handleSerialLine(const String& line);
    void handleSerial();

private:
    ClimateCore _core;
    ClimateController _controller;
    ClimateScheduler _scheduler;
    CommandManager _commands;

    LocationManager _locations;
    URLManager _urls;
    TXManager _tx;
    RXManager _rx;
    SensorManager _sensors;
    CacheManager _cache;
    PlatformManager _platforms;
    ConfigurationManager _config;
    SecurityManager _security;

    ClimateData _localData; // most recent merged local-sensor snapshot, used by hybrid mode
    DataSource _mode;       // REMOTE, LOCAL, or HYBRID - governs what getClimateData()/getters return

    DataUpdateCallback _onDataUpdate;
    ErrorCallback _onError;
    GeneralEventCallback _onEvent;

    String _serialBuffer;

    void wireEventBridge();
    void applyConfiguration(const ClimateConfiguration& cfg);

    // Returns the ClimateData that should back the public getters,
    // honoring the current hybrid mode: REMOTE -> controller's data,
    // LOCAL -> _localData, HYBRID -> a merge of both (computed fresh
    // each call so it always reflects the latest of each source).
    ClimateData effectiveData() const;
};

} // namespace AmelTech

#endif // AMELTECH_CLIMATE_PLUG_H
