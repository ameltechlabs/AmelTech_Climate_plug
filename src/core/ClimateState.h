/*
 * ClimateState.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * Tracks the library's overall lifecycle state, backing
 * AmelTechClimate::getState()/isInitialized()/isReady()/isUpdating().
 * Kept separate from ClimateController so "what phase are we in" is
 * queryable independent of the controller's internal request logic.
 */

#ifndef AMELTECH_CLIMATE_STATE_H
#define AMELTECH_CLIMATE_STATE_H

#include <Arduino.h>

namespace AmelTech {

enum class SystemState {
    UNINITIALIZED,
    INITIALIZING,
    READY,
    UPDATING,
    ERROR
};

const char* systemStateToString(SystemState state);

class ClimateState {
public:
    ClimateState();

    void setState(SystemState state);
    SystemState getState() const;

    bool isInitialized() const;
    bool isReady() const;
    bool isUpdating() const;

    unsigned long getBootTime() const;
    unsigned long getUptime() const; // millis() since boot, wraps safely

private:
    SystemState _state;
    unsigned long _bootTime;
};

} // namespace AmelTech

#endif // AMELTECH_CLIMATE_STATE_H
