/*
 * RequestManager.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * Drives a single climate-data request from start to finish:
 * RequestBuilder -> HTTPManager -> ResponseManager -> RetryManager
 * on failure. Exposes a poll-style non-blocking interface
 * (startRequest() / update() / isBusy() / getResult()) so
 * ClimateController can call this from loop() without stalling.
 *
 * IMPORTANT: HTTPManager::GET() itself blocks for the duration of one
 * TCP request (bounded by its timeout settings - see HTTPManager.h).
 * This class does not attempt to make that individual call
 * asynchronous (ESP32 Arduino's HTTPClient has no non-blocking mode),
 * but it DOES ensure the library never calls it back-to-back in a
 * blocking retry spin: retries wait for RetryManager::shouldRetryNow()
 * before the next attempt, checked once per update() call, so control
 * returns to the user's loop() between every single attempt.
 */

#ifndef AMELTECH_REQUEST_MANAGER_H
#define AMELTECH_REQUEST_MANAGER_H

#include <Arduino.h>
#include "RequestBuilder.h"
#include "ResponseManager.h"
#include "../network/HTTPManager.h"
#include "../network/ConnectionManager.h"
#include "../network/RetryManager.h"
#include "../providers/Provider.h"
#include "../data/ClimateData.h"
#include "../diagnostics/ErrorManager.h"
#include "../core/ClimateEvents.h"

namespace AmelTech {

enum class RequestState {
    IDLE,
    WAITING_FOR_CONNECTION,
    IN_FLIGHT,
    WAITING_TO_RETRY,
    SUCCESS,
    FAILED
};

class RequestManager {
public:
    RequestManager();

    void attach(HTTPManager* http, ConnectionManager* conn, RetryManager* retry,
                ErrorManager* errorManager, ClimateEvents* events);

    // Begins a new request cycle. No-op (returns false) if already busy.
    bool startRequest(Provider* provider, float latitude, float longitude);

    void cancelRequest();

    // Call every loop(). Advances the state machine by at most one
    // HTTP attempt per call.
    void update();

    bool isBusy() const;
    RequestState getState() const;

    // Valid only after getState() == SUCCESS.
    const ClimateData& getResult() const;

private:
    HTTPManager* _http;
    ConnectionManager* _conn;
    RetryManager* _retry;
    ErrorManager* _errorManager;
    ClimateEvents* _events;

    RequestBuilder _builder;
    ResponseManager _responseMgr;

    RequestState _state;
    Provider* _activeProvider;
    float _lat;
    float _lon;
    String _url;
    ClimateData _result;

    void attempt();
    void setError(ErrorCode code, const char* msg);
    void emit(Event event, const String& detail = "");
};

} // namespace AmelTech

#endif // AMELTECH_REQUEST_MANAGER_H
