/*
 * ResponseManager.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * Runs a raw HTTP response body through Provider::parseResponse(),
 * then DataValidator, then DataNormalizer, producing a final
 * ClimateData ready to hand to CacheManager/ClimateController.
 * This is the PARSER -> VALIDATOR -> NORMALIZER stage of the spec's
 * data pipeline diagram.
 */

#ifndef AMELTECH_RESPONSE_MANAGER_H
#define AMELTECH_RESPONSE_MANAGER_H

#include <Arduino.h>
#include "../providers/Provider.h"
#include "../data/ClimateData.h"
#include "../data/DataValidator.h"
#include "../data/DataNormalizer.h"
#include "../diagnostics/ErrorManager.h"

namespace AmelTech {

class ResponseManager {
public:
    ResponseManager();

    // Full pipeline: parse -> validate -> normalize. Returns true if
    // the result is usable (parsed AND validated); `out` is always
    // reset first, so a false return leaves `out` in a clean
    // "unavailable" state rather than partially populated.
    bool process(Provider* provider, const String& rawBody, ClimateData& out, ErrorCode& errorOut);

private:
    DataValidator _validator;
    DataNormalizer _normalizer;
};

} // namespace AmelTech

#endif // AMELTECH_RESPONSE_MANAGER_H
