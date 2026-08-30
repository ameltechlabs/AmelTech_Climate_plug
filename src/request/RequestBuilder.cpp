/*
 * RequestBuilder.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "RequestBuilder.h"

namespace AmelTech {

RequestBuilder::RequestBuilder() : _maxUrlLength(1024) {
}

String RequestBuilder::build(Provider* provider, float latitude, float longitude) const {
    if (provider == nullptr || !provider->isConfigured()) return "";

    String url = provider->buildRequestURL(latitude, longitude);
    if (url.length() == 0) return "";
    if (url.length() > _maxUrlLength) return "";

    return url;
}

void RequestBuilder::setMaxURLLength(size_t maxLength) {
    _maxUrlLength = maxLength;
}

size_t RequestBuilder::getMaxURLLength() const {
    return _maxUrlLength;
}

} // namespace AmelTech
