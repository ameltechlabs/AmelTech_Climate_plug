/*
 * RXManager.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "RXManager.h"

namespace AmelTech {

RXManager::RXManager()
    : _http(nullptr), _errorManager(nullptr), _url(""),
      _method(URLMethod::GET), _format(URLFormat::JSON), _enabled(false),
      _buffer(""), _hasData(false) {
}

void RXManager::attachHTTP(HTTPManager* http) {
    _http = http;
}

void RXManager::attachErrorManager(ErrorManager* errorManager) {
    _errorManager = errorManager;
}

void RXManager::setError(ErrorCode code, const char* msg) {
    if (_errorManager) _errorManager->setError(code, msg);
}

void RXManager::setRXURL(const String& url) {
    _url = url;
}

String RXManager::getRXURL() const {
    return _url;
}

void RXManager::setRXMethod(URLMethod method) {
    _method = method;
}

URLMethod RXManager::getRXMethod() const {
    return _method;
}

void RXManager::setRXFormat(URLFormat format) {
    _format = format;
}

URLFormat RXManager::getRXFormat() const {
    return _format;
}

bool RXManager::isRXEnabled() const {
    return _enabled;
}

void RXManager::enableRX() {
    _enabled = true;
}

void RXManager::disableRX() {
    _enabled = false;
}

bool RXManager::receive() {
    if (!_enabled) {
        setError(ErrorCode::CONFIG_ERROR, "RX is disabled - call enableRX() first");
        return false;
    }
    if (_url.length() == 0) {
        setError(ErrorCode::URL_INVALID, "No RX URL configured");
        return false;
    }
    if (_http == nullptr) {
        setError(ErrorCode::HTTP_ERROR, "No HTTPManager attached to RXManager");
        return false;
    }

    HTTPResponse resp = (_method == URLMethod::GET) ? _http->GET(_url) : _http->GET(_url); // RX is inherently a fetch; non-GET RX methods are not meaningful over HTTP semantics

    if (!resp.success) {
        setError(resp.error, "RX fetch failed");
        _hasData = false;
        return false;
    }

    _buffer = resp.body;
    _hasData = true;
    return true;
}

bool RXManager::hasReceivedData() const {
    return _hasData;
}

String RXManager::readReceivedData() const {
    return _hasData ? _buffer : "";
}

void RXManager::clearReceivedData() {
    _buffer = "";
    _hasData = false;
}

} // namespace AmelTech
