/*
 * CustomProvider.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * User-controlled provider backed by a single custom URL (set via
 * URLManager -> "URL link change"). Entirely separate class and
 * separate storage from OpenMeteoProvider - see that file's header
 * comment for the protection rationale.
 *
 * Since arbitrary user endpoints won't follow Open-Meteo's JSON
 * shape, parseResponse() here does a best-effort flat-JSON scan:
 * it looks for top-level keys matching common climate field names
 * (e.g. "temperature", "humidity") rather than assuming Open-Meteo's
 * nested "current" object structure.
 */

#ifndef AMELTECH_CUSTOM_PROVIDER_H
#define AMELTECH_CUSTOM_PROVIDER_H

#include <Arduino.h>
#include "Provider.h"

namespace AmelTech {

class CustomProvider : public Provider {
public:
    CustomProvider();

    void setEndpoint(const String& url);
    String getEndpoint() const;

    String buildRequestURL(float latitude, float longitude) const override;
    bool parseResponse(const String& jsonBody, ClimateData& out, ErrorCode& errorOut) const override;

    ProviderType getType() const override { return ProviderType::CUSTOM; }
    String getName() const override { return "Custom"; }
    bool isConfigured() const override { return _endpoint.length() > 0; }

private:
    String _endpoint;

    // Attempts to read a float from any of the given candidate key
    // names (to tolerate minor naming variance across custom APIs).
    bool tryReadFloat(const String& jsonBody, const char* const* candidates, uint8_t count, float& out) const;
};

} // namespace AmelTech

#endif // AMELTECH_CUSTOM_PROVIDER_H
