/*
 * ConnectionManager.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "ConnectionManager.h"

namespace AmelTech {

ConnectionManager::ConnectionManager()
    : _wifi(nullptr), _http(nullptr), _tls(nullptr) {
}

void ConnectionManager::attach(WiFiManager* wifi, HTTPManager* http, TLSManager* tls) {
    _wifi = wifi;
    _http = http;
    _tls = tls;
}

bool ConnectionManager::isReady(const String& url) const {
    if (_wifi == nullptr || !_wifi->isWiFiConnected()) return false;

    bool isHttps = url.startsWith("https://");
    if (isHttps && (_tls == nullptr || !_tls->isTLS())) return false;

    return true;
}

} // namespace AmelTech
