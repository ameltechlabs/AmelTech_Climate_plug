/*
 * CredentialManager.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "CredentialManager.h"

namespace AmelTech {

CredentialManager::CredentialManager() : _errorManager(nullptr) {
    for (uint8_t i = 0; i < MAX_CREDENTIALS; i++) {
        _entries[i].used = false;
    }
}

void CredentialManager::attachErrorManager(ErrorManager* errorManager) {
    _errorManager = errorManager;
}

void CredentialManager::setError(ErrorCode code, const char* msg) {
    if (_errorManager) _errorManager->setError(code, msg);
}

int CredentialManager::findIndex(const String& name) const {
    for (uint8_t i = 0; i < MAX_CREDENTIALS; i++) {
        if (_entries[i].used && _entries[i].name == name) return (int)i;
    }
    return -1;
}

bool CredentialManager::setCredential(const String& name, const String& value) {
    int idx = findIndex(name);
    if (idx >= 0) {
        _entries[idx].value = value;
        return true;
    }

    for (uint8_t i = 0; i < MAX_CREDENTIALS; i++) {
        if (!_entries[i].used) {
            _entries[i].name = name;
            _entries[i].value = value;
            _entries[i].used = true;
            return true;
        }
    }

    setError(ErrorCode::CONFIG_ERROR, "Credential storage full");
    return false;
}

String CredentialManager::getCredential(const String& name) const {
    int idx = findIndex(name);
    return idx >= 0 ? _entries[idx].value : "";
}

bool CredentialManager::removeCredential(const String& name) {
    int idx = findIndex(name);
    if (idx < 0) return false;
    _entries[idx].used = false;
    _entries[idx].value = "";
    return true;
}

bool CredentialManager::hasCredential(const String& name) const {
    return findIndex(name) >= 0;
}

String CredentialManager::maskCredential(const String& name) const {
    int idx = findIndex(name);
    if (idx < 0) return "[not set]";

    const String& value = _entries[idx].value;
    if (value.length() == 0) return "[empty]";

    // Fixed-width mask regardless of actual length, so the mask
    // itself never leaks length information about the credential.
    String masked = "";
    masked += value[0];
    masked += "******";
    return masked;
}

String CredentialManager::protectSensitiveOutput(const String& text) const {
    String result = text;

    for (uint8_t i = 0; i < MAX_CREDENTIALS; i++) {
        if (!_entries[i].used) continue;
        if (_entries[i].value.length() == 0) continue;

        int idx = result.indexOf(_entries[i].value);
        while (idx != -1) {
            String mask = maskCredential(_entries[i].name);
            result = result.substring(0, idx) + mask + result.substring(idx + _entries[i].value.length());
            idx = result.indexOf(_entries[i].value); // re-scan in case of repeats
        }
    }

    return result;
}

} // namespace AmelTech
