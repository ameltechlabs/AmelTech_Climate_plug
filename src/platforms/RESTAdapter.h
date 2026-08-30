/*
 * RESTAdapter.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * NO EXTERNAL DEPENDENCY - built entirely on network/HTTPManager,
 * which the core already includes. This is the lowest-friction
 * platform integration: any REST-compatible endpoint (a user's own
 * backend, a serverless function, Home Assistant's REST API, etc).
 */

#ifndef AMELTECH_REST_ADAPTER_H
#define AMELTECH_REST_ADAPTER_H

#include <Arduino.h>
#include "PlatformAdapter.h"
#include "../network/HTTPManager.h"
#include "../urls/URLRecord.h" // reuses URLMethod enum

namespace AmelTech {

class RESTAdapter : public PlatformAdapter {
public:
    RESTAdapter();

    void begin(); // spec: begin()

    void setEndpoint(const String& url);
    void setMethod(URLMethod method);

    void setHeaders(const String& headersJson); // e.g. {"Authorization":"Bearer xyz"}
    void setContentType(const String& contentType);

    // spec's direct HTTP verbs, exposed for advanced/manual use
    // beyond the standard publishClimateData() flow.
    bool GET(String& responseOut);
    bool POST(const String& body, String& responseOut);
    bool PUT(const String& body, String& responseOut);
    bool PATCH(const String& body, String& responseOut);

    bool sendJSON(const String& json);
    bool receiveJSON(String& out);

    bool testConnection();

    // PlatformAdapter interface
    bool connect() override;      // REST is stateless; "connect" = testConnection()
    void disconnect() override;   // no-op, nothing persistent to tear down
    bool isConnected() const override;
    void update() override;       // no-op, REST has no per-loop maintenance
    bool publishClimateData(const ClimateData& data) override;
    PlatformType getType() const override { return PlatformType::REST; }
    String getName() const override { return "REST"; }
    bool isConfigured() const override { return _endpoint.length() > 0; }

    void attachHTTP(HTTPManager* http);

private:
    HTTPManager* _http;
    String _endpoint;
    URLMethod _method;
    String _contentType;
    String _headersJson; // stored for reference; ESP32 HTTPClient headers set per-call in HTTPManager would need extension for arbitrary custom headers beyond Content-Type - see docs/Platforms.md for the current limitation
    bool _lastConnectOk;

    String climateDataToJSON(const ClimateData& data) const;
};

} // namespace AmelTech

#endif // AMELTECH_REST_ADAPTER_H
