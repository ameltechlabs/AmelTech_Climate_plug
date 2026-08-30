/*
 * URLValidator.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * Validates a candidate custom URL string before URLManager stores
 * it: correct scheme, non-empty host, within the configured max
 * length (spec: "Limit URL lengths"). This is a structural check,
 * not a liveness check - testURL() in URLManager does the actual
 * network reachability test separately.
 */

#ifndef AMELTECH_URL_VALIDATOR_H
#define AMELTECH_URL_VALIDATOR_H

#include <Arduino.h>

namespace AmelTech {

class URLValidator {
public:
    explicit URLValidator(size_t maxLength = 512);

    bool validateURL(const String& url) const;

    // Individual checks, exposed for callers that want the specific
    // failure reason rather than a single bool.
    bool hasValidScheme(const String& url) const;   // http:// or https://
    bool hasHost(const String& url) const;
    bool withinLengthLimit(const String& url) const;
    bool containsForbiddenCharacters(const String& url) const;

    void setMaxLength(size_t maxLength);
    size_t getMaxLength() const;

private:
    size_t _maxLength;
};

} // namespace AmelTech

#endif // AMELTECH_URL_VALIDATOR_H
