/*
 * ConnectionManager.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * Thin facade that composes WiFiManager + HTTPManager + TLSManager
 * into the single "am I able to make a network request right now"
 * question that RequestManager needs, without RequestManager having
 * to know about all three subsystems individually.
 */

#ifndef AMELTECH_CONNECTION_MANAGER_H
#define AMELTECH_CONNECTION_MANAGER_H

#include <Arduino.h>
#include "WiFiManager.h"
#include "HTTPManager.h"
#include "TLSManager.h"

namespace AmelTech {

class ConnectionManager {
public:
    ConnectionManager();

    void attach(WiFiManager* wifi, HTTPManager* http, TLSManager* tls);

    // True only when WiFi is connected AND (if the URL is https://)
    // TLS is enabled. Does not itself open a socket.
    bool isReady(const String& url) const;

    WiFiManager* wifi() const { return _wifi; }
    HTTPManager* http() const { return _http; }
    TLSManager* tls() const { return _tls; }

private:
    WiFiManager* _wifi;
    HTTPManager* _http;
    TLSManager* _tls;
};

} // namespace AmelTech

#endif // AMELTECH_CONNECTION_MANAGER_H
