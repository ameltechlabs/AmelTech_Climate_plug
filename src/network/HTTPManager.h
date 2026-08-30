/*
 * HTTPManager.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * Thin wrapper around ESP32 HTTPClient.h providing GET/POST/PUT/PATCH,
 * response size limiting, and timing stats. HTTPClient's underlying
 * calls (http.GET() etc.) are themselves blocking at the TCP level,
 * but bounded by setHTTPTimeout()/setConnectTimeout() so they never
 * hang indefinitely - callers (RequestManager) are responsible for
 * only invoking these from scheduled, non-blocking-friendly points
 * rather than every loop() iteration. See Rule 8/9.
 */

#ifndef AMELTECH_HTTP_MANAGER_H
#define AMELTECH_HTTP_MANAGER_H

#include <Arduino.h>
#include <HTTPClient.h>
#include "../diagnostics/ErrorManager.h"

namespace AmelTech {

enum class HTTPMethod {
    GET,
    POST,
    PUT,
    PATCH
};

struct HTTPResponse {
    bool success;
    int statusCode;
    String body;
    size_t responseSize;
    unsigned long responseTimeMs;
    ErrorCode error;
};

class HTTPManager {
public:
    HTTPManager();

    void attachErrorManager(ErrorManager* errorManager);

    HTTPResponse GET(const String& url);
    HTTPResponse POST(const String& url, const String& body, const String& contentType = "application/json");
    HTTPResponse PUT(const String& url, const String& body, const String& contentType = "application/json");
    HTTPResponse PATCH(const String& url, const String& body, const String& contentType = "application/json");

    void setHTTPTimeout(unsigned long ms);
    unsigned long getHTTPTimeout() const;

    void setConnectTimeout(unsigned long ms);
    unsigned long getConnectTimeout() const;

    void setResponseSizeLimit(size_t bytes);
    size_t getResponseSizeLimit() const;

    // Stats from the most recent request.
    int getHTTPStatus() const;
    int getResponseCode() const;
    size_t getResponseSize() const;
    unsigned long getResponseTime() const;

private:
    unsigned long _httpTimeoutMs;
    unsigned long _connectTimeoutMs;
    size_t _responseSizeLimit;

    int _lastStatus;
    size_t _lastResponseSize;
    unsigned long _lastResponseTimeMs;

    ErrorManager* _errorManager;

    HTTPResponse execute(HTTPMethod method, const String& url, const String& body, const String& contentType);
    void setError(ErrorCode code, const char* msg);
};

} // namespace AmelTech

#endif // AMELTECH_HTTP_MANAGER_H
