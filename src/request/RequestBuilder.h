/*
 * RequestBuilder.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * Given an active Provider and a lat/lon, produces the final request
 * URL. Thin by design - the real per-provider URL logic lives in
 * each Provider::buildRequestURL() implementation; this class exists
 * so RequestManager depends on one stable interface regardless of
 * which provider is active, and so URL-length limiting (spec:
 * "Limit URL lengths") has one enforcement point.
 */

#ifndef AMELTECH_REQUEST_BUILDER_H
#define AMELTECH_REQUEST_BUILDER_H

#include <Arduino.h>
#include "../providers/Provider.h"

namespace AmelTech {

class RequestBuilder {
public:
    RequestBuilder();

    // Returns "" if the built URL would exceed getMaxURLLength() or
    // if provider is null/unconfigured.
    String build(Provider* provider, float latitude, float longitude) const;

    void setMaxURLLength(size_t maxLength);
    size_t getMaxURLLength() const;

private:
    size_t _maxUrlLength;
};

} // namespace AmelTech

#endif // AMELTECH_REQUEST_BUILDER_H
