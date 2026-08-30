/*
 * SecurityManager.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * Top-level facade combining CredentialManager + InputSanitizer +
 * URLValidator into the single entry point AmelTechClimate uses for
 * anything security-adjacent: storing WiFi/platform credentials,
 * sanitizing incoming Serial text, and validating URLs before they
 * reach URLManager/TXManager/RXManager.
 */

#ifndef AMELTECH_SECURITY_MANAGER_H
#define AMELTECH_SECURITY_MANAGER_H

#include <Arduino.h>
#include "CredentialManager.h"
#include "InputSanitizer.h"
#include "../urls/URLValidator.h"
#include "../diagnostics/ErrorManager.h"

namespace AmelTech {

class SecurityManager {
public:
    SecurityManager();

    void attachErrorManager(ErrorManager* errorManager);

    // --- delegated to InputSanitizer ---
    String sanitizeInput(const String& input) const;

    // --- delegated to CredentialManager ---
    bool setCredential(const String& name, const String& value);
    String getCredential(const String& name) const;
    String maskCredential(const String& name) const;
    bool removeCredential(const String& name);

    String protectSensitiveOutput(const String& text) const;

    // --- delegated to URLValidator ---
    bool validateURL(const String& url) const;

    // Convenience wrapper matching spec's exact function name:
    // sanitizes AND validates a credential-like value isn't
    // suspiciously malformed (e.g. contains control characters that
    // suggest a copy-paste error or injection attempt). Does NOT
    // reject based on content strength/complexity - this library
    // doesn't enforce password policy, just structural sanity.
    bool validateCredential(const String& value) const;

private:
    CredentialManager _credentials;
    InputSanitizer _sanitizer;
    URLValidator _urlValidator;
    ErrorManager* _errorManager;
};

} // namespace AmelTech

#endif // AMELTECH_SECURITY_MANAGER_H
