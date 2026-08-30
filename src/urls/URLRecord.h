/*
 * URLRecord.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * Plain data struct for one custom URL entry, matching spec's
 * "Create URLRecord: id, name, url, direction, method, format,
 * enabled".
 */

#ifndef AMELTECH_URL_RECORD_H
#define AMELTECH_URL_RECORD_H

#include <Arduino.h>

namespace AmelTech {

enum class URLDirection {
    TX,     // outbound only
    RX,     // inbound only
    TX_RX   // both
};

enum class URLMethod {
    GET,
    POST,
    PUT,
    PATCH
};

enum class URLFormat {
    JSON,
    TEXT,
    RAW,
    AUTO
};

const char* urlDirectionToString(URLDirection d);
const char* urlMethodToString(URLMethod m);
const char* urlFormatToString(URLFormat f);

struct URLRecord {
    int id;
    String name;
    String url;
    URLDirection direction;
    URLMethod method;
    URLFormat format;
    bool enabled;

    URLRecord();
    URLRecord(int id, const String& name, const String& url,
              URLDirection direction = URLDirection::TX,
              URLMethod method = URLMethod::GET,
              URLFormat format = URLFormat::JSON);
};

} // namespace AmelTech

#endif // AMELTECH_URL_RECORD_H
