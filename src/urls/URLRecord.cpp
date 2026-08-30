/*
 * URLRecord.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "URLRecord.h"

namespace AmelTech {

const char* urlDirectionToString(URLDirection d) {
    switch (d) {
        case URLDirection::TX:    return "TX";
        case URLDirection::RX:    return "RX";
        case URLDirection::TX_RX: return "TX_RX";
        default:                  return "UNKNOWN";
    }
}

const char* urlMethodToString(URLMethod m) {
    switch (m) {
        case URLMethod::GET:   return "GET";
        case URLMethod::POST:  return "POST";
        case URLMethod::PUT:   return "PUT";
        case URLMethod::PATCH: return "PATCH";
        default:                return "UNKNOWN";
    }
}

const char* urlFormatToString(URLFormat f) {
    switch (f) {
        case URLFormat::JSON: return "JSON";
        case URLFormat::TEXT: return "TEXT";
        case URLFormat::RAW:  return "RAW";
        case URLFormat::AUTO: return "AUTO";
        default:               return "UNKNOWN";
    }
}

URLRecord::URLRecord()
    : id(-1), name(""), url(""), direction(URLDirection::TX),
      method(URLMethod::GET), format(URLFormat::JSON), enabled(false) {
}

URLRecord::URLRecord(int id, const String& name, const String& url,
                      URLDirection direction, URLMethod method, URLFormat format)
    : id(id), name(name), url(url), direction(direction), method(method), format(format), enabled(true) {
}

} // namespace AmelTech
