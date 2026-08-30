/*
 * HTTPManager.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "HTTPManager.h"

namespace AmelTech {

HTTPManager::HTTPManager()
    : _httpTimeoutMs(10000),
      _connectTimeoutMs(5000),
      _responseSizeLimit(16384), // 16KB - generous for Open-Meteo "current" JSON, bounds worst case
      _lastStatus(0),
      _lastResponseSize(0),
      _lastResponseTimeMs(0),
      _errorManager(nullptr) {
}

void HTTPManager::attachErrorManager(ErrorManager* errorManager) {
    _errorManager = errorManager;
}

void HTTPManager::setError(ErrorCode code, const char* msg) {
    if (_errorManager) _errorManager->setError(code, msg);
}

HTTPResponse HTTPManager::execute(HTTPMethod method, const String& url, const String& body, const String& contentType) {
    HTTPResponse response;
    response.success = false;
    response.statusCode = -1;
    response.responseSize = 0;
    response.error = ErrorCode::OK;

    unsigned long startTime = millis();

    HTTPClient http;
    http.setConnectTimeout(_connectTimeoutMs);
    http.setTimeout(_httpTimeoutMs);

    if (!http.begin(url)) {
        response.error = ErrorCode::URL_ERROR;
        setError(ErrorCode::URL_ERROR, "HTTPClient::begin failed - malformed URL");
        response.responseTimeMs = millis() - startTime;
        return response;
    }

    if (contentType.length() > 0 && (method == HTTPMethod::POST || method == HTTPMethod::PUT || method == HTTPMethod::PATCH)) {
        http.addHeader("Content-Type", contentType);
    }

    int httpCode = -1;
    switch (method) {
        case HTTPMethod::GET:   httpCode = http.GET(); break;
        case HTTPMethod::POST:  httpCode = http.POST(body); break;
        case HTTPMethod::PUT:   httpCode = http.PUT(body); break;
        case HTTPMethod::PATCH: httpCode = http.PATCH(body); break;
    }

    response.responseTimeMs = millis() - startTime;
    response.statusCode = httpCode;
    _lastStatus = httpCode;
    _lastResponseTimeMs = response.responseTimeMs;

    if (httpCode < 0) {
        // Negative return from HTTPClient means a transport-level failure
        // (DNS, connect refused, timeout) rather than an HTTP status.
        response.error = (httpCode == HTTPC_ERROR_CONNECTION_REFUSED || httpCode == HTTPC_ERROR_CONNECTION_LOST)
                              ? ErrorCode::HTTP_ERROR
                              : ErrorCode::HTTP_TIMEOUT;
        setError(response.error, http.errorToString(httpCode).c_str());
        http.end();
        return response;
    }

    // Check Content-Length against our limit BEFORE reading the body,
    // so we never allocate more than the configured cap.
    int contentLength = http.getSize();
    if (contentLength > 0 && (size_t)contentLength > _responseSizeLimit) {
        response.error = ErrorCode::RESPONSE_ERROR;
        setError(ErrorCode::RESPONSE_ERROR, "Response exceeds configured size limit");
        http.end();
        return response;
    }

    String payload = http.getString();
    if (payload.length() > _responseSizeLimit) {
        payload = payload.substring(0, _responseSizeLimit);
        response.error = ErrorCode::RESPONSE_ERROR;
    }

    response.body = payload;
    response.responseSize = payload.length();
    _lastResponseSize = response.responseSize;

    if (httpCode >= 200 && httpCode < 300) {
        response.success = true;
    } else {
        response.error = ErrorCode::HTTP_STATUS_ERROR;
        setError(ErrorCode::HTTP_STATUS_ERROR, ("HTTP status " + String(httpCode)).c_str());
    }

    http.end();
    return response;
}

HTTPResponse HTTPManager::GET(const String& url) {
    return execute(HTTPMethod::GET, url, "", "");
}

HTTPResponse HTTPManager::POST(const String& url, const String& body, const String& contentType) {
    return execute(HTTPMethod::POST, url, body, contentType);
}

HTTPResponse HTTPManager::PUT(const String& url, const String& body, const String& contentType) {
    return execute(HTTPMethod::PUT, url, body, contentType);
}

HTTPResponse HTTPManager::PATCH(const String& url, const String& body, const String& contentType) {
    return execute(HTTPMethod::PATCH, url, body, contentType);
}

void HTTPManager::setHTTPTimeout(unsigned long ms) {
    _httpTimeoutMs = ms;
}

unsigned long HTTPManager::getHTTPTimeout() const {
    return _httpTimeoutMs;
}

void HTTPManager::setConnectTimeout(unsigned long ms) {
    _connectTimeoutMs = ms;
}

unsigned long HTTPManager::getConnectTimeout() const {
    return _connectTimeoutMs;
}

void HTTPManager::setResponseSizeLimit(size_t bytes) {
    _responseSizeLimit = bytes;
}

size_t HTTPManager::getResponseSizeLimit() const {
    return _responseSizeLimit;
}

int HTTPManager::getHTTPStatus() const {
    return _lastStatus;
}

int HTTPManager::getResponseCode() const {
    return _lastStatus;
}

size_t HTTPManager::getResponseSize() const {
    return _lastResponseSize;
}

unsigned long HTTPManager::getResponseTime() const {
    return _lastResponseTimeMs;
}

} // namespace AmelTech
