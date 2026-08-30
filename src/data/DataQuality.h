/*
 * DataQuality.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * Computes a completeness score for a ClimateData snapshot: what
 * fraction of the 20 tracked parameters are actually populated
 * (hasX == true). Useful for diagnostics and for hybrid mode to
 * decide whether local sensor data meaningfully improves on remote.
 */

#ifndef AMELTECH_DATA_QUALITY_H
#define AMELTECH_DATA_QUALITY_H

#include "ClimateData.h"

namespace AmelTech {

struct QualityReport {
    uint8_t fieldsPresent;
    uint8_t fieldsTotal;
    float completeness;   // 0.0 - 1.0
    bool acceptable;      // completeness >= threshold and data.valid
};

class DataQuality {
public:
    explicit DataQuality(float acceptableThreshold = 0.5f);

    QualityReport assess(const ClimateData& data) const;

    void setThreshold(float threshold);
    float getThreshold() const;

private:
    float _threshold;
};

} // namespace AmelTech

#endif // AMELTECH_DATA_QUALITY_H
