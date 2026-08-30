/*
 * SerialDisplay.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * Formats a ClimateData snapshot into the exact display the spec's
 * "LIVE STATUS DISPLAY" section requires: every field shown, N/A for
 * unavailable, STALE flagged, LIVE flagged when live status is
 * running.
 */

#ifndef AMELTECH_SERIAL_DISPLAY_H
#define AMELTECH_SERIAL_DISPLAY_H

#include <Arduino.h>
#include "../data/ClimateData.h"

namespace AmelTech {

class SerialDisplay {
public:
    SerialDisplay();

    // locationName/lat/lon/providerName are passed separately since
    // ClimateData itself doesn't carry location/provider identity.
    String renderStatus(const ClimateData& data,
                         const String& locationName, float latitude, float longitude,
                         const String& providerName,
                         bool liveRunning,
                         unsigned long staleLifetimeMs) const;

private:
    String fmtFloat(bool has, float value, const char* unit) const;
    String fmtInt(bool has, int value) const;
    String fmtString(bool has, const String& value) const;
};

} // namespace AmelTech

#endif // AMELTECH_SERIAL_DISPLAY_H
