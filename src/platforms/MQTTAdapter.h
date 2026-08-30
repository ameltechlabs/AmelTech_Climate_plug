/*
 * MQTTAdapter.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * OPTIONAL DEPENDENCY: requires "PubSubClient" by Nick O'Leary
 * (the standard lightweight MQTT client for Arduino/ESP32). Also
 * needs a WiFiClient (plain, for unencrypted MQTT broker connections
 * - see setUseTLS() for the WiFiClientSecure path via TLSManager).
 *
 * NON-BLOCKING NOTE: PubSubClient::loop() must be called frequently
 * (every AmelTechClimate::loop() call, via this adapter's update())
 * to process incoming messages and send keepalive pings - this is
 * exactly the kind of per-loop maintenance PlatformAdapter::update()
 * exists for.
 */

#ifndef AMELTECH_MQTT_ADAPTER_H
#define AMELTECH_MQTT_ADAPTER_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h> // OPTIONAL - install "PubSubClient" separately
#include "PlatformAdapter.h"
#include "../diagnostics/ErrorManager.h"

namespace AmelTech {

class MQTTAdapter : public PlatformAdapter {
public:
    MQTTAdapter();

    void attachErrorManager(ErrorManager* errorManager);

    void begin();

    void setBroker(const String& host);
    void setPort(uint16_t port);
    void setUsername(const String& username);
    void setPassword(const String& password);
    void setClientID(const String& clientId);

    bool publish(const String& topic, const String& payload, bool retained = false);
    bool subscribe(const String& topic);

    bool publishClimateData(const ClimateData& data) override;
    bool publishSensorData(const String sensorNames[], const float sensorValues[], uint8_t count, const String& topicPrefix);

    bool reconnect();

    // PlatformAdapter interface
    bool connect() override;
    void disconnect() override;
    bool isConnected() const override;
    void update() override; // calls PubSubClient::loop()
    PlatformType getType() const override { return PlatformType::MQTT; }
    String getName() const override { return "MQTT"; }
    bool isConfigured() const override { return _broker.length() > 0; }

    void loop() { update(); } // spec explicitly lists loop() as an MQTT function name

private:
    WiFiClient _wifiClient;
    PubSubClient _mqtt;

    String _broker;
    uint16_t _port;
    String _username;
    String _password;
    String _clientId;
    String _dataTopic;

    ErrorManager* _errorManager;

    void setError(ErrorCode code, const char* msg);
    String climateDataToJSON(const ClimateData& data) const;
};

} // namespace AmelTech

#endif // AMELTECH_MQTT_ADAPTER_H
