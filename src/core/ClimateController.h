/*
 * ClimateController.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * The orchestration layer: on each update() tick, checks the
 * scheduler, and if due AND not already busy, kicks off a
 * RequestManager cycle against the currently-active Provider. On
 * completion, stores the result as the current ClimateData and fires
 * DATA_UPDATED (already emitted by RequestManager) plus advances
 * ClimateState. This is what AmelTechClimate (the public class)
 * actually calls into for update()/updateNow()/request().
 */

#ifndef AMELTECH_CLIMATE_CONTROLLER_H
#define AMELTECH_CLIMATE_CONTROLLER_H

#include <Arduino.h>
#include "ClimateState.h"
#include "ClimateScheduler.h"
#include "ClimateEvents.h"
#include "../providers/ProviderManager.h"
#include "../request/RequestManager.h"
#include "../network/ConnectionManager.h"
#include "../network/RetryManager.h"
#include "../network/HTTPManager.h"
#include "../data/ClimateData.h"
#include "../diagnostics/ErrorManager.h"

namespace AmelTech {

class ClimateController {
public:
    ClimateController();

    void attach(ClimateState* state, ClimateScheduler* scheduler, ClimateEvents* events,
                ProviderManager* providers, ConnectionManager* conn, ErrorManager* errorManager);

    // Call every loop(). Non-blocking: starts a request only if the
    // scheduler says one is due and none is currently in flight.
    void update();

    // Forces an immediate request regardless of scheduler timing.
    // Still non-blocking - just sets things in motion.
    bool updateNow(float latitude, float longitude);

    bool request(float latitude, float longitude); // alias used by public API's request()
    void cancelRequest();

    bool isUpdating() const;

    const ClimateData& getClimateData() const;

    // Sets the coordinates used for scheduled (non-forced) updates.
    // Called by LocationManager whenever the active location changes.
    void setActiveCoordinates(float latitude, float longitude);

private:
    ClimateState* _state;
    ClimateScheduler* _scheduler;
    ClimateEvents* _events;
    ProviderManager* _providers;
    ErrorManager* _errorManager;

    RetryManager _retry;
    RequestManager _requestMgr;

    ClimateData _currentData;
    float _activeLat;
    float _activeLon;
    bool _hasActiveCoordinates;
};

} // namespace AmelTech

#endif // AMELTECH_CLIMATE_CONTROLLER_H
