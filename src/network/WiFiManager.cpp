/*
 * WiFiManager.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "WiFiManager.h"

namespace AmelTech {

WiFiManager::WiFiManager()
    : _ssid(""), _password(""),
      _state(WiFiState::IDLE),
      _connectStartedAt(0),
      _lastDisconnectAt(0),
      _wifiTimeoutMs(15000),
      _reconnectIntervalMs(10000),
      _autoReconnect(true),
      _wasConnected(false),
      _errorManager(nullptr),
      _events(nullptr) {
}

void WiFiManager::attachErrorManager(ErrorManager* errorManager) {
    _errorManager = errorManager;
}

void WiFiManager::attachEvents(ClimateEvents* events) {
    _events = events;
}

void WiFiManager::setError(ErrorCode code, const char* msg) {
    if (_errorManager) _errorManager->setError(code, msg);
}

void WiFiManager::emit(Event event, const String& detail) {
    if (_events) _events->emit(event, detail);
}

void WiFiManager::setWiFi(const char* ssid, const char* password) {
    setWiFiCredentials(ssid, password);
}

void WiFiManager::setWiFiCredentials(const char* ssid, const char* password) {
    _ssid = String(ssid);
    _password = String(password);
}

void WiFiManager::setSSID(const char* ssid) {
    _ssid = String(ssid);
}

void WiFiManager::setPassword(const char* password) {
    _password = String(password);
}

bool WiFiManager::connectWiFi() {
    if (_ssid.length() == 0) {
        setError(ErrorCode::WIFI_ERROR, "No SSID configured");
        _state = WiFiState::FAILED;
        return false;
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid.c_str(), _password.c_str());

    _state = WiFiState::CONNECTING;
    _connectStartedAt = millis();
    return true;
}

void WiFiManager::disconnectWiFi() {
    WiFi.disconnect(true);
    _state = WiFiState::DISCONNECTED;
    _lastDisconnectAt = millis();
}

bool WiFiManager::reconnectWiFi() {
    disconnectWiFi();
    return connectWiFi();
}

void WiFiManager::update() {
    wl_status_t status = WiFi.status();

    if (_state == WiFiState::CONNECTING) {
        if (status == WL_CONNECTED) {
            _state = WiFiState::CONNECTED;
        } else if (millis() - _connectStartedAt > _wifiTimeoutMs) {
            _state = WiFiState::FAILED;
            setError(ErrorCode::WIFI_TIMEOUT, "WiFi connect timed out");
            _lastDisconnectAt = millis();
        }
    } else if (_state == WiFiState::CONNECTED) {
        if (status != WL_CONNECTED) {
            _state = WiFiState::DISCONNECTED;
            _lastDisconnectAt = millis();
            setError(ErrorCode::WIFI_DISCONNECTED, "WiFi connection lost");
        }
    } else if (_state == WiFiState::DISCONNECTED || _state == WiFiState::FAILED) {
        if (_autoReconnect && (millis() - _lastDisconnectAt > _reconnectIntervalMs)) {
            connectWiFi(); // moves state back to CONNECTING; retried again if it fails
        }
    }

    // Edge-triggered events, fired once per transition rather than every loop().
    bool nowConnected = (_state == WiFiState::CONNECTED);
    if (nowConnected && !_wasConnected) {
        emit(Event::WIFI_CONNECTED, getIP());
    } else if (!nowConnected && _wasConnected) {
        emit(Event::WIFI_DISCONNECTED);
    }
    _wasConnected = nowConnected;
}

bool WiFiManager::isWiFiConnected() const {
    return _state == WiFiState::CONNECTED && WiFi.status() == WL_CONNECTED;
}

WiFiState WiFiManager::getWiFiStatus() const {
    return _state;
}

int32_t WiFiManager::getRSSI() const {
    if (!isWiFiConnected()) return 0;
    return WiFi.RSSI();
}

String WiFiManager::getIP() const {
    if (!isWiFiConnected()) return "0.0.0.0";
    return WiFi.localIP().toString();
}

String WiFiManager::getGateway() const {
    if (!isWiFiConnected()) return "0.0.0.0";
    return WiFi.gatewayIP().toString();
}

String WiFiManager::getSubnet() const {
    if (!isWiFiConnected()) return "0.0.0.0";
    return WiFi.subnetMask().toString();
}

String WiFiManager::getDNS() const {
    if (!isWiFiConnected()) return "0.0.0.0";
    return WiFi.dnsIP().toString();
}

void WiFiManager::setWiFiTimeout(unsigned long ms) {
    _wifiTimeoutMs = ms;
}

unsigned long WiFiManager::getWiFiTimeout() const {
    return _wifiTimeoutMs;
}

void WiFiManager::setReconnectInterval(unsigned long ms) {
    _reconnectIntervalMs = ms;
}

unsigned long WiFiManager::getReconnectInterval() const {
    return _reconnectIntervalMs;
}

void WiFiManager::enableAutoReconnect() {
    _autoReconnect = true;
}

void WiFiManager::disableAutoReconnect() {
    _autoReconnect = false;
}

bool WiFiManager::isAutoReconnectEnabled() const {
    return _autoReconnect;
}

} // namespace AmelTech
