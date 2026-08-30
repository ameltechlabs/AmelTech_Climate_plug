/*
 * RXManager.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * Fetches data from a configured RX URLRecord using HTTPManager and
 * buffers the most recent result for readReceivedData(). Implements
 * spec's RX functions: receive/receiveJSON/receiveText/receiveRaw,
 * hasReceivedData/readReceivedData/clearReceivedData.
 */

#ifndef AMELTECH_RX_MANAGER_H
#define AMELTECH_RX_MANAGER_H

#include <Arduino.h>
#include "URLRecord.h"
#include "../network/HTTPManager.h"
#include "../diagnostics/ErrorManager.h"

namespace AmelTech {

class RXManager {
public:
    RXManager();

    void attachHTTP(HTTPManager* http);
    void attachErrorManager(ErrorManager* errorManager);

    void setRXURL(const String& url);
    String getRXURL() const;

    void setRXMethod(URLMethod method);
    URLMethod getRXMethod() const;

    void setRXFormat(URLFormat format);
    URLFormat getRXFormat() const;

    bool isRXEnabled() const;
    void enableRX();
    void disableRX();

    // Performs the actual HTTP fetch and buffers the body. Returns
    // true on success. This call is synchronous/blocking for the
    // duration of one HTTP request (bounded by HTTPManager's
    // timeouts) - callers that need non-blocking RX should invoke
    // this from a scheduled point, the same caution as RequestManager.
    bool receive();
    bool receiveData() { return receive(); }
    bool receiveJSON() { return receive(); }
    bool receiveText() { return receive(); }
    bool receiveRaw() { return receive(); }

    bool hasReceivedData() const;
    String readReceivedData() const;
    void clearReceivedData();

private:
    HTTPManager* _http;
    ErrorManager* _errorManager;

    String _url;
    URLMethod _method;
    URLFormat _format;
    bool _enabled;

    String _buffer;
    bool _hasData;

    void setError(ErrorCode code, const char* msg);
};

} // namespace AmelTech

#endif // AMELTECH_RX_MANAGER_H
