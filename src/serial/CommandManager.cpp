/*
 * CommandManager.cpp
 * AmelTech_Climate_plug
 * Full spec command set - all commands with a real backing manager
 * now dispatch to it via callback; only commands with no
 * corresponding wired feature remain PENDING (see registerAllCommands).
 */

#include "CommandManager.h"

namespace AmelTech {

CommandManager::CommandManager()
    : _events(nullptr), _staleLifetimeMs(1800000) {
}

void CommandManager::attachEvents(ClimateEvents* events) {
    _events = events;
    _liveStatus.attachEvents(events);
}

void CommandManager::setDataCallback(DataProviderCallback cb) { _getData = cb; }
void CommandManager::setLocationNameCallback(StringProviderCallback cb) { _getLocationName = cb; }
void CommandManager::setLatitudeCallback(FloatProviderCallback cb) { _getLatitude = cb; }
void CommandManager::setLongitudeCallback(FloatProviderCallback cb) { _getLongitude = cb; }
void CommandManager::setProviderNameCallback(StringProviderCallback cb) { _getProviderName = cb; }
void CommandManager::setUpdateNowCallback(ActionCallback cb) { _updateNow = cb; }
void CommandManager::setStaleLifetimeMs(unsigned long ms) { _staleLifetimeMs = ms; }

void CommandManager::setLocationAddCallback(LocationSetCallback cb) { _locationAdd = cb; }
void CommandManager::setLocationSelectCallback(IdActionCallback cb) { _locationSelect = cb; }
void CommandManager::setLocationDeleteCallback(IdActionCallback cb) { _locationDelete = cb; }
void CommandManager::setLocationListCallback(ListCallback cb) { _locationList = cb; }

void CommandManager::setURLAddCallback(URLSetCallback cb) { _urlAdd = cb; }
void CommandManager::setURLSelectCallback(IdActionCallback cb) { _urlSelect = cb; }
void CommandManager::setURLDeleteCallback(IdActionCallback cb) { _urlDelete = cb; }
void CommandManager::setURLListCallback(ListCallback cb) { _urlList = cb; }
void CommandManager::setURLTestCallback(IdActionCallback cb) { _urlTest = cb; }

void CommandManager::setSensorScanCallback(ActionCallback cb) { _sensorScan = cb; }
void CommandManager::setSensorStatusCallback(ListCallback cb) { _sensorStatus = cb; }

void CommandManager::setCacheStatusCallback(ListCallback cb) { _cacheStatus = cb; }
void CommandManager::setCacheClearCallback(ActionCallback cb) { _cacheClear = cb; }

void CommandManager::setNetworkStatusCallback(ListCallback cb) { _networkStatus = cb; }
void CommandManager::setProviderStatusCallback(ListCallback cb) { _providerStatus = cb; }
void CommandManager::setPlatformStatusCallback(ListCallback cb) { _platformStatus = cb; }
void CommandManager::setFactoryResetCallback(ActionCallback cb) { _factoryReset = cb; }

String CommandManager::pendingMessage(const String& name) {
    return "\"" + name + "\" is part of the AmelTech_Climate_plug API but is not yet implemented in this build.\n"
           "See PENDING_MODULES.md in the repository for the implementation roadmap.";
}

// Parses input of the form:
//   <prefix> <name> / <lat> / <lon>
// e.g. "location add Kerala, Thrissur / 10.5276 / 76.2144"
// This is the single-line-friendly equivalent of the spec's
// multi-prompt interactive flow, since Serial input here arrives one
// line at a time rather than through a stateful multi-turn prompt.
bool CommandManager::parseNameLatLon(const String& raw, const String& commandPrefix, String& nameOut, float& latOut, float& lonOut) const {
    if (!raw.startsWith(commandPrefix)) return false;
    String rest = raw.substring(commandPrefix.length());
    rest.trim();
    if (rest.length() == 0) return false;

    int firstSlash = rest.indexOf('/');
    int secondSlash = (firstSlash == -1) ? -1 : rest.indexOf('/', firstSlash + 1);
    if (firstSlash == -1 || secondSlash == -1) return false;

    nameOut = rest.substring(0, firstSlash);
    nameOut.trim();
    String latStr = rest.substring(firstSlash + 1, secondSlash);
    latStr.trim();
    String lonStr = rest.substring(secondSlash + 1);
    lonStr.trim();

    if (nameOut.length() == 0 || latStr.length() == 0 || lonStr.length() == 0) return false;

    latOut = latStr.toFloat();
    lonOut = lonStr.toFloat();
    return true;
}

// Parses "<prefix> <name> / <url>", e.g. "URL link add website / https://example.com/api"
bool CommandManager::parseNameURL(const String& raw, const String& commandPrefix, String& nameOut, String& urlOut) const {
    if (!raw.startsWith(commandPrefix)) return false;
    String rest = raw.substring(commandPrefix.length());
    rest.trim();
    if (rest.length() == 0) return false;

    int slash = rest.indexOf('/');
    if (slash == -1) return false;

    nameOut = rest.substring(0, slash);
    nameOut.trim();
    urlOut = rest.substring(slash + 1);
    urlOut.trim();

    return nameOut.length() > 0 && urlOut.length() > 0;
}

// Parses a trailing integer id, e.g. "location select 3" -> 3
bool CommandManager::parseTrailingId(const String& raw, const String& commandPrefix, int& idOut) const {
    if (!raw.startsWith(commandPrefix)) return false;
    String rest = raw.substring(commandPrefix.length());
    rest.trim();
    if (rest.length() == 0) return false;

    idOut = rest.toInt();
    // toInt() returns 0 both for a genuine "0" and for unparseable
    // input; disambiguate by checking the first character is a digit
    // or sign, so "location select abc" is correctly rejected rather
    // than silently treated as id 0.
    char first = rest[0];
    if (!isdigit(first) && first != '-') return false;
    return true;
}

void CommandManager::registerAllCommands() {
    // ---------------- IMPLEMENTED: core display/update ----------------

    _registry.registerCommand("help", "Show this command reference",
        [this](const String&, const String&) -> String {
            return _help.render(_registry);
        });

    _registry.registerCommand("status", "Print current climate status once",
        [this](const String&, const String&) -> String {
            const ClimateData& data = _getData ? _getData() : _emptyData;
            String loc = _getLocationName ? _getLocationName() : "N/A";
            float lat = _getLatitude ? _getLatitude() : 0.0f;
            float lon = _getLongitude ? _getLongitude() : 0.0f;
            String provider = _getProviderName ? _getProviderName() : "N/A";
            SerialDisplay display;
            return display.renderStatus(data, loc, lat, lon, provider, _liveStatus.isLiveStatusRunning(), _staleLifetimeMs);
        });

    _registry.registerCommand("update", "Force an immediate climate data update",
        [this](const String&, const String&) -> String {
            if (!_updateNow) return "Update mechanism not yet wired.";
            bool started = _updateNow();
            return started ? "Update started. Data will refresh shortly (non-blocking)." : "Update could not be started (already in progress, WiFi not connected, or no active location set).";
        });

    _registry.registerCommand("live status stop", "Stop the continuous live status display",
        [this](const String&, const String&) -> String {
            _liveStatus.stopLiveStatus();
            return "Live status stopped. WiFi, scheduler, and cached data remain active.";
        });

    _registry.registerCommand("live status", "Start continuous live climate display",
        [this](const String&, const String&) -> String {
            _liveStatus.startLiveStatus();
            const ClimateData& data = _getData ? _getData() : _emptyData;
            String loc = _getLocationName ? _getLocationName() : "N/A";
            float lat = _getLatitude ? _getLatitude() : 0.0f;
            float lon = _getLongitude ? _getLongitude() : 0.0f;
            String provider = _getProviderName ? _getProviderName() : "N/A";
            SerialDisplay display;
            return "Live status started (refreshes every " + String(_liveStatus.getLiveStatusInterval() / 1000) + "s). Type 'live status stop' to stop.\n\n" +
                   display.renderStatus(data, loc, lat, lon, provider, true, _staleLifetimeMs);
        });

    // ---------------- IMPLEMENTED: location ----------------

    _registry.registerCommand("location add", "Add a saved location: location add <name> / <lat> / <lon>",
        [this](const String&, const String& raw) -> String {
            if (!_locationAdd) return "Location system not yet wired.";
            String name; float lat, lon;
            if (!parseNameLatLon(raw, "location add", name, lat, lon)) {
                return "Usage: location add <name> / <latitude> / <longitude>\nExample: location add Kerala, Thrissur / 10.5276 / 76.2144";
            }
            bool ok = _locationAdd(name, lat, lon);
            return ok ? ("Location added: " + name) : "Failed to add location (invalid coordinates, name, or storage full).";
        });

    _registry.registerCommand("location select", "Select a saved location as active: location select <id>",
        [this](const String&, const String& raw) -> String {
            if (!_locationSelect) return "Location system not yet wired.";
            int id;
            if (!parseTrailingId(raw, "location select", id)) return "Usage: location select <id>";
            bool ok = _locationSelect(id);
            return ok ? ("Location " + String(id) + " is now active.") : ("No location with id " + String(id) + " found.");
        });

    _registry.registerCommand("location delete", "Delete a saved location: location delete <id>",
        [this](const String&, const String& raw) -> String {
            if (!_locationDelete) return "Location system not yet wired.";
            int id;
            if (!parseTrailingId(raw, "location delete", id)) return "Usage: location delete <id>";
            bool ok = _locationDelete(id);
            return ok ? ("Location " + String(id) + " deleted.") : ("No location with id " + String(id) + " found.");
        });

    _registry.registerCommand("location list", "List all saved locations",
        [this](const String&, const String&) -> String {
            if (!_locationList) return "Location system not yet wired.";
            String listing = _locationList();
            return listing.length() > 0 ? listing : "No saved locations yet. Use 'location add' or 'location change' to add one.";
        });

    _registry.registerCommand("location show", "Show the active location's details",
        [this](const String&, const String&) -> String {
            String loc = _getLocationName ? _getLocationName() : "";
            if (loc.length() == 0) return "No active location set.";
            float lat = _getLatitude ? _getLatitude() : 0.0f;
            float lon = _getLongitude ? _getLongitude() : 0.0f;
            return "Active location: " + loc + "\nLatitude  : " + String(lat, 6) + "\nLongitude : " + String(lon, 6);
        });

    _registry.registerCommand("location change", "Change the active location: location change <name> / <lat> / <lon>",
        [this](const String&, const String& raw) -> String {
            if (!_locationAdd || !_locationSelect) return "Location system not yet wired.";
            String name; float lat, lon;
            if (!parseNameLatLon(raw, "location change", name, lat, lon)) {
                return "Which location change?\n\nLocation name : <name>\nLatitude      : <value>\nLongitude     : <value>\n\nEnter as one line: location change <name> / <latitude> / <longitude>\nExample: location change Kerala, Thrissur / 10.5276 / 76.2144";
            }
            bool added = _locationAdd(name, lat, lon);
            if (!added) return "Failed to change location (invalid coordinates or name).";
            return "Location changed to: " + name + " (" + String(lat, 6) + ", " + String(lon, 6) + ")";
        });

    // ---------------- IMPLEMENTED: custom URLs ----------------

    _registry.registerCommand("URL link add", "Add a custom URL: URL link add <name> / <url>",
        [this](const String&, const String& raw) -> String {
            if (!_urlAdd) return "URL system not yet wired.";
            String name, url;
            if (!parseNameURL(raw, "URL link add", name, url)) {
                return "Usage: URL link add <name> / <url>\nExample: URL link add website / https://example.com/api";
            }
            bool ok = _urlAdd(name, url);
            return ok ? ("URL added: " + name) : "Failed to add URL (invalid URL, reserved name, or storage full).";
        });

    _registry.registerCommand("URL link select", "Select the active custom URL: URL link select <id>",
        [this](const String&, const String& raw) -> String {
            if (!_urlSelect) return "URL system not yet wired.";
            int id;
            if (!parseTrailingId(raw, "URL link select", id)) return "Usage: URL link select <id>";
            bool ok = _urlSelect(id);
            return ok ? ("URL " + String(id) + " is now active.") : ("No URL with id " + String(id) + " found.");
        });

    _registry.registerCommand("URL link delete", "Delete a custom URL: URL link delete <id>",
        [this](const String&, const String& raw) -> String {
            if (!_urlDelete) return "URL system not yet wired.";
            int id;
            if (!parseTrailingId(raw, "URL link delete", id)) return "Usage: URL link delete <id>";
            bool ok = _urlDelete(id);
            return ok ? ("URL " + String(id) + " deleted.") : ("No URL with id " + String(id) + " found.");
        });

    _registry.registerCommand("URL link list", "List all custom URLs",
        [this](const String&, const String&) -> String {
            if (!_urlList) return "URL system not yet wired.";
            String listing = _urlList();
            return listing.length() > 0 ? listing : "No custom URLs yet. Use 'URL link add' to add one.";
        });

    _registry.registerCommand("URL link test", "Test a custom URL: URL link test <id>",
        [this](const String&, const String& raw) -> String {
            if (!_urlTest) return "URL system not yet wired.";
            int id;
            if (!parseTrailingId(raw, "URL link test", id)) return "Usage: URL link test <id>";
            bool ok = _urlTest(id);
            return ok ? ("URL " + String(id) + " passed structural validation.") : ("URL " + String(id) + " failed validation or was not found.");
        });

    _registry.registerCommand("URL link change", "Add or change a custom URL: URL link change <name> / <url>",
        [this](const String&, const String& raw) -> String {
            if (!_urlAdd) return "URL system not yet wired.";
            String name, url;
            if (!parseNameURL(raw, "URL link change", name, url)) {
                return "Which URL link change?\n\nURL link name : <name>\nURL link      : <url>\n\nEnter as one line: URL link change <name> / <url>\nExample: URL link change website / https://example.com/api";
            }
            bool ok = _urlAdd(name, url);
            if (!ok) return "Failed to change URL (invalid URL, or this name is reserved for the protected Open-Meteo provider).";
            return "URL link changed: " + name + " -> " + url;
        });

    // ---------------- IMPLEMENTED: sensors ----------------

    _registry.registerCommand("sensor scan", "Scan for connected local sensors",
        [this](const String&, const String&) -> String {
            if (!_sensorScan) return "Sensor system not yet wired.";
            bool anyFound = _sensorScan();
            return anyFound ? "Sensor scan complete - see 'sensor status' for details." : "Sensor scan complete - no sensors responded. Check wiring/registration.";
        });

    _registry.registerCommand("sensor status", "Show status of registered sensors",
        [this](const String&, const String&) -> String {
            if (!_sensorStatus) return "Sensor system not yet wired.";
            String status = _sensorStatus();
            return status.length() > 0 ? status : "No sensors registered. Use registerSensor() in your sketch to add one.";
        });

    _registry.registerPending("sensor read", "Take an immediate sensor reading");

    // ---------------- IMPLEMENTED: cache ----------------

    _registry.registerCommand("cache status", "Show cache state and age",
        [this](const String&, const String&) -> String {
            if (!_cacheStatus) return "Cache system not yet wired.";
            return _cacheStatus();
        });

    _registry.registerCommand("cache clear", "Clear the climate data cache",
        [this](const String&, const String&) -> String {
            if (!_cacheClear) return "Cache system not yet wired.";
            _cacheClear();
            return "Cache cleared.";
        });

    // ---------------- IMPLEMENTED: diagnostics/status ----------------

    _registry.registerCommand("network status", "Show detailed network diagnostics",
        [this](const String&, const String&) -> String {
            if (!_networkStatus) return "Network status not yet wired.";
            return _networkStatus();
        });

    _registry.registerCommand("provider status", "Show detailed provider diagnostics",
        [this](const String&, const String&) -> String {
            if (!_providerStatus) return "Provider status not yet wired.";
            return _providerStatus();
        });

    _registry.registerCommand("platform status", "Show connected platform integrations",
        [this](const String&, const String&) -> String {
            if (!_platformStatus) return "Platform status not yet wired.";
            return _platformStatus();
        });

    _registry.registerPending("diagnostics", "Run a full system health check");
    _registry.registerPending("statistics", "Show request/cache/sensor statistics");

    _registry.registerCommand("factory reset", "Erase all persisted configuration",
        [this](const String&, const String&) -> String {
            if (!_factoryReset) return "Factory reset not yet wired.";
            _factoryReset();
            return "Factory reset complete. In-memory state and flash-persisted configuration have been cleared.";
        });
}

String CommandManager::dispatch(const String& line) {
    ParsedCommand parsed = _parser.parse(line);
    if (parsed.normalized.length() == 0) return "";

    const CommandEntry* match = _registry.findMatch(parsed.normalized);
    if (match == nullptr) {
        return "Unrecognized command: \"" + parsed.raw + "\". Type 'help' for a list of commands.";
    }

    if (match->status == CommandStatus::PENDING) {
        return pendingMessage(match->name);
    }

    return match->handler(parsed.normalized, parsed.raw);
}

void CommandManager::update() {
    if (!_liveStatus.isLiveStatusRunning()) return;

    const ClimateData& data = _getData ? _getData() : _emptyData;
    String loc = _getLocationName ? _getLocationName() : "N/A";
    float lat = _getLatitude ? _getLatitude() : 0.0f;
    float lon = _getLongitude ? _getLongitude() : 0.0f;
    String provider = _getProviderName ? _getProviderName() : "N/A";

    _liveStatus.refreshLiveStatus(data, loc, lat, lon, provider, _staleLifetimeMs);
}

} // namespace AmelTech
