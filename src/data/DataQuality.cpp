/*
 * DataQuality.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "DataQuality.h"

namespace AmelTech {

DataQuality::DataQuality(float acceptableThreshold) : _threshold(acceptableThreshold) {
}

QualityReport DataQuality::assess(const ClimateData& data) const {
    QualityReport report;
    report.fieldsTotal = 20;
    report.fieldsPresent = 0;

    if (data.hasTemperature) report.fieldsPresent++;
    if (data.hasRelativeHumidity) report.fieldsPresent++;
    if (data.hasDewPoint) report.fieldsPresent++;
    if (data.hasApparentTemperature) report.fieldsPresent++;
    if (data.hasAtmosphericPressure) report.fieldsPresent++;
    if (data.hasPrecipitation) report.fieldsPresent++;
    if (data.hasRain) report.fieldsPresent++;
    if (data.hasSnowfall) report.fieldsPresent++;
    if (data.hasWeatherCode) report.fieldsPresent++;
    if (data.hasCloudCover) report.fieldsPresent++;
    if (data.hasWindSpeed) report.fieldsPresent++;
    if (data.hasWindDirection) report.fieldsPresent++;
    if (data.hasWindGusts) report.fieldsPresent++;
    if (data.hasSolarRadiation) report.fieldsPresent++;
    if (data.hasUvIndex) report.fieldsPresent++;
    if (data.hasEvapotranspiration) report.fieldsPresent++;
    if (data.hasSoilTemperature) report.fieldsPresent++;
    if (data.hasSoilMoisture) report.fieldsPresent++;
    if (data.hasFreezingLevel) report.fieldsPresent++;
    if (data.hasVisibility) report.fieldsPresent++;

    report.completeness = (float)report.fieldsPresent / (float)report.fieldsTotal;
    report.acceptable = data.valid && (report.completeness >= _threshold);
    return report;
}

void DataQuality::setThreshold(float threshold) {
    _threshold = threshold;
}

float DataQuality::getThreshold() const {
    return _threshold;
}

} // namespace AmelTech
