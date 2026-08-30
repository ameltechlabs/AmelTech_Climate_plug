/*
 * LiveStatus.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * RULE 3: "Live Status is a presentation layer, not an uncontrolled
 * API polling mechanism." This class has its OWN millis()-based
 * refresh timer for how often it reprints to Serial, completely
 * separate from ClimateScheduler's update-request timer. Calling
 * refreshLiveStatus() repeatedly re-displays whatever ClimateData is
 * currently cached in ClimateController - it never itself triggers
 * a network request.
 */

#ifndef AMELTECH_LIVE_STATUS_H
#define AMELTECH_LIVE_STATUS_H

#include <Arduino.h>
#include "SerialDisplay.h"
#include "../data/ClimateData.h"
#include "../core/ClimateEvents.h"

namespace AmelTech {

class LiveStatus {
public:
    LiveStatus();

    void attachEvents(ClimateEvents* events);

    void startLiveStatus();
    void stopLiveStatus();
    bool isLiveStatusRunning() const;

    void enableLiveStatus();  // alias, same as start
    void disableLiveStatus(); // alias, same as stop

    void setLiveStatusInterval(unsigned long ms);
    unsigned long getLiveStatusInterval() const;

    // Call every loop(). Internally checks its own interval timer and
    // prints via SerialDisplay only when due - never faster.
    void refreshLiveStatus(const ClimateData& data,
                            const String& locationName, float latitude, float longitude,
                            const String& providerName, unsigned long staleLifetimeMs);

    // Prints immediately regardless of timer (used by the `status`
    // one-shot command, distinct from the continuous `live status`).
    void printLiveStatus(const ClimateData& data,
                          const String& locationName, float latitude, float longitude,
                          const String& providerName, unsigned long staleLifetimeMs) const;

    void clearLiveDisplay() const;

private:
    SerialDisplay _display;
    ClimateEvents* _events;

    bool _running;
    unsigned long _intervalMs;
    unsigned long _lastRefreshAt;
};

} // namespace AmelTech

#endif // AMELTECH_LIVE_STATUS_H
