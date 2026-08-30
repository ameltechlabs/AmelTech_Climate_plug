/*
 * LocationValidator.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "LocationValidator.h"

namespace AmelTech {

LocationValidator::LocationValidator() {
}

bool LocationValidator::validateLatitude(float latitude) const {
    return latitude >= LAT_MIN && latitude <= LAT_MAX;
}

bool LocationValidator::validateLongitude(float longitude) const {
    return longitude >= LON_MIN && longitude <= LON_MAX;
}

bool LocationValidator::validateLocation(const Location& location) const {
    return validateLatitude(location.latitude) &&
           validateLongitude(location.longitude) &&
           location.name.length() > 0;
}

bool LocationValidator::validateLocation(const String& name, float latitude, float longitude) const {
    return validateLatitude(latitude) &&
           validateLongitude(longitude) &&
           name.length() > 0;
}

} // namespace AmelTech
