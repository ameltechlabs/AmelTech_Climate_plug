/*
 * PlatformManager.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * Manages up to one instance each of RESTAdapter and WebhookAdapter
 * directly (since neither has an optional-library dependency, they
 * can be owned here unconditionally), while MQTTAdapter and
 * BlynkAdapter are attached via pointer since they depend on
 * optional external libraries the core cannot assume are installed
 * - the user's sketch constructs those and calls attachMQTT()/
 * attachBlynk() only if it includes the relevant headers.
 */

#ifndef AMELTECH_PLATFORM_MANAGER_H
#define AMELTECH_PLATFORM_MANAGER_H

#include <Arduino.h>
#include "PlatformAdapter.h"
#include "RESTAdapter.h"
#include "WebhookAdapter.h"
#include "../data/ClimateData.h"
#include "../network/HTTPManager.h"
#include "../diagnostics/ErrorManager.h"
#include "../core/ClimateEvents.h"

namespace AmelTech {

class PlatformManager {
public:
    PlatformManager();

    void attachErrorManager(ErrorManager* errorManager);
    void attachEvents(ClimateEvents* events);
    void attachHTTP(HTTPManager* http);

    // REST and Webhook are always available (no optional dependency).
    RESTAdapter& rest() { return _rest; }
    WebhookAdapter& webhook() { return _webhook; }

    // MQTT and Blynk are optional-dependency; attach an
    // externally-owned instance (see class header). Passing nullptr
    // detaches.
    void attachMQTT(PlatformAdapter* mqttAdapter);
    void attachBlynk(PlatformAdapter* blynkAdapter);

    void enablePlatform(PlatformType type);
    void disablePlatform(PlatformType type);

    bool connectPlatform(PlatformType type);
    void disconnectPlatform(PlatformType type);
    bool isPlatformConnected(PlatformType type) const;

    // Generic attach/detach matching spec function names, operating
    // on whichever adapter is passed (for adapters not covered by
    // the type-specific methods above, e.g. a custom user adapter).
    bool attachPlatform(PlatformAdapter* adapter);
    void detachPlatform(PlatformAdapter* adapter);

    // Publishes to every currently-enabled, connected platform.
    // Returns count of platforms that succeeded.
    uint8_t publishClimateData(const ClimateData& data);
    uint8_t publishSensorData(const String sensorNames[], const float sensorValues[], uint8_t count);

    bool sendPlatformData(PlatformType type, const ClimateData& data);
    bool receivePlatformData(PlatformType type, String& out); // currently meaningful for REST only - see docs/Platforms.md

    // Call every loop() - drives update() on every attached/enabled adapter.
    void update();

    String getPlatformStatus() const;

private:
    static const uint8_t MAX_EXTRA_ADAPTERS = 4;

    RESTAdapter _rest;
    WebhookAdapter _webhook;
    PlatformAdapter* _mqtt;   // externally owned, optional
    PlatformAdapter* _blynk;  // externally owned, optional
    PlatformAdapter* _extra[MAX_EXTRA_ADAPTERS]; // generic attachPlatform() slots
    uint8_t _extraCount;

    bool _restEnabled;
    bool _webhookEnabled;
    bool _mqttEnabled;
    bool _blynkEnabled;

    ErrorManager* _errorManager;
    ClimateEvents* _events;

    void setError(ErrorCode code, const char* msg);
    PlatformAdapter* resolve(PlatformType type) const;
};

} // namespace AmelTech

#endif // AMELTECH_PLATFORM_MANAGER_H
