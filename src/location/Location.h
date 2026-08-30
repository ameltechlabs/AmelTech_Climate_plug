/*
 * Location.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * Plain data struct for one saved location, matching the spec's
 * "Create Location structure containing: id, name, latitude,
 * longitude, enabled".
 */

#ifndef AMELTECH_LOCATION_H
#define AMELTECH_LOCATION_H

#include <Arduino.h>

namespace AmelTech {

struct Location {
    int id;
    String name;
    float latitude;
    float longitude;
    bool enabled;

    Location();
    Location(int id, const String& name, float latitude, float longitude);

    bool isValidCoordinate() const;
};

} // namespace AmelTech

#endif // AMELTECH_LOCATION_H
