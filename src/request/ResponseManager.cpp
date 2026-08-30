/*
 * ResponseManager.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "ResponseManager.h"

namespace AmelTech {

ResponseManager::ResponseManager() {
}

bool ResponseManager::process(Provider* provider, const String& rawBody, ClimateData& out, ErrorCode& errorOut) {
    out.reset();

    if (provider == nullptr) {
        errorOut = ErrorCode::PROVIDER_ERROR;
        return false;
    }

    if (rawBody.length() == 0) {
        errorOut = ErrorCode::RESPONSE_ERROR;
        return false;
    }

    // PARSER
    if (!provider->parseResponse(rawBody, out, errorOut)) {
        return false; // errorOut already set by the provider
    }

    // NORMALIZER (runs before validation so unit-corrected values are
    // what gets range-checked)
    _normalizer.normalize(out);

    // VALIDATOR
    if (!_validator.validateClimateData(out)) {
        errorOut = ErrorCode::DATA_INVALID;
        out.valid = false;
        return false;
    }

    out.valid = true;
    out.stale = false;
    errorOut = ErrorCode::OK;
    return true;
}

} // namespace AmelTech
