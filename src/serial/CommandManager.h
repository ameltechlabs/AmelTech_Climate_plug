/*
 * CommandManager.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED (P1 command set; PENDING commands registered
 * but not yet functional - see registerAllCommands() in the .cpp for
 * the authoritative list of what's wired vs. pending)
 *
 * The single entry point AmelTechClimate calls for Serial command
 * handling. Owns the CommandParser, CommandRegistry, HelpSystem, and
 * LiveStatus, and registers every command name from the spec's
 * "SERIAL COMMANDS" list - implemented ones get real handlers,
 * not-yet-built ones are registered as PENDING so `help` and
 * unrecognized-command handling both stay honest.
 *
 * DESIGN NOTE: rather than holding a pointer back to AmelTechClimate
 * (which would create a circular include, since AmelTech_Climate_plug.h
 * already includes this header), commands that need live data or need
 * to trigger an action are wired through small std::function callbacks
 * set via the setXCallback() methods below. AmelTechClimate wires
 * these once in its constructor/begin().
 */

#ifndef AMELTECH_COMMAND_MANAGER_H
#define AMELTECH_COMMAND_MANAGER_H

#include <Arduino.h>
#include <functional>
#include "CommandParser.h"
#include "CommandRegistry.h"
#include "HelpSystem.h"
#include "LiveStatus.h"
#include "../data/ClimateData.h"
#include "../core/ClimateEvents.h"

namespace AmelTech {

// Supplies the current ClimateData + location/provider context for
// display commands (status, live status).
typedef std::function<const ClimateData&()> DataProviderCallback;
typedef std::function<String()> StringProviderCallback;
typedef std::function<float()> FloatProviderCallback;
typedef std::function<int()> IntProviderCallback;

// Triggers an action; returns true if it was accepted/started.
typedef std::function<bool()> ActionCallback;

// Location commands: name/lat/lon parsed from raw serial text by
// CommandManager, then handed to AmelTechClimate via these.
typedef std::function<bool(const String& name, float lat, float lon)> LocationSetCallback;
typedef std::function<bool(int id)> IdActionCallback;
typedef std::function<String()> ListCallback; // returns pre-formatted listing text (location list, url list, sensor status, etc.)

// URL commands: name/url parsed from raw serial text.
typedef std::function<bool(const String& name, const String& url)> URLSetCallback;

class CommandManager {
public:
    CommandManager();

    void attachEvents(ClimateEvents* events);

    void setDataCallback(DataProviderCallback cb);
    void setLocationNameCallback(StringProviderCallback cb);
    void setLatitudeCallback(FloatProviderCallback cb);
    void setLongitudeCallback(FloatProviderCallback cb);
    void setProviderNameCallback(StringProviderCallback cb);
    void setUpdateNowCallback(ActionCallback cb);
    void setStaleLifetimeMs(unsigned long ms);

    // Location sub-commands
    void setLocationAddCallback(LocationSetCallback cb);
    void setLocationSelectCallback(IdActionCallback cb);
    void setLocationDeleteCallback(IdActionCallback cb);
    void setLocationListCallback(ListCallback cb);

    // URL sub-commands
    void setURLAddCallback(URLSetCallback cb);
    void setURLSelectCallback(IdActionCallback cb);
    void setURLDeleteCallback(IdActionCallback cb);
    void setURLListCallback(ListCallback cb);
    void setURLTestCallback(IdActionCallback cb);

    // Sensor sub-commands
    void setSensorScanCallback(ActionCallback cb);
    void setSensorStatusCallback(ListCallback cb);

    // Cache sub-commands
    void setCacheStatusCallback(ListCallback cb);
    void setCacheClearCallback(ActionCallback cb);

    // Diagnostics/status sub-commands
    void setNetworkStatusCallback(ListCallback cb);
    void setProviderStatusCallback(ListCallback cb);
    void setPlatformStatusCallback(ListCallback cb);
    void setFactoryResetCallback(ActionCallback cb);

    // Registers every command from the spec, real handlers for P1,
    // PENDING markers for the rest. Call once during begin().
    void registerAllCommands();

    // Parses and dispatches one line, returning the text that should
    // be printed to Serial (the caller does the actual Serial.print).
    String dispatch(const String& line);

    LiveStatus& liveStatus() { return _liveStatus; }

    // Called every loop() so continuous "live status" can refresh on
    // its own timer independent of any command being typed.
    void update();

private:
    CommandParser _parser;
    CommandRegistry _registry;
    HelpSystem _help;
    LiveStatus _liveStatus;
    ClimateEvents* _events;

    DataProviderCallback _getData;
    StringProviderCallback _getLocationName;
    FloatProviderCallback _getLatitude;
    FloatProviderCallback _getLongitude;
    StringProviderCallback _getProviderName;
    ActionCallback _updateNow;
    unsigned long _staleLifetimeMs;

    LocationSetCallback _locationAdd;
    IdActionCallback _locationSelect;
    IdActionCallback _locationDelete;
    ListCallback _locationList;

    URLSetCallback _urlAdd;
    IdActionCallback _urlSelect;
    IdActionCallback _urlDelete;
    ListCallback _urlList;
    IdActionCallback _urlTest;

    ActionCallback _sensorScan;
    ListCallback _sensorStatus;

    ListCallback _cacheStatus;
    ActionCallback _cacheClear;

    ListCallback _networkStatus;
    ListCallback _providerStatus;
    ListCallback _platformStatus;
    ActionCallback _factoryReset;

    static String pendingMessage(const String& name);

    // Parses "<command prefix> <name> : <value> / <name> : <value> ..."
    // style multi-field input the spec's interactive commands use
    // (e.g. "Location name : X\nLatitude : Y\nLongitude : Z" entered
    // as a single line separated by '/' since Serial input here is
    // one line at a time - see docs/Commands.md for the exact
    // supported syntax). Returns false if required fields are missing.
    bool parseNameLatLon(const String& raw, const String& commandPrefix, String& nameOut, float& latOut, float& lonOut) const;
    bool parseNameURL(const String& raw, const String& commandPrefix, String& nameOut, String& urlOut) const;
    bool parseTrailingId(const String& raw, const String& commandPrefix, int& idOut) const;

    // Empty-context ClimateData returned when no callback is wired
    // yet (e.g. dispatch() called before begin() finishes wiring).
    ClimateData _emptyData;
};

} // namespace AmelTech

#endif // AMELTECH_COMMAND_MANAGER_H
