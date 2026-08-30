/*
 * OpenMeteoProvider.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * *** PROTECTED PROVIDER - RULE 1 & RULE 2 ***
 *
 * This class implements the built-in, no-API-key-required Open-Meteo
 * data source (open-meteo.com). Per the spec's "IMPORTANT OPEN-METEO
 * RULE" and "MOST IMPORTANT DESIGN RULES" sections:
 *
 *   - The base endpoint URL is a private compile-time constant below.
 *     There is intentionally NO public setter that can overwrite it.
 *   - URLManager/CustomProvider (the "URL link change" command) are
 *     entirely separate classes operating on separate storage; they
 *     have no reference to, and cannot reach, this class's internals.
 *   - See ProviderManager.cpp for the enforcement point that rejects
 *     any attempt to route a "protected provider modification" through
 *     the custom URL system, returning the exact message the spec
 *     requires: "Protected provider: modification not allowed."
 *
 * USAGE NOTE (Rule 11): this class only ever issues the single request
 * the scheduler currently calls for - it does not implement polling
 * faster than configured, request coalescing tricks, or anything else
 * intended to extract more calls than a normal client would make. See
 * README "External-service usage limitations" for the fair-use notice
 * users must read before deploying at scale.
 */

#ifndef AMELTECH_OPEN_METEO_PROVIDER_H
#define AMELTECH_OPEN_METEO_PROVIDER_H

#include <Arduino.h>
#include "Provider.h"
#include "../data/ParameterRegistry.h"

namespace AmelTech {

class OpenMeteoProvider : public Provider {
public:
    OpenMeteoProvider();

    String buildRequestURL(float latitude, float longitude) const override;
    bool parseResponse(const String& jsonBody, ClimateData& out, ErrorCode& errorOut) const override;

    ProviderType getType() const override { return ProviderType::OPEN_METEO; }
    String getName() const override { return "Open-Meteo"; }
    bool isConfigured() const override { return true; } // no key/URL required

    // --- variable selection (spec: setOpenMeteoVariables / reset) ---
    // Controls WHICH of the 20 parameters are requested, not the
    // endpoint itself. Disabling variables here still cannot touch
    // the protected base URL.
    void setOpenMeteoVariables(const bool enabledFlags[(int)ClimateParameter::PARAMETER_COUNT]);
    void resetOpenMeteoVariables();

    // Human-readable summary of current config (base URL + which
    // variables are enabled), for the "provider status" command.
    String getOpenMeteoConfiguration() const;

private:
    // *** THE PROTECTED ENDPOINT ***
    // No public accessor exists for this. It is intentionally private
    // and const so nothing outside this translation unit - and no
    // public method on this class - can reassign it at runtime.
    static const char* const BASE_URL;

    bool _enabledParams[(int)ClimateParameter::PARAMETER_COUNT];

    // Reads one numeric field from the "current" JSON object if
    // present; sets hasX/value only when the key exists and parses.
    bool extractCurrentField(const String& json, const char* key, float& valueOut) const;
    bool extractHourlyFirstField(const String& json, const char* key, float& valueOut) const;
};

} // namespace AmelTech

#endif // AMELTECH_OPEN_METEO_PROVIDER_H
