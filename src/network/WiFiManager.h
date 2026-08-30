/*
 * WiFiManager.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * Wraps ESP32 WiFi.h with non-blocking connect, auto-reconnect, and
 * the getters the spec's "WI-FI FUNCTIONS" section requires.
 *
 * NON-BLOCKING NOTE: connectWiFi() starts the connection attempt and
 * returns immediately. Call update() from the main loop to poll
 * status and drive auto-reconnect; do not call WiFi.waitForConnect
 * or delay() loops here (see Rule 9 - loop() must stay non-blocking).
 */

#ifndef AMELTECH_WIFI_MANAGER_H
#define AMELTECH_WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include "../diagnostics/ErrorManager.h"
#include "../core/ClimateEvents.h"

namespace AmelTech {

enum class WiFiState {
    IDLE,
    CONNECTING,
    CONNECTED,
    DISCONNECTED,
    FAILED
};

class WiFiManager {
public:
    WiFiManager();

    void attachErrorManager(ErrorManager* errorManager);
    void attachEvents(ClimateEvents* events);

    // --- credentials ---
    void setWiFi(const char* ssid, const char* password);
    void setWiFiCredentials(const char* ssid, const char* password);
    void setSSID(const char* ssid);
    void setPassword(const char* password);

    // --- connection control (non-blocking) ---
    bool connectWiFi();
    void disconnectWiFi();
    bool reconnectWiFi();

    // Call every loop() iteration. Polls connection state, times out
    // stalled CONNECTING attempts, and drives auto-reconnect.
    void update();

    // --- status ---
    bool isWiFiConnected() const;
    WiFiState getWiFiStatus() const;

    int32_t getRSSI() const;
    String getIP() const;
    String getGateway() const;
    String getSubnet() const;
    String getDNS() const;

    // --- timing config ---
    void setWiFiTimeout(unsigned long ms);
    unsigned long getWiFiTimeout() const;

    void setReconnectInterval(unsigned long ms);
    unsigned long getReconnectInterval() const;

    void enableAutoReconnect();
    void disableAutoReconnect();
    bool isAutoReconnectEnabled() const;

private:
    String _ssid;
    String _password;

    WiFiState _state;
    unsigned long _connectStartedAt;
    unsigned long _lastDisconnectAt;
    unsigned long _wifiTimeoutMs;
    unsigned long _reconnectIntervalMs;
    bool _autoReconnect;
    bool _wasConnected; // for edge-triggering WIFI_CONNECTED/DISCONNECTED events

    ErrorManager* _errorManager;
    ClimateEvents* _events;

    void setError(ErrorCode code, const char* msg);
    void emit(Event event, const String& detail = "");
};

} // namespace AmelTech

#endif // AMELTECH_WIFI_MANAGER_H
