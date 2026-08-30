/*
 * LocationValidator.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * Standalone validator matching spec's "LOCATION VALIDATION" section
 * exactly: latitude -90..+90, longitude -180..+180. Kept separate
 * from Location.h so it can validate raw candidate values BEFORE a
 * Location object is constructed (e.g. while parsing serial input,
 * before deciding whether to save anything).
 */

#ifndef AMELTECH_LOCATION_VALIDATOR_H
#define AMELTECH_LOCATION_VALIDATOR_H

#include "Location.h"

namespace AmelTech {

class LocationValidator {
public:
    LocationValidator();

    bool validateLatitude(float latitude) const;
    bool validateLongitude(float longitude) const;
    bool validateLocation(const Location& location) const;

    // Also validates the name isn't empty (a location with blank
    // name is not useful even if coordinates are in range).
    bool validateLocation(const String& name, float latitude, float longitude) const;

private:
    static constexpr float LAT_MIN = -90.0f;
    static constexpr float LAT_MAX = 90.0f;
    static constexpr float LON_MIN = -180.0f;
    static constexpr float LON_MAX = 180.0f;
};

} // namespace AmelTech

#endif // AMELTECH_LOCATION_VALIDATOR_H
