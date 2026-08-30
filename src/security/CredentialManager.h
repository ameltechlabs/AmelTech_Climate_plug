/*
 * CredentialManager.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * Holds sensitive strings (WiFi password, MQTT password, Blynk auth
 * token, etc.) behind an interface that makes it structurally hard
 * to accidentally print them - getCredential() returns the real
 * value (callers that genuinely need it, like WiFiManager::connectWiFi,
 * use this), but maskCredential() is what SerialDisplay/HelpSystem/
 * any status-printing code should call instead.
 */

#ifndef AMELTECH_CREDENTIAL_MANAGER_H
#define AMELTECH_CREDENTIAL_MANAGER_H

#include <Arduino.h>
#include "../diagnostics/ErrorManager.h"

namespace AmelTech {

class CredentialManager {
public:
    static const uint8_t MAX_CREDENTIALS = 8;

    CredentialManager();

    void attachErrorManager(ErrorManager* errorManager);

    // Stores/overwrites a named credential (e.g. "wifi_password",
    // "mqtt_password", "blynk_token").
    bool setCredential(const String& name, const String& value);

    // Returns the RAW value. Callers must have a genuine functional
    // need (establishing a connection) - never call this to display
    // something to the user.
    String getCredential(const String& name) const;

    bool removeCredential(const String& name);
    bool hasCredential(const String& name) const;

    // Returns a masked representation safe for Serial/logs: shows
    // only the first character (if any) plus a fixed run of asterisks,
    // never the actual length or content beyond that.
    String maskCredential(const String& name) const;

    // Given an arbitrary status string that MIGHT contain a raw
    // credential value verbatim (e.g. built by string concatenation
    // elsewhere), replaces any occurrence of a stored credential's
    // real value with its masked form. Defense-in-depth for status/
    // diagnostic strings assembled outside this class.
    String protectSensitiveOutput(const String& text) const;

private:
    struct CredentialEntry {
        String name;
        String value;
        bool used;
    };

    CredentialEntry _entries[MAX_CREDENTIALS];
    ErrorManager* _errorManager;

    int findIndex(const String& name) const;
    void setError(ErrorCode code, const char* msg);
};

} // namespace AmelTech

#endif // AMELTECH_CREDENTIAL_MANAGER_H
