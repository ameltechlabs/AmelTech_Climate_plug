/*
 * LiveStatus.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "LiveStatus.h"

namespace AmelTech {

LiveStatus::LiveStatus()
    : _events(nullptr), _running(false), _intervalMs(5000), _lastRefreshAt(0) {
}

void LiveStatus::attachEvents(ClimateEvents* events) {
    _events = events;
}

void LiveStatus::startLiveStatus() {
    _running = true;
    _lastRefreshAt = 0; // force an immediate refresh on next loop() tick
    if (_events) _events->emit(Event::LIVE_STATUS_STARTED);
}

void LiveStatus::stopLiveStatus() {
    _running = false;
    if (_events) _events->emit(Event::LIVE_STATUS_STOPPED);
    // Per spec: does NOT touch climate data, cache, WiFi, scheduler,
    // sensors, or platform integrations - this function intentionally
    // does nothing else.
}

bool LiveStatus::isLiveStatusRunning() const {
    return _running;
}

void LiveStatus::enableLiveStatus() {
    startLiveStatus();
}

void LiveStatus::disableLiveStatus() {
    stopLiveStatus();
}

void LiveStatus::setLiveStatusInterval(unsigned long ms) {
    _intervalMs = ms;
}

unsigned long LiveStatus::getLiveStatusInterval() const {
    return _intervalMs;
}

void LiveStatus::refreshLiveStatus(const ClimateData& data,
                                    const String& locationName, float latitude, float longitude,
                                    const String& providerName, unsigned long staleLifetimeMs) {
    if (!_running) return;
    if (millis() - _lastRefreshAt < _intervalMs) return;

    printLiveStatus(data, locationName, latitude, longitude, providerName, staleLifetimeMs);
    _lastRefreshAt = millis();
}

void LiveStatus::printLiveStatus(const ClimateData& data,
                                  const String& locationName, float latitude, float longitude,
                                  const String& providerName, unsigned long staleLifetimeMs) const {
    Serial.print(_display.renderStatus(data, locationName, latitude, longitude, providerName, _running, staleLifetimeMs));
}

void LiveStatus::clearLiveDisplay() const {
    // No true "clear screen" on a plain Serial Monitor; print a
    // visual separator instead so successive frames are distinguishable.
    Serial.println("\n\n----- (display refresh) -----\n");
}

} // namespace AmelTech
