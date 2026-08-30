/*
 * ClimateScheduler.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "ClimateScheduler.h"

namespace AmelTech {

ClimateScheduler::ClimateScheduler()
    : _intervalMs(600000), // 10 minutes - a reasonable default that respects Open-Meteo fair use
      _lastUpdateAt(0),
      _running(false) {
}

void ClimateScheduler::setUpdateInterval(unsigned long ms) {
    _intervalMs = ms;
}

unsigned long ClimateScheduler::getUpdateInterval() const {
    return _intervalMs;
}

void ClimateScheduler::startScheduler() {
    _running = true;
    // Force an update to be "due" immediately on start, rather than
    // waiting a full interval before the first-ever request.
    _lastUpdateAt = millis() - _intervalMs;
}

void ClimateScheduler::stopScheduler() {
    _running = false;
}

bool ClimateScheduler::isSchedulerRunning() const {
    return _running;
}

void ClimateScheduler::scheduleUpdate() {
    _lastUpdateAt = millis();
}

bool ClimateScheduler::shouldUpdate() const {
    if (!_running) return false;
    return (millis() - _lastUpdateAt) >= _intervalMs;
}

unsigned long ClimateScheduler::timeUntilNextUpdate() const {
    if (!_running) return 0;
    unsigned long elapsed = millis() - _lastUpdateAt;
    if (elapsed >= _intervalMs) return 0;
    return _intervalMs - elapsed;
}

} // namespace AmelTech
