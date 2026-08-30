/*
 * OpenMeteoProvider.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * JSON PARSING NOTE: this implementation uses ArduinoJson (the de
 * facto standard JSON library for Arduino/ESP32; listed as an
 * optional dependency in library.properties). This keeps parsing
 * memory-bounded via StaticJsonDocument/DynamicJsonDocument sizing
 * rather than hand-rolled string scanning, per the spec's "Limit
 * JSON parsing memory" requirement under MEMORY AND STABILITY.
 */

#include "OpenMeteoProvider.h"
#include <ArduinoJson.h>

namespace AmelTech {

// The protected base URL. "current" params are appended dynamically
// based on _enabledParams; hourly-only params (see ParameterRegistry)
// are appended to a separate &hourly= list.
const char* const OpenMeteoProvider::BASE_URL = "https://api.open-meteo.com/v1/forecast";

OpenMeteoProvider::OpenMeteoProvider() {
    resetOpenMeteoVariables();
}

void OpenMeteoProvider::resetOpenMeteoVariables() {
    // Default: every documented parameter enabled.
    for (int i = 0; i < (int)ClimateParameter::PARAMETER_COUNT; i++) {
        _enabledParams[i] = true;
    }
}

void OpenMeteoProvider::setOpenMeteoVariables(const bool enabledFlags[(int)ClimateParameter::PARAMETER_COUNT]) {
    for (int i = 0; i < (int)ClimateParameter::PARAMETER_COUNT; i++) {
        _enabledParams[i] = enabledFlags[i];
    }
}

String OpenMeteoProvider::buildRequestURL(float latitude, float longitude) const {
    String url = String(BASE_URL);
    url += "?latitude=" + String(latitude, 6);
    url += "&longitude=" + String(longitude, 6);

    String currentParams = "";
    String hourlyParams = "";

    for (int i = 0; i < (int)ClimateParameter::PARAMETER_COUNT; i++) {
        if (!_enabledParams[i]) continue;
        ClimateParameter p = (ClimateParameter)i;
        const char* token = ParameterRegistry::toQueryToken(p);
        if (token == nullptr || strlen(token) == 0) continue;

        if (ParameterRegistry::isHourlyOnly(p)) {
            if (hourlyParams.length() > 0) hourlyParams += ",";
            hourlyParams += token;
        } else {
            if (currentParams.length() > 0) currentParams += ",";
            currentParams += token;
        }
    }

    if (currentParams.length() > 0) {
        url += "&current=" + currentParams;
    }
    if (hourlyParams.length() > 0) {
        url += "&hourly=" + hourlyParams;
        url += "&forecast_days=1"; // minimal hourly payload - we only read index 0
    }

    url += "&timezone=auto";
    url += "&wind_speed_unit=kmh";
    url += "&precipitation_unit=mm";

    return url;
}

bool OpenMeteoProvider::extractCurrentField(const String& json, const char* key, float& valueOut) const {
    // Parsed by caller via ArduinoJson; this helper is kept for
    // interface symmetry but the real extraction happens in
    // parseResponse() directly against the deserialized document,
    // since re-parsing per field would be wasteful. See parseResponse.
    (void)json; (void)key; (void)valueOut;
    return false;
}

bool OpenMeteoProvider::extractHourlyFirstField(const String& json, const char* key, float& valueOut) const {
    (void)json; (void)key; (void)valueOut;
    return false;
}

bool OpenMeteoProvider::parseResponse(const String& jsonBody, ClimateData& out, ErrorCode& errorOut) const {
    out.reset();

    // Bounded document size: Open-Meteo "current"+minimal "hourly"
    // payloads are well under 4KB in practice; 8KB gives headroom
    // without letting a malformed/huge response exhaust heap.
    DynamicJsonDocument doc(8192);
    DeserializationError err = deserializeJson(doc, jsonBody);

    if (err) {
        errorOut = ErrorCode::JSON_ERROR;
        return false;
    }

    if (doc.containsKey("error") && doc["error"].as<bool>()) {
        errorOut = ErrorCode::PROVIDER_ERROR;
        return false;
    }

    JsonObject current = doc["current"];
    if (current.isNull()) {
        errorOut = ErrorCode::RESPONSE_ERROR;
        return false;
    }

    out.timestamp = current["time"].is<const char*>() ? 0 : 0; // Open-Meteo returns ISO8601 string; epoch left 0 (unknown) intentionally, see receivedAt for real timing
    out.receivedAt = millis();
    out.source = DataSource::REMOTE;

    if (current.containsKey("temperature_2m")) {
        out.temperature = current["temperature_2m"].as<float>();
        out.hasTemperature = true;
    }
    if (current.containsKey("relative_humidity_2m")) {
        out.relativeHumidity = current["relative_humidity_2m"].as<float>();
        out.hasRelativeHumidity = true;
    }
    if (current.containsKey("dew_point_2m")) {
        out.dewPoint = current["dew_point_2m"].as<float>();
        out.hasDewPoint = true;
    }
    if (current.containsKey("apparent_temperature")) {
        out.apparentTemperature = current["apparent_temperature"].as<float>();
        out.hasApparentTemperature = true;
    }
    if (current.containsKey("pressure_msl")) {
        out.atmosphericPressure = current["pressure_msl"].as<float>();
        out.hasAtmosphericPressure = true;
    }
    if (current.containsKey("precipitation")) {
        out.precipitation = current["precipitation"].as<float>();
        out.hasPrecipitation = true;
    }
    if (current.containsKey("rain")) {
        out.rain = current["rain"].as<float>();
        out.hasRain = true;
    }
    if (current.containsKey("snowfall")) {
        out.snowfall = current["snowfall"].as<float>();
        out.hasSnowfall = true;
    }
    if (current.containsKey("weather_code")) {
        out.weatherCode = current["weather_code"].as<int>();
        out.hasWeatherCode = true;
    }
    if (current.containsKey("cloud_cover")) {
        out.cloudCover = current["cloud_cover"].as<float>();
        out.hasCloudCover = true;
    }
    if (current.containsKey("wind_speed_10m")) {
        out.windSpeed = current["wind_speed_10m"].as<float>();
        out.hasWindSpeed = true;
    }
    if (current.containsKey("wind_direction_10m")) {
        out.windDirection = current["wind_direction_10m"].as<float>();
        out.hasWindDirection = true;
    }
    if (current.containsKey("wind_gusts_10m")) {
        out.windGusts = current["wind_gusts_10m"].as<float>();
        out.hasWindGusts = true;
    }
    if (current.containsKey("shortwave_radiation")) {
        out.solarRadiation = current["shortwave_radiation"].as<float>();
        out.hasSolarRadiation = true;
    }
    if (current.containsKey("uv_index")) {
        out.uvIndex = current["uv_index"].as<float>();
        out.hasUvIndex = true;
    }

    // Hourly-only params: read array index 0 (the current hour), if present.
    JsonObject hourly = doc["hourly"];
    if (!hourly.isNull()) {
        if (hourly.containsKey("evapotranspiration") && hourly["evapotranspiration"].is<JsonArray>()) {
            JsonArray arr = hourly["evapotranspiration"].as<JsonArray>();
            if (arr.size() > 0 && !arr[0].isNull()) {
                out.evapotranspiration = arr[0].as<float>();
                out.hasEvapotranspiration = true;
            }
        }
        if (hourly.containsKey("soil_temperature_0cm") && hourly["soil_temperature_0cm"].is<JsonArray>()) {
            JsonArray arr = hourly["soil_temperature_0cm"].as<JsonArray>();
            if (arr.size() > 0 && !arr[0].isNull()) {
                out.soilTemperature = arr[0].as<float>();
                out.hasSoilTemperature = true;
            }
        }
        if (hourly.containsKey("soil_moisture_0_to_1cm") && hourly["soil_moisture_0_to_1cm"].is<JsonArray>()) {
            JsonArray arr = hourly["soil_moisture_0_to_1cm"].as<JsonArray>();
            if (arr.size() > 0 && !arr[0].isNull()) {
                out.soilMoisture = arr[0].as<float>();
                out.hasSoilMoisture = true;
            }
        }
        if (hourly.containsKey("freezing_level_height") && hourly["freezing_level_height"].is<JsonArray>()) {
            JsonArray arr = hourly["freezing_level_height"].as<JsonArray>();
            if (arr.size() > 0 && !arr[0].isNull()) {
                out.freezingLevel = arr[0].as<float>();
                out.hasFreezingLevel = true;
            }
        }
        if (hourly.containsKey("visibility") && hourly["visibility"].is<JsonArray>()) {
            JsonArray arr = hourly["visibility"].as<JsonArray>();
            if (arr.size() > 0 && !arr[0].isNull()) {
                out.visibility = arr[0].as<float>();
                out.hasVisibility = true;
            }
        }
        if (hourly.containsKey("cape") && hourly["cape"].is<JsonArray>()) {
            JsonArray arr = hourly["cape"].as<JsonArray>();
            if (arr.size() > 0 && !arr[0].isNull()) {
                out.cape = arr[0].as<float>();
                out.hasCape = true;
            }
        }
    }

    errorOut = ErrorCode::OK;
    return true;
}

String OpenMeteoProvider::getOpenMeteoConfiguration() const {
    String cfg = "Provider: Open-Meteo (protected)\n";
    cfg += "Base URL: " + String(BASE_URL) + " [read-only]\n";
    cfg += "Enabled parameters: ";
    uint8_t enabledCount = 0;
    for (int i = 0; i < (int)ClimateParameter::PARAMETER_COUNT; i++) {
        if (_enabledParams[i]) enabledCount++;
    }
    cfg += String(enabledCount) + "/" + String((int)ClimateParameter::PARAMETER_COUNT);
    return cfg;
}

} // namespace AmelTech
