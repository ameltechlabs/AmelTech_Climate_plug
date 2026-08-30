/*
 * ClimateController.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "ClimateController.h"

namespace AmelTech {

ClimateController::ClimateController()
    : _state(nullptr), _scheduler(nullptr), _events(nullptr), _providers(nullptr), _errorManager(nullptr),
      _activeLat(0), _activeLon(0), _hasActiveCoordinates(false) {
}

void ClimateController::attach(ClimateState* state, ClimateScheduler* scheduler, ClimateEvents* events,
                                ProviderManager* providers, ConnectionManager* conn, ErrorManager* errorManager) {
    _state = state;
    _scheduler = scheduler;
    _events = events;
    _providers = providers;
    _errorManager = errorManager;

    // RequestManager needs an HTTPManager - ConnectionManager already
    // holds one, so we reuse that instance rather than constructing
    // a second HTTPManager.
    _requestMgr.attach(conn ? conn->http() : nullptr, conn, &_retry, errorManager, events);
}

void ClimateController::setActiveCoordinates(float latitude, float longitude) {
    _activeLat = latitude;
    _activeLon = longitude;
    _hasActiveCoordinates = true;
}

void ClimateController::update() {
    // Drive any in-flight request forward first.
    _requestMgr.update();

    if (_requestMgr.getState() == RequestState::SUCCESS) {
        _currentData = _requestMgr.getResult();
        if (_scheduler) _scheduler->scheduleUpdate();
        if (_state) _state->setState(SystemState::READY);
        _requestMgr.cancelRequest(); // consume the result, return to IDLE
    } else if (_requestMgr.getState() == RequestState::FAILED) {
        if (_scheduler) _scheduler->scheduleUpdate(); // still reschedule - don't hammer retries forever
        if (_state) _state->setState(SystemState::READY); // READY, not stuck in ERROR - stale cached data may still be usable
        _requestMgr.cancelRequest();
    }

    // Only auto-start a new request if nothing is in flight and the
    // scheduler says one is due.
    if (!_requestMgr.isBusy() && _scheduler && _scheduler->shouldUpdate() && _hasActiveCoordinates) {
        updateNow(_activeLat, _activeLon);
    }
}

bool ClimateController::updateNow(float latitude, float longitude) {
    if (_requestMgr.isBusy()) return false;
    if (_providers == nullptr) return false;

    if (_state) _state->setState(SystemState::UPDATING);

    Provider* provider = _providers->getProvider();
    bool started = _requestMgr.startRequest(provider, latitude, longitude);

    if (!started && _state) {
        _state->setState(SystemState::READY); // didn't actually start; don't leave state stuck UPDATING
    }
    return started;
}

bool ClimateController::request(float latitude, float longitude) {
    return updateNow(latitude, longitude);
}

void ClimateController::cancelRequest() {
    _requestMgr.cancelRequest();
    if (_state) _state->setState(SystemState::READY);
}

bool ClimateController::isUpdating() const {
    return _requestMgr.isBusy();
}

const ClimateData& ClimateController::getClimateData() const {
    return _currentData;
}

} // namespace AmelTech
