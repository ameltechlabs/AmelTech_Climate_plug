/*
 * TXManager.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * Sends data to a configured TX URLRecord using HTTPManager.
 * Implements the spec's TX functions: send/sendJSON/sendText/
 * sendRaw/sendClimateData/sendSensorData plus config getters/setters.
 */

#ifndef AMELTECH_TX_MANAGER_H
#define AMELTECH_TX_MANAGER_H

#include <Arduino.h>
#include "URLRecord.h"
#include "../network/HTTPManager.h"
#include "../data/ClimateData.h"
#include "../diagnostics/ErrorManager.h"

namespace AmelTech {

class TXManager {
public:
    TXManager();

    void attachHTTP(HTTPManager* http);
    void attachErrorManager(ErrorManager* errorManager);

    void setTXURL(const String& url);
    String getTXURL() const;

    void setTXMethod(URLMethod method);
    URLMethod getTXMethod() const;

    void setTXFormat(URLFormat format);
    URLFormat getTXFormat() const;

    bool isTXEnabled() const;
    void enableTX();
    void disableTX();

    // Generic send: body + explicit content-type.
    bool send(const String& body, const String& contentType = "application/json");
    bool sendData(const String& body) { return send(body, "application/json"); }
    bool sendJSON(const String& json) { return send(json, "application/json"); }
    bool sendText(const String& text) { return send(text, "text/plain"); }
    bool sendRaw(const String& raw) { return send(raw, "application/octet-stream"); }

    // Serializes a ClimateData snapshot to JSON and sends it.
    bool sendClimateData(const ClimateData& data);

    // Serializes an arbitrary name->value float map (used for local
    // sensor readings) to JSON and sends it.
    bool sendSensorData(const String sensorNames[], const float sensorValues[], uint8_t count);

private:
    HTTPManager* _http;
    ErrorManager* _errorManager;

    String _url;
    URLMethod _method;
    URLFormat _format;
    bool _enabled;

    void setError(ErrorCode code, const char* msg);
    String climateDataToJSON(const ClimateData& data) const;
};

} // namespace AmelTech

#endif // AMELTECH_TX_MANAGER_H
