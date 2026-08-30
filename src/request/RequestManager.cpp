/*
 * RequestManager.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "RequestManager.h"

namespace AmelTech {

RequestManager::RequestManager()
    : _http(nullptr), _conn(nullptr), _retry(nullptr), _errorManager(nullptr), _events(nullptr),
      _state(RequestState::IDLE), _activeProvider(nullptr), _lat(0), _lon(0), _url("") {
}

void RequestManager::attach(HTTPManager* http, ConnectionManager* conn, RetryManager* retry,
                             ErrorManager* errorManager, ClimateEvents* events) {
    _http = http;
    _conn = conn;
    _retry = retry;
    _errorManager = errorManager;
    _events = events;
}

void RequestManager::setError(ErrorCode code, const char* msg) {
    if (_errorManager) _errorManager->setError(code, msg);
}

void RequestManager::emit(Event event, const String& detail) {
    if (_events) _events->emit(event, detail);
}

bool RequestManager::startRequest(Provider* provider, float latitude, float longitude) {
    if (isBusy()) return false;
    if (provider == nullptr) {
        setError(ErrorCode::PROVIDER_ERROR, "No provider set");
        return false;
    }

    _activeProvider = provider;
    _lat = latitude;
    _lon = longitude;

    _url = _builder.build(provider, latitude, longitude);
    if (_url.length() == 0) {
        setError(ErrorCode::URL_INVALID, "RequestBuilder produced empty/oversized URL");
        _state = RequestState::FAILED;
        return false;
    }

    if (_retry) _retry->resetRetryState();
    _state = RequestState::WAITING_FOR_CONNECTION;
    emit(Event::REQUEST_STARTED, _url);
    return true;
}

void RequestManager::cancelRequest() {
    _state = RequestState::IDLE;
    _activeProvider = nullptr;
    _url = "";
}

void RequestManager::attempt() {
    if (_http == nullptr) {
        setError(ErrorCode::HTTP_ERROR, "No HTTPManager attached");
        _state = RequestState::FAILED;
        return;
    }

    HTTPResponse resp = _http->GET(_url);

    if (!resp.success) {
        bool willRetry = _retry ? _retry->retryRequest() : false;
        if (willRetry) {
            _state = RequestState::WAITING_TO_RETRY;
        } else {
            setError(resp.error, "Request failed, retries exhausted");
            emit(Event::REQUEST_FAILED, errorCodeToString(resp.error));
            _state = RequestState::FAILED;
        }
        return;
    }

    ErrorCode parseErr;
    if (!_responseMgr.process(_activeProvider, resp.body, _result, parseErr)) {
        bool willRetry = _retry ? _retry->retryRequest() : false;
        if (willRetry) {
            _state = RequestState::WAITING_TO_RETRY;
        } else {
            setError(parseErr, "Response processing failed, retries exhausted");
            emit(Event::REQUEST_FAILED, errorCodeToString(parseErr));
            _state = RequestState::FAILED;
        }
        return;
    }

    _state = RequestState::SUCCESS;
    emit(Event::REQUEST_SUCCESS, _url);
    emit(Event::DATA_UPDATED);
}

void RequestManager::update() {
    switch (_state) {
        case RequestState::WAITING_FOR_CONNECTION:
            if (_conn && _conn->isReady(_url)) {
                _state = RequestState::IN_FLIGHT;
                attempt();
            }
            // else: stay here, checked again next update() - non-blocking wait
            break;

        case RequestState::IN_FLIGHT:
            // attempt() is synchronous once called (see class header note);
            // state has already advanced past IN_FLIGHT by the time we'd
            // re-enter here, so this case is effectively transient.
            break;

        case RequestState::WAITING_TO_RETRY:
            if (_retry && _retry->shouldRetryNow()) {
                _state = RequestState::IN_FLIGHT;
                attempt();
            }
            break;

        case RequestState::IDLE:
        case RequestState::SUCCESS:
        case RequestState::FAILED:
        default:
            break; // nothing to do until startRequest() is called again
    }
}

bool RequestManager::isBusy() const {
    return _state == RequestState::WAITING_FOR_CONNECTION ||
           _state == RequestState::IN_FLIGHT ||
           _state == RequestState::WAITING_TO_RETRY;
}

RequestState RequestManager::getState() const {
    return _state;
}

const ClimateData& RequestManager::getResult() const {
    return _result;
}

} // namespace AmelTech
