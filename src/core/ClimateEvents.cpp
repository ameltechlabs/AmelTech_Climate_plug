/*
 * ClimateEvents.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "ClimateEvents.h"

namespace AmelTech {

const char* eventToString(Event event) {
    switch (event) {
        case Event::SYSTEM_STARTED:        return "SYSTEM_STARTED";
        case Event::SYSTEM_READY:          return "SYSTEM_READY";
        case Event::WIFI_CONNECTED:        return "WIFI_CONNECTED";
        case Event::WIFI_DISCONNECTED:     return "WIFI_DISCONNECTED";
        case Event::REQUEST_STARTED:       return "REQUEST_STARTED";
        case Event::REQUEST_SUCCESS:       return "REQUEST_SUCCESS";
        case Event::REQUEST_FAILED:        return "REQUEST_FAILED";
        case Event::REQUEST_TIMEOUT:       return "REQUEST_TIMEOUT";
        case Event::DATA_UPDATED:          return "DATA_UPDATED";
        case Event::DATA_INVALID:          return "DATA_INVALID";
        case Event::DATA_STALE:            return "DATA_STALE";
        case Event::LOCATION_CHANGED:      return "LOCATION_CHANGED";
        case Event::URL_ADDED:             return "URL_ADDED";
        case Event::URL_UPDATED:           return "URL_UPDATED";
        case Event::URL_DELETED:           return "URL_DELETED";
        case Event::SENSOR_CONNECTED:      return "SENSOR_CONNECTED";
        case Event::SENSOR_DISCONNECTED:   return "SENSOR_DISCONNECTED";
        case Event::SENSOR_ERROR:          return "SENSOR_ERROR";
        case Event::CACHE_UPDATED:         return "CACHE_UPDATED";
        case Event::CACHE_EXPIRED:         return "CACHE_EXPIRED";
        case Event::LIVE_STATUS_STARTED:   return "LIVE_STATUS_STARTED";
        case Event::LIVE_STATUS_STOPPED:   return "LIVE_STATUS_STOPPED";
        case Event::PLATFORM_CONNECTED:    return "PLATFORM_CONNECTED";
        case Event::PLATFORM_DISCONNECTED: return "PLATFORM_DISCONNECTED";
        case Event::ERROR_OCCURRED:        return "ERROR_OCCURRED";
        default:                           return "UNKNOWN_EVENT";
    }
}

ClimateEvents::ClimateEvents() : _count(0) {
    for (uint8_t i = 0; i < MAX_LISTENERS; i++) {
        _active[i] = false;
    }
}

int8_t ClimateEvents::subscribe(EventCallback callback) {
    for (uint8_t i = 0; i < MAX_LISTENERS; i++) {
        if (!_active[i]) {
            _listeners[i] = callback;
            _active[i] = true;
            _count++;
            return (int8_t)i;
        }
    }
    return -1; // full
}

void ClimateEvents::unsubscribe(int8_t listenerId) {
    if (listenerId < 0 || listenerId >= (int8_t)MAX_LISTENERS) return;
    if (_active[listenerId]) {
        _active[listenerId] = false;
        _count--;
    }
}

void ClimateEvents::emit(Event event, const String& detail) {
    EventData data;
    data.event = event;
    data.detail = detail;
    data.timestamp = millis();

    for (uint8_t i = 0; i < MAX_LISTENERS; i++) {
        if (_active[i] && _listeners[i]) {
            _listeners[i](data);
        }
    }
}

uint8_t ClimateEvents::getListenerCount() const {
    return _count;
}

} // namespace AmelTech
