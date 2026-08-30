/*
 * ClimateEvents.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * Central event enum + lightweight pub/sub dispatcher used across
 * every module (core, location, urls, sensors, cache, platforms).
 */

#ifndef AMELTECH_CLIMATE_EVENTS_H
#define AMELTECH_CLIMATE_EVENTS_H

#include <Arduino.h>
#include <functional>

namespace AmelTech {

enum class Event {
    SYSTEM_STARTED,
    SYSTEM_READY,

    WIFI_CONNECTED,
    WIFI_DISCONNECTED,

    REQUEST_STARTED,
    REQUEST_SUCCESS,
    REQUEST_FAILED,
    REQUEST_TIMEOUT,

    DATA_UPDATED,
    DATA_INVALID,
    DATA_STALE,

    LOCATION_CHANGED,

    URL_ADDED,
    URL_UPDATED,
    URL_DELETED,

    SENSOR_CONNECTED,
    SENSOR_DISCONNECTED,
    SENSOR_ERROR,

    CACHE_UPDATED,
    CACHE_EXPIRED,

    LIVE_STATUS_STARTED,
    LIVE_STATUS_STOPPED,

    PLATFORM_CONNECTED,
    PLATFORM_DISCONNECTED,

    ERROR_OCCURRED
};

const char* eventToString(Event event);

// Payload passed to event listeners. `detail` is a short human-readable
// string (e.g. location name, sensor type); not all events populate it.
struct EventData {
    Event event;
    String detail;
    unsigned long timestamp;
};

typedef std::function<void(const EventData&)> EventCallback;

/*
 * ClimateEvents
 *
 * Simple fixed-capacity listener list (no dynamic allocation growth
 * beyond a bounded array) so it stays predictable on ESP32.
 */
class ClimateEvents {
public:
    static const uint8_t MAX_LISTENERS = 8;

    ClimateEvents();

    // Returns listener slot index, or -1 if full.
    int8_t subscribe(EventCallback callback);
    void unsubscribe(int8_t listenerId);

    void emit(Event event, const String& detail = "");

    uint8_t getListenerCount() const;

private:
    EventCallback _listeners[MAX_LISTENERS];
    bool _active[MAX_LISTENERS];
    uint8_t _count;
};

} // namespace AmelTech

#endif // AMELTECH_CLIMATE_EVENTS_H
