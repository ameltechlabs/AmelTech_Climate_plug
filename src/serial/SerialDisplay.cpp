/*
 * SerialDisplay.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "SerialDisplay.h"

namespace AmelTech {

SerialDisplay::SerialDisplay() {
}

String SerialDisplay::fmtFloat(bool has, float value, const char* unit) const {
    if (!has) return "N/A";
    return String(value, 2) + " " + String(unit);
}

String SerialDisplay::fmtInt(bool has, int value) const {
    if (!has) return "N/A";
    return String(value);
}

String SerialDisplay::fmtString(bool has, const String& value) const {
    if (!has || value.length() == 0) return "N/A";
    return value;
}

String SerialDisplay::renderStatus(const ClimateData& data,
                                    const String& locationName, float latitude, float longitude,
                                    const String& providerName,
                                    bool liveRunning,
                                    unsigned long staleLifetimeMs) const {
    String out = "";

    bool isStale = (data.receivedAt == 0) || (millis() - data.receivedAt > staleLifetimeMs) || data.stale;

    out += "==================================================\n";
    out += String(liveRunning ? "[LIVE] " : "") + "Climate Status" + String(isStale ? " [STALE]" : "") + "\n";
    out += "==================================================\n";

    out += "Location      : " + (locationName.length() ? locationName : String("N/A")) + "\n";
    out += "Latitude      : " + String(latitude, 6) + "\n";
    out += "Longitude     : " + String(longitude, 6) + "\n";
    out += "Provider      : " + providerName + "\n";
    out += "Data source   : " + String(dataSourceToString(data.source)) + "\n";
    out += "Data state    : " + String(data.valid ? "VALID" : "INVALID") + "\n";

    if (data.receivedAt == 0) {
        out += "Last update   : never\n";
        out += "Data age      : N/A\n";
    } else {
        unsigned long ageMs = millis() - data.receivedAt;
        out += "Last update   : " + String(ageMs / 1000) + "s ago\n";
        out += "Data age      : " + String(ageMs / 1000) + "s\n";
    }

    out += "--------------------------------------------------\n";
    out += "Temperature       : " + fmtFloat(data.hasTemperature, data.temperature, "C") + "\n";
    out += "Relative humidity : " + fmtFloat(data.hasRelativeHumidity, data.relativeHumidity, "%") + "\n";
    out += "Dew point         : " + fmtFloat(data.hasDewPoint, data.dewPoint, "C") + "\n";
    out += "Apparent temp     : " + fmtFloat(data.hasApparentTemperature, data.apparentTemperature, "C") + "\n";
    out += "Atmospheric press.: " + fmtFloat(data.hasAtmosphericPressure, data.atmosphericPressure, "hPa") + "\n";
    out += "Precipitation     : " + fmtFloat(data.hasPrecipitation, data.precipitation, "mm") + "\n";
    out += "Rain              : " + fmtFloat(data.hasRain, data.rain, "mm") + "\n";
    out += "Snowfall          : " + fmtFloat(data.hasSnowfall, data.snowfall, "cm") + "\n";
    out += "Weather code      : " + fmtInt(data.hasWeatherCode, data.weatherCode) + "\n";
    out += "Weather condition : " + fmtString(data.hasWeatherCondition, data.weatherCondition) + "\n";
    out += "Cloud cover       : " + fmtFloat(data.hasCloudCover, data.cloudCover, "%") + "\n";
    out += "Wind speed        : " + fmtFloat(data.hasWindSpeed, data.windSpeed, "km/h") + "\n";
    out += "Wind direction    : " + fmtFloat(data.hasWindDirection, data.windDirection, "deg") + "\n";
    out += "Wind gusts        : " + fmtFloat(data.hasWindGusts, data.windGusts, "km/h") + "\n";
    out += "Solar radiation   : " + fmtFloat(data.hasSolarRadiation, data.solarRadiation, "W/m2") + "\n";
    out += "UV index          : " + fmtFloat(data.hasUvIndex, data.uvIndex, "") + "\n";
    out += "Evapotranspiration: " + fmtFloat(data.hasEvapotranspiration, data.evapotranspiration, "mm") + "\n";
    out += "Soil temperature  : " + fmtFloat(data.hasSoilTemperature, data.soilTemperature, "C") + "\n";
    out += "Soil moisture     : " + fmtFloat(data.hasSoilMoisture, data.soilMoisture, "m3/m3") + "\n";
    out += "Freezing level    : " + fmtFloat(data.hasFreezingLevel, data.freezingLevel, "m") + "\n";
    out += "Visibility        : " + fmtFloat(data.hasVisibility, data.visibility, "m") + "\n";
    out += "CAPE              : " + fmtFloat(data.hasCape, data.cape, "J/kg") + "\n";
    out += "==================================================\n";

    return out;
}

} // namespace AmelTech
