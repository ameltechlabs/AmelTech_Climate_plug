/*
 * AmelTech_Climate_plug.cpp
 * AmelTech_Climate_plug
 * All 14 spec modules wired together (see header for [WIRED]/[PENDING] map)
 */

#include "AmelTech_Climate_plug.h"

namespace AmelTech {

static const char* LIBRARY_VERSION = "0.2.0-wired";

AmelTechClimate::AmelTechClimate()
    : _mode(DataSource::REMOTE), _serialBuffer("") {
}

void AmelTechClimate::wireEventBridge() {
    _core.wireSubsystems();

    _controller.attach(&_core.state(), &_scheduler, &_core.events(),
                        &_core.providers(), &_core.connection(), &_core.errors());

    _locations.attachErrorManager(&_core.errors());
    _locations.attachEvents(&_core.events());

    _urls.attachErrorManager(&_core.errors());
    _urls.attachEvents(&_core.events());

    _tx.attachHTTP(&_core.http());
    _tx.attachErrorManager(&_core.errors());

    _rx.attachHTTP(&_core.http());
    _rx.attachErrorManager(&_core.errors());

    _sensors.attachErrorManager(&_core.errors());
    _sensors.attachEvents(&_core.events());

    _cache.attachErrorManager(&_core.errors());
    _cache.attachEvents(&_core.events());

    _platforms.attachErrorManager(&_core.errors());
    _platforms.attachEvents(&_core.events());
    _platforms.attachHTTP(&_core.http());

    _config.attachErrorManager(&_core.errors());
    _security.attachErrorManager(&_core.errors());

    _commands.attachEvents(&_core.events());

    _commands.setDataCallback([this]() -> const ClimateData& {
        static ClimateData snapshot; // static: caller (LiveStatus/SerialDisplay) takes a const& and reads it synchronously within the same call, so lifetime is safe
        snapshot = effectiveData();
        return snapshot;
    });
    _commands.setLocationNameCallback([this]() -> String {
        return getLocationName();
    });
    _commands.setLatitudeCallback([this]() -> float {
        return getLatitude();
    });
    _commands.setLongitudeCallback([this]() -> float {
        return getLongitude();
    });
    _commands.setProviderNameCallback([this]() -> String {
        return _core.providers().getProviderName();
    });
    _commands.setUpdateNowCallback([this]() -> bool {
        return updateNow();
    });

    _commands.setLocationAddCallback([this](const String& name, float lat, float lon) -> bool {
        return addLocation(name.c_str(), lat, lon);
    });
    _commands.setLocationSelectCallback([this](int id) -> bool {
        return selectLocation(id);
    });
    _commands.setLocationDeleteCallback([this](int id) -> bool {
        return deleteLocation(id);
    });
    _commands.setLocationListCallback([this]() -> String {
        Location buffer[LocationManager::MAX_LOCATIONS];
        uint8_t n = _locations.listLocations(buffer, LocationManager::MAX_LOCATIONS);
        if (n == 0) return "";
        const Location* active = _locations.getActiveLocation();
        String out = "";
        for (uint8_t i = 0; i < n; i++) {
            bool isActive = active && active->id == buffer[i].id;
            out += String(isActive ? "* " : "  ") + "[" + String(buffer[i].id) + "] " + buffer[i].name +
                   " (" + String(buffer[i].latitude, 4) + ", " + String(buffer[i].longitude, 4) + ")\n";
        }
        return out;
    });

    _commands.setURLAddCallback([this](const String& name, const String& url) -> bool {
        return addURL(name.c_str(), url.c_str());
    });
    _commands.setURLSelectCallback([this](int id) -> bool {
        return selectURL(id);
    });
    _commands.setURLDeleteCallback([this](int id) -> bool {
        return deleteURL(id);
    });
    _commands.setURLListCallback([this]() -> String {
        URLRecord buffer[URLManager::MAX_URLS];
        uint8_t n = _urls.listURLs(buffer, URLManager::MAX_URLS);
        if (n == 0) return "";
        const URLRecord* active = _urls.getActiveURL();
        String out = "";
        for (uint8_t i = 0; i < n; i++) {
            bool isActive = active && active->id == buffer[i].id;
            out += String(isActive ? "* " : "  ") + "[" + String(buffer[i].id) + "] " + buffer[i].name +
                   " -> " + buffer[i].url + " (" + urlDirectionToString(buffer[i].direction) + ")\n";
        }
        return out;
    });
    _commands.setURLTestCallback([this](int id) -> bool {
        return _urls.testURL(id);
    });

    _commands.setSensorScanCallback([this]() -> bool {
        return scanSensors() > 0;
    });
    _commands.setSensorStatusCallback([this]() -> String {
        uint8_t count = _sensors.getSensorCount();
        if (count == 0) return "";
        String out = "";
        for (uint8_t i = 0; i < SensorRegistry::MAX_SENSORS; i++) {
            String name = _sensors.getSensorName((int8_t)i);
            if (name == "N/A") continue;
            out += "  " + name + ": " + sensorStatusToString(_sensors.getSensorStatus((int8_t)i)) + "\n";
        }
        return out;
    });

    _commands.setCacheStatusCallback([this]() -> String {
        String out = "Cache state : " + String(cacheDataStateToString(_cache.getState())) + "\n";
        out += "Cache age   : " + String(_cache.getCacheAge() / 1000) + "s\n";
        out += "Lifetime    : " + String(_cache.getCacheLifetime() / 1000) + "s\n";
        out += "Persistence : " + String(_cache.isPersistenceEnabled() ? "enabled" : "disabled");
        return out;
    });
    _commands.setCacheClearCallback([this]() -> bool {
        _cache.clearCache();
        return true;
    });

    _commands.setNetworkStatusCallback([this]() -> String {
        String out = "WiFi status : " + String(isWiFiConnected() ? "connected" : "disconnected") + "\n";
        out += "IP address  : " + getIP() + "\n";
        out += "RSSI        : " + String(getRSSI()) + " dBm\n";
        out += "Gateway     : " + getGateway();
        return out;
    });
    _commands.setProviderStatusCallback([this]() -> String {
        return getProviderStatus();
    });
    _commands.setPlatformStatusCallback([this]() -> String {
        return getPlatformStatus();
    });
    _commands.setFactoryResetCallback([this]() -> bool {
        factoryReset();
        return true;
    });

    _core.events().subscribe([this](const EventData& evt) {
        if (_onEvent) _onEvent(evt);

        if (evt.event == Event::DATA_UPDATED) {
            _cache.saveCache(_controller.getClimateData());
            if (_onDataUpdate) _onDataUpdate(effectiveData());
        }
        if (evt.event == Event::ERROR_OCCURRED || evt.event == Event::REQUEST_FAILED) {
            if (_onError) _onError(_core.errors().getErrorCode(), _core.errors().getErrorMessage());
        }
    });

    _commands.registerAllCommands();
}

void AmelTechClimate::applyConfiguration(const ClimateConfiguration& cfg) {
    if (cfg.ssid.length() > 0) {
        _core.wifi().setWiFiTimeout(cfg.wifiTimeoutMs);
    }
    _scheduler.setUpdateInterval(cfg.updateIntervalMs);

    if (cfg.enablePersistence) {
        _cache.enablePersistence();
    }
}

ClimateData AmelTechClimate::effectiveData() const {
    switch (_mode) {
        case DataSource::LOCAL:
            return _localData;
        case DataSource::HYBRID: {
            ClimateData merged = _controller.getClimateData();
            merged.mergeFrom(_localData);
            return merged;
        }
        case DataSource::REMOTE:
        default:
            return _controller.getClimateData();
    }
}

// ===================== LIFECYCLE =====================

bool AmelTechClimate::begin() {
    return begin("", "");
}

bool AmelTechClimate::begin(const char* ssid, const char* password) {
    wireEventBridge();

    _core.bootStageCore();
    _config.begin();
    _cache.begin();

    bool wifiOk = true;
    if (ssid != nullptr && strlen(ssid) > 0) {
        wifiOk = _core.bootStageWiFi(ssid, password, _core.wifi().getWiFiTimeout());
    }

    _core.bootStageProvider();

    _scheduler.startScheduler();
    _core.bootStageReady();

    if (!wifiOk && ssid != nullptr && strlen(ssid) > 0) {
        _core.errors().setError(ErrorCode::WIFI_TIMEOUT, "Initial WiFi connect did not complete before timeout; will keep retrying in background");
    }

    return _core.state().isReady();
}

bool AmelTechClimate::begin(const ClimateConfiguration& configuration) {
    applyConfiguration(configuration);
    bool ok = begin(configuration.ssid.c_str(), configuration.password.c_str());

    if (configuration.enablePersistence && ok) {
        loadLocationsFromFlash();
        loadURLsFromFlash();
    }

    return ok;
}

void AmelTechClimate::end() {
    _scheduler.stopScheduler();
    _controller.cancelRequest();
    _core.wifi().disconnectWiFi();
    _core.state().setState(SystemState::UNINITIALIZED);
}

void AmelTechClimate::loop() {
    _core.loopMaintenance();
    _controller.update();
    _platforms.update();
    _commands.update();
    handleSerial();
}

void AmelTechClimate::restart() {
    end();
    _core.wifi().connectWiFi();
    begin();
}

void AmelTechClimate::reset() {
    _core.errors().clearError();
    _controller.cancelRequest();
}

void AmelTechClimate::factoryReset() {
    reset();
    _locations.clearLocations();
    _urls.clearURLs();
    _cache.clearCache();
    _config.factoryReset();
}

bool AmelTechClimate::isInitialized() const {
    return _core.state().isInitialized();
}

bool AmelTechClimate::isReady() const {
    return _core.state().isReady();
}

SystemState AmelTechClimate::getState() const {
    return _core.state().getState();
}

String AmelTechClimate::getVersion() const {
    return String(LIBRARY_VERSION);
}

unsigned long AmelTechClimate::getUptime() const {
    return _core.state().getUptime();
}

// ===================== DATA UPDATES =====================

void AmelTechClimate::update() {
    _controller.update();
}

bool AmelTechClimate::updateNow() {
    if (!_locations.hasActiveLocation()) {
        _core.errors().setError(ErrorCode::LOCATION_INVALID, "No active location set - call setLocation() first");
        return false;
    }
    return _controller.updateNow(_locations.getLatitude(), _locations.getLongitude());
}

bool AmelTechClimate::request() {
    return updateNow();
}

void AmelTechClimate::cancelRequest() {
    _controller.cancelRequest();
}

bool AmelTechClimate::isUpdating() const {
    return _controller.isUpdating();
}

const ClimateData& AmelTechClimate::getClimateData() const {
    return _controller.getClimateData();
}

// ===================== CALLBACKS =====================

void AmelTechClimate::onDataUpdate(DataUpdateCallback callback) { _onDataUpdate = callback; }
void AmelTechClimate::onError(ErrorCallback callback) { _onError = callback; }
void AmelTechClimate::onEvent(GeneralEventCallback callback) { _onEvent = callback; }

// ===================== WI-FI =====================

void AmelTechClimate::setWiFi(const char* ssid, const char* password) { _core.wifi().setWiFi(ssid, password); }
void AmelTechClimate::setWiFiCredentials(const char* ssid, const char* password) { _core.wifi().setWiFiCredentials(ssid, password); }
void AmelTechClimate::setSSID(const char* ssid) { _core.wifi().setSSID(ssid); }
void AmelTechClimate::setPassword(const char* password) { _core.wifi().setPassword(password); }
bool AmelTechClimate::connectWiFi() { return _core.wifi().connectWiFi(); }
void AmelTechClimate::disconnectWiFi() { _core.wifi().disconnectWiFi(); }
bool AmelTechClimate::reconnectWiFi() { return _core.wifi().reconnectWiFi(); }
bool AmelTechClimate::isWiFiConnected() const { return _core.wifi().isWiFiConnected(); }
WiFiState AmelTechClimate::getWiFiStatus() const { return _core.wifi().getWiFiStatus(); }
int32_t AmelTechClimate::getRSSI() const { return _core.wifi().getRSSI(); }
String AmelTechClimate::getIP() const { return _core.wifi().getIP(); }
String AmelTechClimate::getGateway() const { return _core.wifi().getGateway(); }
String AmelTechClimate::getSubnet() const { return _core.wifi().getSubnet(); }
String AmelTechClimate::getDNS() const { return _core.wifi().getDNS(); }
void AmelTechClimate::setWiFiTimeout(unsigned long ms) { _core.wifi().setWiFiTimeout(ms); }
unsigned long AmelTechClimate::getWiFiTimeout() const { return _core.wifi().getWiFiTimeout(); }
void AmelTechClimate::setReconnectInterval(unsigned long ms) { _core.wifi().setReconnectInterval(ms); }
unsigned long AmelTechClimate::getReconnectInterval() const { return _core.wifi().getReconnectInterval(); }
void AmelTechClimate::enableAutoReconnect() { _core.wifi().enableAutoReconnect(); }
void AmelTechClimate::disableAutoReconnect() { _core.wifi().disableAutoReconnect(); }

// ===================== PROVIDER =====================

void AmelTechClimate::useOpenMeteo() {
    _core.providers().useOpenMeteo();
}

void AmelTechClimate::useCustomProvider() {
    const URLRecord* active = _urls.getActiveURL();
    if (active == nullptr) {
        _core.errors().setError(ErrorCode::CONFIG_ERROR, "useCustomProvider() called but no custom URL is selected - call addURL() and selectURL() first");
        return;
    }
    _core.providers().custom().setEndpoint(active->url);
    _core.providers().useCustomProvider();
}

Provider* AmelTechClimate::getProvider() { return _core.providers().getProvider(); }
String AmelTechClimate::getProviderName() const { return _core.providers().getProviderName(); }
String AmelTechClimate::getProviderStatus() const { return _core.providers().getProviderStatus(); }
bool AmelTechClimate::isOpenMeteo() const { return _core.providers().isOpenMeteo(); }
String AmelTechClimate::getOpenMeteoConfiguration() const { return _core.providers().getOpenMeteoConfiguration(); }
void AmelTechClimate::setOpenMeteoVariables(const bool enabledFlags[(int)ClimateParameter::PARAMETER_COUNT]) { _core.providers().setOpenMeteoVariables(enabledFlags); }
void AmelTechClimate::resetOpenMeteoVariables() { _core.providers().resetOpenMeteoVariables(); }

// ===================== CLIMATE GETTERS =====================

float AmelTechClimate::getTemperature() {
    ClimateData d = effectiveData();
    if (!d.hasTemperature) { _core.errors().setError(ErrorCode::DATA_UNAVAILABLE, "temperature"); return NAN; }
    return d.temperature;
}
float AmelTechClimate::getRelativeHumidity() {
    ClimateData d = effectiveData();
    if (!d.hasRelativeHumidity) { _core.errors().setError(ErrorCode::DATA_UNAVAILABLE, "relativeHumidity"); return NAN; }
    return d.relativeHumidity;
}
float AmelTechClimate::getDewPoint() {
    ClimateData d = effectiveData();
    if (!d.hasDewPoint) { _core.errors().setError(ErrorCode::DATA_UNAVAILABLE, "dewPoint"); return NAN; }
    return d.dewPoint;
}
float AmelTechClimate::getApparentTemperature() {
    ClimateData d = effectiveData();
    if (!d.hasApparentTemperature) { _core.errors().setError(ErrorCode::DATA_UNAVAILABLE, "apparentTemperature"); return NAN; }
    return d.apparentTemperature;
}
float AmelTechClimate::getAtmosphericPressure() {
    ClimateData d = effectiveData();
    if (!d.hasAtmosphericPressure) { _core.errors().setError(ErrorCode::DATA_UNAVAILABLE, "atmosphericPressure"); return NAN; }
    return d.atmosphericPressure;
}
float AmelTechClimate::getPrecipitation() {
    ClimateData d = effectiveData();
    if (!d.hasPrecipitation) { _core.errors().setError(ErrorCode::DATA_UNAVAILABLE, "precipitation"); return NAN; }
    return d.precipitation;
}
float AmelTechClimate::getRain() {
    ClimateData d = effectiveData();
    if (!d.hasRain) { _core.errors().setError(ErrorCode::DATA_UNAVAILABLE, "rain"); return NAN; }
    return d.rain;
}
float AmelTechClimate::getSnowfall() {
    ClimateData d = effectiveData();
    if (!d.hasSnowfall) { _core.errors().setError(ErrorCode::DATA_UNAVAILABLE, "snowfall"); return NAN; }
    return d.snowfall;
}
int AmelTechClimate::getWeatherCode() {
    ClimateData d = effectiveData();
    if (!d.hasWeatherCode) { _core.errors().setError(ErrorCode::DATA_UNAVAILABLE, "weatherCode"); return -1; }
    return d.weatherCode;
}
String AmelTechClimate::getWeatherCondition() {
    ClimateData d = effectiveData();
    if (!d.hasWeatherCondition) { _core.errors().setError(ErrorCode::DATA_UNAVAILABLE, "weatherCondition"); return "N/A"; }
    return d.weatherCondition;
}
float AmelTechClimate::getCloudCover() {
    ClimateData d = effectiveData();
    if (!d.hasCloudCover) { _core.errors().setError(ErrorCode::DATA_UNAVAILABLE, "cloudCover"); return NAN; }
    return d.cloudCover;
}
float AmelTechClimate::getWindSpeed() {
    ClimateData d = effectiveData();
    if (!d.hasWindSpeed) { _core.errors().setError(ErrorCode::DATA_UNAVAILABLE, "windSpeed"); return NAN; }
    return d.windSpeed;
}
float AmelTechClimate::getWindDirection() {
    ClimateData d = effectiveData();
    if (!d.hasWindDirection) { _core.errors().setError(ErrorCode::DATA_UNAVAILABLE, "windDirection"); return NAN; }
    return d.windDirection;
}
float AmelTechClimate::getWindGusts() {
    ClimateData d = effectiveData();
    if (!d.hasWindGusts) { _core.errors().setError(ErrorCode::DATA_UNAVAILABLE, "windGusts"); return NAN; }
    return d.windGusts;
}
float AmelTechClimate::getSolarRadiation() {
    ClimateData d = effectiveData();
    if (!d.hasSolarRadiation) { _core.errors().setError(ErrorCode::DATA_UNAVAILABLE, "solarRadiation"); return NAN; }
    return d.solarRadiation;
}
float AmelTechClimate::getUVIndex() {
    ClimateData d = effectiveData();
    if (!d.hasUvIndex) { _core.errors().setError(ErrorCode::DATA_UNAVAILABLE, "uvIndex"); return NAN; }
    return d.uvIndex;
}
float AmelTechClimate::getEvapotranspiration() {
    ClimateData d = effectiveData();
    if (!d.hasEvapotranspiration) { _core.errors().setError(ErrorCode::DATA_UNAVAILABLE, "evapotranspiration"); return NAN; }
    return d.evapotranspiration;
}
float AmelTechClimate::getSoilTemperature() {
    ClimateData d = effectiveData();
    if (!d.hasSoilTemperature) { _core.errors().setError(ErrorCode::DATA_UNAVAILABLE, "soilTemperature"); return NAN; }
    return d.soilTemperature;
}
float AmelTechClimate::getSoilMoisture() {
    ClimateData d = effectiveData();
    if (!d.hasSoilMoisture) { _core.errors().setError(ErrorCode::DATA_UNAVAILABLE, "soilMoisture"); return NAN; }
    return d.soilMoisture;
}
float AmelTechClimate::getFreezingLevel() {
    ClimateData d = effectiveData();
    if (!d.hasFreezingLevel) { _core.errors().setError(ErrorCode::DATA_UNAVAILABLE, "freezingLevel"); return NAN; }
    return d.freezingLevel;
}
float AmelTechClimate::getVisibility() {
    ClimateData d = effectiveData();
    if (!d.hasVisibility) { _core.errors().setError(ErrorCode::DATA_UNAVAILABLE, "visibility"); return NAN; }
    return d.visibility;
}
float AmelTechClimate::getCAPE() {
    ClimateData d = effectiveData();
    if (!d.hasCape) { _core.errors().setError(ErrorCode::DATA_UNAVAILABLE, "cape"); return NAN; }
    return d.cape;
}

// ===================== DATA VALIDATION =====================

bool AmelTechClimate::validateClimateData() {
    DataValidator validator;
    ClimateData copy = effectiveData();
    return validator.validateClimateData(copy);
}

bool AmelTechClimate::isDataValid() const {
    return effectiveData().valid;
}

bool AmelTechClimate::isDataStale() const {
    DataValidator validator;
    return validator.isDataStale(effectiveData(), _cache.getCacheLifetime());
}

// ===================== LOCATION =====================

void AmelTechClimate::setLocation(const char* name, float latitude, float longitude) {
    String cleanName = _security.sanitizeInput(String(name));
    if (!_locations.setLocation(cleanName, latitude, longitude)) return;

    const Location* active = _locations.getActiveLocation();
    if (active) {
        _controller.setActiveCoordinates(active->latitude, active->longitude);
    }
}

float AmelTechClimate::getLatitude() const { return _locations.getLatitude(); }
float AmelTechClimate::getLongitude() const { return _locations.getLongitude(); }
String AmelTechClimate::getLocationName() const { return _locations.getLocationName(); }

bool AmelTechClimate::addLocation(const char* name, float latitude, float longitude) {
    String cleanName = _security.sanitizeInput(String(name));
    return _locations.addLocation(cleanName, latitude, longitude) >= 0;
}

bool AmelTechClimate::editLocation(int id, const char* name, float latitude, float longitude) {
    String cleanName = _security.sanitizeInput(String(name));
    return _locations.editLocation(id, cleanName, latitude, longitude);
}

bool AmelTechClimate::deleteLocation(int id) {
    return _locations.deleteLocation(id);
}

bool AmelTechClimate::selectLocation(int id) {
    bool ok = _locations.selectLocation(id);
    if (ok) {
        const Location* active = _locations.getActiveLocation();
        if (active) _controller.setActiveCoordinates(active->latitude, active->longitude);
    }
    return ok;
}

int AmelTechClimate::getLocationCount() const {
    return _locations.getLocationCount();
}

bool AmelTechClimate::saveLocationsToFlash() {
    Location buffer[LocationManager::MAX_LOCATIONS];
    uint8_t n = _locations.listLocations(buffer, LocationManager::MAX_LOCATIONS);
    const Location* active = _locations.getActiveLocation();
    int activeId = active ? active->id : -1;
    return _config.saveLocations(buffer, n, activeId);
}

bool AmelTechClimate::loadLocationsFromFlash() {
    Location buffer[LocationManager::MAX_LOCATIONS];
    int activeId = -1;
    uint8_t n = _config.loadLocations(buffer, LocationManager::MAX_LOCATIONS, activeId);

    if (n == 0) return false;

    _locations.clearLocations();
    for (uint8_t i = 0; i < n; i++) {
        _locations.addLocation(buffer[i].name, buffer[i].latitude, buffer[i].longitude);
    }
    if (activeId >= 0) {
        Location restored[LocationManager::MAX_LOCATIONS];
        uint8_t restoredCount = _locations.listLocations(restored, LocationManager::MAX_LOCATIONS);
        if (restoredCount > 0) {
            selectLocation(restored[0].id);
        }
    }
    return true;
}

// ===================== CUSTOM URLS / TX / RX =====================

bool AmelTechClimate::addURL(const char* name, const char* url, URLDirection direction, URLMethod method, URLFormat format) {
    String cleanName = _security.sanitizeInput(String(name));
    return _urls.addURL(cleanName, String(url), direction, method, format) >= 0;
}

bool AmelTechClimate::editURL(int id, const char* name, const char* url) {
    String cleanName = _security.sanitizeInput(String(name));
    return _urls.editURL(id, cleanName, String(url));
}

bool AmelTechClimate::deleteURL(int id) {
    return _urls.deleteURL(id);
}

bool AmelTechClimate::selectURL(int id) {
    return _urls.selectURL(id);
}

int AmelTechClimate::getURLCount() const {
    return _urls.getURLCount();
}

String AmelTechClimate::getActiveURLName() const {
    const URLRecord* active = _urls.getActiveURL();
    return active ? active->name : "";
}

bool AmelTechClimate::saveURLsToFlash() {
    URLRecord buffer[URLManager::MAX_URLS];
    uint8_t n = _urls.listURLs(buffer, URLManager::MAX_URLS);
    const URLRecord* active = _urls.getActiveURL();
    int activeId = active ? active->id : -1;
    return _config.saveURLs(buffer, n, activeId);
}

bool AmelTechClimate::loadURLsFromFlash() {
    URLRecord buffer[URLManager::MAX_URLS];
    int activeId = -1;
    uint8_t n = _config.loadURLs(buffer, URLManager::MAX_URLS, activeId);

    if (n == 0) return false;

    _urls.clearURLs();
    for (uint8_t i = 0; i < n; i++) {
        _urls.addURL(buffer[i].name, buffer[i].url, buffer[i].direction, buffer[i].method, buffer[i].format);
    }
    return true;
}

bool AmelTechClimate::sendClimateDataTX() {
    return _tx.sendClimateData(effectiveData());
}

bool AmelTechClimate::sendJSON(const char* json) {
    return _tx.sendJSON(String(json));
}

void AmelTechClimate::setTXURL(const char* url) { _tx.setTXURL(String(url)); }
void AmelTechClimate::enableTX() { _tx.enableTX(); }
void AmelTechClimate::disableTX() { _tx.disableTX(); }

bool AmelTechClimate::receiveRX() { return _rx.receive(); }
bool AmelTechClimate::hasReceivedData() const { return _rx.hasReceivedData(); }
String AmelTechClimate::readReceivedData() const { return _rx.readReceivedData(); }
void AmelTechClimate::setRXURL(const char* url) { _rx.setRXURL(String(url)); }
void AmelTechClimate::enableRX() { _rx.enableRX(); }
void AmelTechClimate::disableRX() { _rx.disableRX(); }

// ===================== SENSORS =====================

int8_t AmelTechClimate::registerSensor(Sensor* sensor) {
    return _sensors.registerSensor(sensor);
}

uint8_t AmelTechClimate::scanSensors() {
    return _sensors.scanSensors();
}

uint8_t AmelTechClimate::getSensorCount() const {
    return _sensors.getSensorCount();
}

bool AmelTechClimate::readAllSensorsIntoLocalData() {
    uint8_t successCount = _sensors.readAllSensors(_localData);
    return successCount > 0;
}

// ===================== HYBRID MODE =====================

void AmelTechClimate::setMode(DataSource mode) {
    if (mode == DataSource::REMOTE || mode == DataSource::LOCAL || mode == DataSource::HYBRID) {
        _mode = mode;
    } else {
        _mode = DataSource::REMOTE;
    }
}

DataSource AmelTechClimate::getMode() const { return _mode; }
void AmelTechClimate::enableHybrid() { _mode = DataSource::HYBRID; }
void AmelTechClimate::disableHybrid() { _mode = DataSource::REMOTE; }
void AmelTechClimate::useRemoteData() { _mode = DataSource::REMOTE; }
void AmelTechClimate::useLocalData() { _mode = DataSource::LOCAL; }

// ===================== CACHE =====================

void AmelTechClimate::enableCache() { _cache.enableCache(); }
void AmelTechClimate::disableCache() { _cache.disableCache(); }
void AmelTechClimate::setCacheLifetime(unsigned long ms) { _cache.setCacheLifetime(ms); }
unsigned long AmelTechClimate::getCacheLifetime() const { return _cache.getCacheLifetime(); }
void AmelTechClimate::clearCache() { _cache.clearCache(); }
bool AmelTechClimate::isCacheValid() const { return _cache.isCacheValid(); }
unsigned long AmelTechClimate::getCacheAge() const { return _cache.getCacheAge(); }
void AmelTechClimate::forceRefresh() { _cache.forceRefresh(); }

// ===================== PLATFORMS =====================

void AmelTechClimate::setRESTEndpoint(const char* url) { _platforms.rest().setEndpoint(String(url)); }
void AmelTechClimate::enableRESTPlatform() { _platforms.enablePlatform(PlatformType::REST); }
void AmelTechClimate::disableRESTPlatform() { _platforms.disablePlatform(PlatformType::REST); }

void AmelTechClimate::setWebhookURL(const char* url) { _platforms.webhook().setURL(String(url)); }
void AmelTechClimate::enableWebhookPlatform() { _platforms.enablePlatform(PlatformType::WEBHOOK); }
void AmelTechClimate::disableWebhookPlatform() { _platforms.disablePlatform(PlatformType::WEBHOOK); }
bool AmelTechClimate::triggerWebhook() { return _platforms.webhook().trigger(); }

void AmelTechClimate::attachMQTTAdapter(PlatformAdapter* mqttAdapter) { _platforms.attachMQTT(mqttAdapter); }
void AmelTechClimate::attachBlynkAdapter(PlatformAdapter* blynkAdapter) { _platforms.attachBlynk(blynkAdapter); }
void AmelTechClimate::enablePlatform(PlatformType type) { _platforms.enablePlatform(type); }
void AmelTechClimate::disablePlatform(PlatformType type) { _platforms.disablePlatform(type); }

uint8_t AmelTechClimate::publishToAllPlatforms() {
    return _platforms.publishClimateData(effectiveData());
}

String AmelTechClimate::getPlatformStatus() const {
    return _platforms.getPlatformStatus();
}

// ===================== SECURITY =====================

void AmelTechClimate::setCredential(const char* name, const char* value) {
    _security.setCredential(String(name), String(value));
}

String AmelTechClimate::getMaskedCredential(const char* name) const {
    return _security.maskCredential(String(name));
}

// ===================== ERROR HANDLING =====================

bool AmelTechClimate::hasError() const { return _core.errors().hasError(); }
ErrorCode AmelTechClimate::getLastError() const { return _core.errors().getErrorCode(); }
ErrorCode AmelTechClimate::getErrorCode() const { return _core.errors().getErrorCode(); }
String AmelTechClimate::getErrorMessage() const { return _core.errors().getErrorMessage(); }
void AmelTechClimate::clearError() { _core.errors().clearError(); }

// ===================== SERIAL COMMAND INTERFACE =====================

void AmelTechClimate::handleSerialLine(const String& line) {
    String output = _commands.dispatch(line);
    if (output.length() > 0) {
        Serial.println(output);
    }
}

void AmelTechClimate::handleSerial() {
    while (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '\n' || c == '\r') {
            if (_serialBuffer.length() > 0) {
                handleSerialLine(_serialBuffer);
                _serialBuffer = "";
            }
        } else {
            if (_serialBuffer.length() < 256) {
                _serialBuffer += c;
            }
        }
    }
}

} // namespace AmelTech
