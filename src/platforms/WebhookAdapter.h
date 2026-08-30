/*
 * WebhookAdapter.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * NO EXTERNAL DEPENDENCY - built on HTTPManager, same as RESTAdapter.
 * Distinguished from RESTAdapter by intent/API shape: webhooks are
 * fire-and-forget event notifications (trigger/sendEvent), not a
 * request/response API a user polls.
 */

#ifndef AMELTECH_WEBHOOK_ADAPTER_H
#define AMELTECH_WEBHOOK_ADAPTER_H

#include <Arduino.h>
#include "PlatformAdapter.h"
#include "../network/HTTPManager.h"
#include "../urls/URLRecord.h"

namespace AmelTech {

class WebhookAdapter : public PlatformAdapter {
public:
    WebhookAdapter();

    void attachHTTP(HTTPManager* http);

    void setURL(const String& url);
    void setMethod(URLMethod method);

    // Fires the webhook with an empty/minimal body - just a ping.
    bool trigger();

    // Fires the webhook with a custom event name + payload.
    bool sendEvent(const String& eventName, const String& payloadJson);

    // Convenience: fires a "climate_update" event with the given
    // ClimateData serialized as the payload.
    bool sendClimateEvent(const ClimateData& data);

    bool test();

    // PlatformAdapter interface
    bool connect() override;
    void disconnect() override;
    bool isConnected() const override;
    void update() override;
    bool publishClimateData(const ClimateData& data) override { return sendClimateEvent(data); }
    PlatformType getType() const override { return PlatformType::WEBHOOK; }
    String getName() const override { return "Webhook"; }
    bool isConfigured() const override { return _url.length() > 0; }

private:
    HTTPManager* _http;
    String _url;
    URLMethod _method;
    bool _lastTriggerOk;

    String climateDataToJSON(const ClimateData& data) const;
};

} // namespace AmelTech

#endif // AMELTECH_WEBHOOK_ADAPTER_H
