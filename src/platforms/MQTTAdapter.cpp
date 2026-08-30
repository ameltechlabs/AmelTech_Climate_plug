/*
 * MQTTAdapter.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "MQTTAdapter.h"
#include <ArduinoJson.h>

namespace AmelTech {

MQTTAdapter::MQTTAdapter()
    : _mqtt(_wifiClient), _broker(""), _port(1883), _username(""), _password(""),
      _clientId(""), _dataTopic("ameltech/climate"), _errorManager(nullptr) {
}

void MQTTAdapter::attachErrorManager(ErrorManager* errorManager) {
    _errorManager = errorManager;
}

void MQTTAdapter::setError(ErrorCode code, const char* msg) {
    if (_errorManager) _errorManager->setError(code, msg);
}

void MQTTAdapter::begin() {
    if (_clientId.length() == 0) {
        // Generate a reasonably-unique default client ID from the
        // ESP32's chip ID rather than requiring the user to set one.
        _clientId = "AmelTechClimate-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    }
    _mqtt.setServer(_broker.c_str(), _port);
}

void MQTTAdapter::setBroker(const String& host) {
    _broker = host;
}

void MQTTAdapter::setPort(uint16_t port) {
    _port = port;
}

void MQTTAdapter::setUsername(const String& username) {
    _username = username;
}

void MQTTAdapter::setPassword(const String& password) {
    _password = password;
}

void MQTTAdapter::setClientID(const String& clientId) {
    _clientId = clientId;
}

bool MQTTAdapter::connect() {
    if (_broker.length() == 0) {
        setError(ErrorCode::PLATFORM_ERROR, "No MQTT broker configured");
        return false;
    }

    _mqtt.setServer(_broker.c_str(), _port);

    bool ok;
    if (_username.length() > 0) {
        ok = _mqtt.connect(_clientId.c_str(), _username.c_str(), _password.c_str());
    } else {
        ok = _mqtt.connect(_clientId.c_str());
    }

    if (!ok) {
        setError(ErrorCode::PLATFORM_ERROR, "MQTT connect failed");
    }
    return ok;
}

bool MQTTAdapter::reconnect() {
    return connect();
}

void MQTTAdapter::disconnect() {
    _mqtt.disconnect();
}

bool MQTTAdapter::isConnected() const {
    // PubSubClient::connected() is not const-qualified upstream;
    // const_cast is safe here since connected() only reads socket
    // state and does not mutate adapter-visible behavior.
    return const_cast<PubSubClient&>(_mqtt).connected();
}

void MQTTAdapter::update() {
    if (isConnected()) {
        _mqtt.loop();
    }
}

bool MQTTAdapter::publish(const String& topic, const String& payload, bool retained) {
    if (!isConnected()) {
        setError(ErrorCode::PLATFORM_ERROR, "MQTT not connected");
        return false;
    }
    return _mqtt.publish(topic.c_str(), payload.c_str(), retained);
}

bool MQTTAdapter::subscribe(const String& topic) {
    if (!isConnected()) {
        setError(ErrorCode::PLATFORM_ERROR, "MQTT not connected");
        return false;
    }
    return _mqtt.subscribe(topic.c_str());
}

String MQTTAdapter::climateDataToJSON(const ClimateData& data) const {
    DynamicJsonDocument doc(2048);
    if (data.hasTemperature) doc["temperature"] = data.temperature;
    if (data.hasRelativeHumidity) doc["relative_humidity"] = data.relativeHumidity;
    if (data.hasDewPoint) doc["dew_point"] = data.dewPoint;
    if (data.hasAtmosphericPressure) doc["pressure"] = data.atmosphericPressure;
    if (data.hasPrecipitation) doc["precipitation"] = data.precipitation;
    if (data.hasWeatherCode) doc["weather_code"] = data.weatherCode;
    if (data.hasWindSpeed) doc["wind_speed"] = data.windSpeed;
    if (data.hasUvIndex) doc["uv_index"] = data.uvIndex;
    doc["valid"] = data.valid;
    doc["source"] = dataSourceToString(data.source);

    String out;
    serializeJson(doc, out);
    return out;
}

bool MQTTAdapter::publishClimateData(const ClimateData& data) {
    return publish(_dataTopic, climateDataToJSON(data), false);
}

bool MQTTAdapter::publishSensorData(const String sensorNames[], const float sensorValues[], uint8_t count, const String& topicPrefix) {
    bool allOk = true;
    for (uint8_t i = 0; i < count; i++) {
        String topic = topicPrefix + "/" + sensorNames[i];
        if (!publish(topic, String(sensorValues[i], 2), false)) {
            allOk = false;
        }
    }
    return allOk;
}

} // namespace AmelTech
