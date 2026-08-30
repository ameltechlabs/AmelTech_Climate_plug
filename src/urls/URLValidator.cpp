/*
 * URLValidator.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "URLValidator.h"

namespace AmelTech {

URLValidator::URLValidator(size_t maxLength) : _maxLength(maxLength) {
}

bool URLValidator::hasValidScheme(const String& url) const {
    return url.startsWith("http://") || url.startsWith("https://");
}

bool URLValidator::hasHost(const String& url) const {
    int schemeEnd = url.indexOf("://");
    if (schemeEnd == -1) return false;

    String afterScheme = url.substring(schemeEnd + 3);
    if (afterScheme.length() == 0) return false;

    // Host is everything up to the next '/', '?', or ':' (port).
    int hostEnd = afterScheme.length();
    int slashPos = afterScheme.indexOf('/');
    int queryPos = afterScheme.indexOf('?');
    int portPos = afterScheme.indexOf(':');

    if (slashPos != -1 && slashPos < hostEnd) hostEnd = slashPos;
    if (queryPos != -1 && queryPos < hostEnd) hostEnd = queryPos;
    if (portPos != -1 && portPos < hostEnd) hostEnd = portPos;

    String host = afterScheme.substring(0, hostEnd);
    return host.length() > 0;
}

bool URLValidator::withinLengthLimit(const String& url) const {
    return url.length() > 0 && url.length() <= _maxLength;
}

bool URLValidator::containsForbiddenCharacters(const String& url) const {
    // Reject raw whitespace and control characters, which have no
    // legitimate place in a URL and could indicate injection attempts
    // or copy-paste errors.
    for (size_t i = 0; i < url.length(); i++) {
        char c = url[i];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || (unsigned char)c < 0x20) {
            return true;
        }
    }
    return false;
}

bool URLValidator::validateURL(const String& url) const {
    if (!withinLengthLimit(url)) return false;
    if (containsForbiddenCharacters(url)) return false;
    if (!hasValidScheme(url)) return false;
    if (!hasHost(url)) return false;
    return true;
}

void URLValidator::setMaxLength(size_t maxLength) {
    _maxLength = maxLength;
}

size_t URLValidator::getMaxLength() const {
    return _maxLength;
}

} // namespace AmelTech
