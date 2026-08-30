/*
 * Location.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "Location.h"

namespace AmelTech {

Location::Location() : id(-1), name(""), latitude(0.0f), longitude(0.0f), enabled(false) {
}

Location::Location(int id, const String& name, float latitude, float longitude)
    : id(id), name(name), latitude(latitude), longitude(longitude), enabled(true) {
}

bool Location::isValidCoordinate() const {
    return latitude >= -90.0f && latitude <= 90.0f &&
           longitude >= -180.0f && longitude <= 180.0f;
}

} // namespace AmelTech
