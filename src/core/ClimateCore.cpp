/*
 * ClimateCore.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "ClimateCore.h"

namespace AmelTech {

ClimateCore::ClimateCore() {
}

void ClimateCore::wireSubsystems() {
    _wifi.attachErrorManager(&_errorManager);
    _wifi.attachEvents(&_events);

    _http.attachErrorManager(&_errorManager);
    _tls.attachErrorManager(&_errorManager);

    _connection.attach(&_wifi, &_http, &_tls);
}

void ClimateCore::bootStageCore() {
    _state.setState(SystemState::INITIALIZING);
    _errorManager.clearError();
    _events.emit(Event::SYSTEM_STARTED);
}

bool ClimateCore::bootStageWiFi(const char* ssid, const char* password, unsigned long timeoutMs) {
    if (ssid == nullptr || strlen(ssid) == 0) {
        // No credentials given - not necessarily an error (user may
        // configure WiFi later via setWiFi()), just skip auto-connect.
        return false;
    }

    _wifi.setWiFiTimeout(timeoutMs);
    _wifi.setWiFi(ssid, password);
    _wifi.connectWiFi();

    // Bounded, blocking-but-timeout-limited wait ONLY during begin().
    // This is the one place in the library where a short blocking
    // wait is acceptable: the user explicitly called begin() and
    // expects WiFi to be up (or definitively failed) before setup()
    // returns, matching the spec's minimal usage example. Runtime
    // reconnects after this point are fully non-blocking via
    // WiFiManager::update() in loopMaintenance().
    unsigned long start = millis();
    while (_wifi.getWiFiStatus() == WiFiState::CONNECTING && (millis() - start) < timeoutMs) {
        _wifi.update();
        delay(50);
    }

    return _wifi.isWiFiConnected();
}

void ClimateCore::bootStageProvider() {
    _providers.useOpenMeteo();
}

void ClimateCore::bootStageReady() {
    _state.setState(SystemState::READY);
    _events.emit(Event::SYSTEM_READY);
}

void ClimateCore::loopMaintenance() {
    _wifi.update();
}

} // namespace AmelTech
