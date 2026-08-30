/*
 * TLSManager.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "TLSManager.h"

namespace AmelTech {

TLSManager::TLSManager()
    : _tlsEnabled(true), // Open-Meteo requires HTTPS; TLS on by default
      _hasCertificate(false),
      _certificatePem(""),
      _errorManager(nullptr) {
}

void TLSManager::attachErrorManager(ErrorManager* errorManager) {
    _errorManager = errorManager;
}

void TLSManager::setError(ErrorCode code, const char* msg) {
    if (_errorManager) _errorManager->setError(code, msg);
}

void TLSManager::enableTLS() {
    _tlsEnabled = true;
}

void TLSManager::disableTLS() {
    _tlsEnabled = false;
}

bool TLSManager::isTLS() const {
    return _tlsEnabled;
}

bool TLSManager::setCertificate(const char* pemCertificate) {
    if (pemCertificate == nullptr || strlen(pemCertificate) == 0) {
        setError(ErrorCode::TLS_ERROR, "Empty certificate");
        return false;
    }

    // Minimal sanity check: PEM certs are wrapped in these markers.
    String cert = String(pemCertificate);
    if (cert.indexOf("BEGIN CERTIFICATE") == -1 || cert.indexOf("END CERTIFICATE") == -1) {
        setError(ErrorCode::TLS_ERROR, "Certificate missing PEM markers");
        return false;
    }

    _certificatePem = cert;
    _hasCertificate = true;
    return true;
}

void TLSManager::clearCertificate() {
    _certificatePem = "";
    _hasCertificate = false;
}

bool TLSManager::validateCertificate() const {
    if (!_hasCertificate) return false;
    return _certificatePem.indexOf("BEGIN CERTIFICATE") != -1 &&
           _certificatePem.indexOf("END CERTIFICATE") != -1;
}

bool TLSManager::hasCertificate() const {
    return _hasCertificate;
}

} // namespace AmelTech
