/*
 * URLManager.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "URLManager.h"
#include "../providers/ProviderManager.h"

namespace AmelTech {

URLManager::URLManager()
    : _count(0), _activeId(-1), _nextId(1), _errorManager(nullptr), _events(nullptr) {
}

void URLManager::attachErrorManager(ErrorManager* errorManager) {
    _errorManager = errorManager;
}

void URLManager::attachEvents(ClimateEvents* events) {
    _events = events;
}

void URLManager::setError(ErrorCode code, const char* msg) {
    if (_errorManager) _errorManager->setError(code, msg);
}

int URLManager::findIndexById(int id) const {
    for (uint8_t i = 0; i < _count; i++) {
        if (_records[i].id == id) return (int)i;
    }
    return -1;
}

bool URLManager::isProtectedName(const String& name) {
    String lower = name;
    lower.toLowerCase();
    return lower == "open-meteo" || lower == "openmeteo" || lower == "open_meteo" || lower == "meteo";
}

int URLManager::addURL(const String& name, const String& url, URLDirection direction, URLMethod method, URLFormat format) {
    if (isProtectedName(name)) {
        setError(ErrorCode::URL_ERROR, ProviderManager::protectedModificationMessage().c_str());
        return -1;
    }

    if (!_validator.validateURL(url)) {
        setError(ErrorCode::URL_INVALID, "URL failed structural validation (scheme/host/length)");
        return -1;
    }

    if (name.length() == 0) {
        setError(ErrorCode::URL_INVALID, "URL name cannot be empty");
        return -1;
    }

    if (_count >= MAX_URLS) {
        setError(ErrorCode::CONFIG_ERROR, "Maximum stored custom URLs reached");
        return -1;
    }

    int newId = _nextId++;
    _records[_count] = URLRecord(newId, name, url, direction, method, format);
    _count++;

    if (_events) _events->emit(Event::URL_ADDED, name);
    return newId;
}

bool URLManager::editURL(int id, const String& name, const String& url) {
    if (isProtectedName(name)) {
        setError(ErrorCode::URL_ERROR, ProviderManager::protectedModificationMessage().c_str());
        return false;
    }

    int idx = findIndexById(id);
    if (idx < 0) {
        setError(ErrorCode::URL_ERROR, "URL id not found");
        return false;
    }

    if (!_validator.validateURL(url)) {
        setError(ErrorCode::URL_INVALID, "URL failed structural validation");
        return false;
    }

    _records[idx].name = name;
    _records[idx].url = url;

    if (_events) _events->emit(Event::URL_UPDATED, name);
    return true;
}

bool URLManager::deleteURL(int id) {
    int idx = findIndexById(id);
    if (idx < 0) {
        setError(ErrorCode::URL_ERROR, "URL id not found");
        return false;
    }

    String name = _records[idx].name;

    for (uint8_t i = idx; i < _count - 1; i++) {
        _records[i] = _records[i + 1];
    }
    _count--;

    if (_activeId == id) _activeId = -1;

    if (_events) _events->emit(Event::URL_DELETED, name);
    return true;
}

bool URLManager::renameURL(int id, const String& newName) {
    if (isProtectedName(newName)) {
        setError(ErrorCode::URL_ERROR, ProviderManager::protectedModificationMessage().c_str());
        return false;
    }

    int idx = findIndexById(id);
    if (idx < 0) {
        setError(ErrorCode::URL_ERROR, "URL id not found");
        return false;
    }

    if (newName.length() == 0) {
        setError(ErrorCode::URL_INVALID, "Name cannot be empty");
        return false;
    }

    _records[idx].name = newName;
    if (_events) _events->emit(Event::URL_UPDATED, newName);
    return true;
}

bool URLManager::selectURL(int id) {
    int idx = findIndexById(id);
    if (idx < 0) {
        setError(ErrorCode::URL_ERROR, "URL id not found");
        return false;
    }
    _activeId = id;
    return true;
}

const URLRecord* URLManager::getActiveURL() const {
    if (_activeId < 0) return nullptr;
    int idx = findIndexById(_activeId);
    if (idx < 0) return nullptr;
    return &_records[idx];
}

uint8_t URLManager::listURLs(URLRecord outBuffer[], uint8_t maxCount) const {
    uint8_t n = (_count < maxCount) ? _count : maxCount;
    for (uint8_t i = 0; i < n; i++) outBuffer[i] = _records[i];
    return n;
}

uint8_t URLManager::getURLCount() const {
    return _count;
}

String URLManager::getURL(int id) const {
    int idx = findIndexById(id);
    return idx < 0 ? "" : _records[idx].url;
}

String URLManager::getURLName(int id) const {
    int idx = findIndexById(id);
    return idx < 0 ? "" : _records[idx].name;
}

bool URLManager::enableURL(int id) {
    int idx = findIndexById(id);
    if (idx < 0) {
        setError(ErrorCode::URL_ERROR, "URL id not found");
        return false;
    }
    _records[idx].enabled = true;
    return true;
}

bool URLManager::disableURL(int id) {
    int idx = findIndexById(id);
    if (idx < 0) {
        setError(ErrorCode::URL_ERROR, "URL id not found");
        return false;
    }
    _records[idx].enabled = false;
    return true;
}

bool URLManager::validateURL(const String& url) const {
    return _validator.validateURL(url);
}

bool URLManager::testURL(int id) {
    // [PENDING] structural-only for now; see header note.
    int idx = findIndexById(id);
    if (idx < 0) {
        setError(ErrorCode::URL_ERROR, "URL id not found");
        return false;
    }
    return _validator.validateURL(_records[idx].url);
}

bool URLManager::saveURL() {
    return _count > 0; // [PENDING flash persistence]
}

bool URLManager::loadURL() {
    return _count > 0; // [PENDING flash persistence]
}

void URLManager::clearURLs() {
    _count = 0;
    _activeId = -1;
    _nextId = 1;
}

} // namespace AmelTech
