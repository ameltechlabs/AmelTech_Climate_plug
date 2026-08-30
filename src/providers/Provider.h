/*
 * Provider.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * Abstract base for anything that can produce a ClimateData snapshot
 * from a network request: the built-in OpenMeteoProvider and the
 * user-facing CustomProvider both implement this interface, so
 * RequestManager/ClimateController can drive either one uniformly.
 */

#ifndef AMELTECH_PROVIDER_H
#define AMELTECH_PROVIDER_H

#include <Arduino.h>
#include "../data/ClimateData.h"
#include "../diagnostics/ErrorManager.h"

namespace AmelTech {

enum class ProviderType {
    OPEN_METEO,
    CUSTOM
};

class Provider {
public:
    virtual ~Provider() {}

    // Builds the full request URL for the given lat/lon.
    virtual String buildRequestURL(float latitude, float longitude) const = 0;

    // Parses a raw HTTP JSON body into a ClimateData snapshot.
    // Returns false (and sets errorOut) on unparseable/malformed input.
    virtual bool parseResponse(const String& jsonBody, ClimateData& out, ErrorCode& errorOut) const = 0;

    virtual ProviderType getType() const = 0;
    virtual String getName() const = 0;

    // Whether this provider instance currently has enough config
    // (e.g. a non-empty custom URL) to be usable.
    virtual bool isConfigured() const = 0;
};

} // namespace AmelTech

#endif // AMELTECH_PROVIDER_H
