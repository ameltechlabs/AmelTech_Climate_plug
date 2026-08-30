/*
 * ClimateState.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "ClimateState.h"

namespace AmelTech {

const char* systemStateToString(SystemState state) {
    switch (state) {
        case SystemState::UNINITIALIZED: return "UNINITIALIZED";
        case SystemState::INITIALIZING:  return "INITIALIZING";
        case SystemState::READY:         return "READY";
        case SystemState::UPDATING:      return "UPDATING";
        case SystemState::ERROR:         return "ERROR";
        default:                         return "UNKNOWN";
    }
}

ClimateState::ClimateState() : _state(SystemState::UNINITIALIZED), _bootTime(0) {
}

void ClimateState::setState(SystemState state) {
    if (_state == SystemState::UNINITIALIZED && state == SystemState::INITIALIZING) {
        _bootTime = millis();
    }
    _state = state;
}

SystemState ClimateState::getState() const {
    return _state;
}

bool ClimateState::isInitialized() const {
    return _state != SystemState::UNINITIALIZED;
}

bool ClimateState::isReady() const {
    return _state == SystemState::READY || _state == SystemState::UPDATING;
}

bool ClimateState::isUpdating() const {
    return _state == SystemState::UPDATING;
}

unsigned long ClimateState::getBootTime() const {
    return _bootTime;
}

unsigned long ClimateState::getUptime() const {
    if (_bootTime == 0) return 0;
    return millis() - _bootTime; // unsigned subtraction wraps correctly across millis() rollover
}

} // namespace AmelTech
