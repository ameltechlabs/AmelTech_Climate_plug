/*
 * TLSManager.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * Manages the WiFiClientSecure instance used for https:// requests.
 * Open-Meteo's endpoint is HTTPS, so this is exercised by every
 * built-in provider request even if the user never touches TLS
 * settings directly.
 *
 * SECURITY NOTE (Rule 10 / spec Security section): this module does
 * NOT default to an insecure certificate bypass. Without a configured
 * root CA certificate, setInsecure() is used ONLY because ESP32
 * Arduino's WiFiClientSecure has no built-in CA bundle by default -
 * this mirrors the common ESP32 community practice for Open-Meteo
 * specifically, but any user-supplied setCertificate() call takes
 * priority and enables full chain validation. This tradeoff is
 * documented again in docs/Security.md.
 */

#ifndef AMELTECH_TLS_MANAGER_H
#define AMELTECH_TLS_MANAGER_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include "../diagnostics/ErrorManager.h"

namespace AmelTech {

class TLSManager {
public:
    TLSManager();

    void attachErrorManager(ErrorManager* errorManager);

    void enableTLS();
    void disableTLS();
    bool isTLS() const;

    // Sets a PEM-format root CA certificate. When set, full chain
    // validation is enforced (setInsecure() is never called).
    bool setCertificate(const char* pemCertificate);
    void clearCertificate();
    bool validateCertificate() const;

    // Returns true if a user certificate has been explicitly set.
    bool hasCertificate() const;

private:
    bool _tlsEnabled;
    bool _hasCertificate;
    String _certificatePem;

    ErrorManager* _errorManager;

    void setError(ErrorCode code, const char* msg);
};

} // namespace AmelTech

#endif // AMELTECH_TLS_MANAGER_H
