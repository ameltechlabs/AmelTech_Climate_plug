/*
 * ClimateCore.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED (WiFi/Provider stages real; Storage/Sensor/
 * Platform stages call into Phase 2+ modules and are marked PENDING
 * there - see those modules' headers)
 *
 * Sequences the boot flow from the spec's "MASTER EXECUTION FLOW":
 *   CORE INIT -> STORAGE INIT -> CONFIG LOAD -> SERIAL INIT ->
 *   WIFI INIT -> SENSOR INIT -> PROVIDER INIT -> CACHE INIT ->
 *   PLATFORM INIT -> READY
 *
 * Each stage is a discrete, individually-callable step so
 * AmelTechClimate::begin() can run them in order and fail loudly if
 * a required stage (WiFi) doesn't complete, while optional stages
 * (sensors, platforms) degrade gracefully rather than blocking boot.
 */

#ifndef AMELTECH_CLIMATE_CORE_H
#define AMELTECH_CLIMATE_CORE_H

#include <Arduino.h>
#include "ClimateState.h"
#include "ClimateEvents.h"
#include "../network/WiFiManager.h"
#include "../network/HTTPManager.h"
#include "../network/TLSManager.h"
#include "../network/ConnectionManager.h"
#include "../providers/ProviderManager.h"
#include "../diagnostics/ErrorManager.h"

namespace AmelTech {

class ClimateCore {
public:
    ClimateCore();

    // Wires every owned subsystem's cross-references together
    // (error manager, event bus). Must be called before any boot
    // stage below.
    void wireSubsystems();

    // --- individual boot stages, callable independently for testing ---
    void bootStageCore();     // marks state INITIALIZING, resets error manager
    bool bootStageWiFi(const char* ssid, const char* password, unsigned long timeoutMs);
    void bootStageProvider(); // resets provider manager to Open-Meteo default
    void bootStageReady();    // marks state READY, emits SYSTEM_READY

    // Runs core-owned subsystems' per-loop maintenance (currently:
    // WiFiManager::update()). Sensor/storage/platform per-loop work
    // is driven by AmelTechClimate directly once those modules exist.
    void loopMaintenance();

    ClimateState& state() { return _state; }
    const ClimateState& state() const { return _state; }
    ClimateEvents& events() { return _events; }
    const ClimateEvents& events() const { return _events; }
    ErrorManager& errors() { return _errorManager; }
    const ErrorManager& errors() const { return _errorManager; }
    WiFiManager& wifi() { return _wifi; }
    const WiFiManager& wifi() const { return _wifi; }
    HTTPManager& http() { return _http; }
    const HTTPManager& http() const { return _http; }
    TLSManager& tls() { return _tls; }
    const TLSManager& tls() const { return _tls; }
    ConnectionManager& connection() { return _connection; }
    const ConnectionManager& connection() const { return _connection; }
    ProviderManager& providers() { return _providers; }
    const ProviderManager& providers() const { return _providers; }

private:
    ClimateState _state;
    ClimateEvents _events;
    ErrorManager _errorManager;

    WiFiManager _wifi;
    HTTPManager _http;
    TLSManager _tls;
    ConnectionManager _connection;

    ProviderManager _providers;
};

} // namespace AmelTech

#endif // AMELTECH_CLIMATE_CORE_H
