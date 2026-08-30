/*
 * PlatformAdapter.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * RULE 5: "Blynk, MQTT, REST, Webhook, and other platforms consume
 * ClimateData rather than directly coupling themselves to
 * Open-Meteo." Every concrete adapter below implements this
 * interface and only ever receives a ClimateData snapshot - none of
 * them import providers/ or know Open-Meteo exists.
 */

#ifndef AMELTECH_PLATFORM_ADAPTER_H
#define AMELTECH_PLATFORM_ADAPTER_H

#include <Arduino.h>
#include "../data/ClimateData.h"
#include "../diagnostics/ErrorManager.h"

namespace AmelTech {

enum class PlatformType {
    BLYNK,
    MQTT,
    REST,
    WEBHOOK
};

const char* platformTypeToString(PlatformType type);

class PlatformAdapter {
public:
    virtual ~PlatformAdapter() {}

    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;

    // Non-blocking per-loop maintenance (e.g. MQTT's PubSubClient
    // needs loop() called regularly to process incoming packets and
    // keep the connection alive).
    virtual void update() = 0;

    // Sends a ClimateData snapshot through this platform.
    virtual bool publishClimateData(const ClimateData& data) = 0;

    virtual PlatformType getType() const = 0;
    virtual String getName() const = 0;
    virtual bool isConfigured() const = 0;
};

} // namespace AmelTech

#endif // AMELTECH_PLATFORM_ADAPTER_H
