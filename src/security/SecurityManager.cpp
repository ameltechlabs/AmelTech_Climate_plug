/*
 * SecurityManager.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "SecurityManager.h"

namespace AmelTech {

SecurityManager::SecurityManager() : _errorManager(nullptr) {
}

void SecurityManager::attachErrorManager(ErrorManager* errorManager) {
    _errorManager = errorManager;
    _credentials.attachErrorManager(errorManager);
}

String SecurityManager::sanitizeInput(const String& input) const {
    return _sanitizer.clean(input);
}

bool SecurityManager::setCredential(const String& name, const String& value) {
    return _credentials.setCredential(name, value);
}

String SecurityManager::getCredential(const String& name) const {
    return _credentials.getCredential(name);
}

String SecurityManager::maskCredential(const String& name) const {
    return _credentials.maskCredential(name);
}

bool SecurityManager::removeCredential(const String& name) {
    return _credentials.removeCredential(name);
}

String SecurityManager::protectSensitiveOutput(const String& text) const {
    return _credentials.protectSensitiveOutput(text);
}

bool SecurityManager::validateURL(const String& url) const {
    return _urlValidator.validateURL(url);
}

bool SecurityManager::validateCredential(const String& value) const {
    if (value.length() == 0) return false;
    return _sanitizer.isSafeForSerial(value); // rejects embedded newlines/CR, a copy-paste/injection red flag
}

} // namespace AmelTech
