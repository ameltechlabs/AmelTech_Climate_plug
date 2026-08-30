/*
 * LocationManager.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "LocationManager.h"

namespace AmelTech {

LocationManager::LocationManager()
    : _count(0), _activeId(-1), _nextId(1), _errorManager(nullptr), _events(nullptr) {
}

void LocationManager::attachErrorManager(ErrorManager* errorManager) {
    _errorManager = errorManager;
}

void LocationManager::attachEvents(ClimateEvents* events) {
    _events = events;
}

void LocationManager::setError(ErrorCode code, const char* msg) {
    if (_errorManager) _errorManager->setError(code, msg);
}

int LocationManager::findIndexById(int id) const {
    for (uint8_t i = 0; i < _count; i++) {
        if (_locations[i].id == id) return (int)i;
    }
    return -1;
}

int LocationManager::addLocation(const String& name, float latitude, float longitude) {
    if (!_validator.validateLocation(name, latitude, longitude)) {
        setError(ErrorCode::LOCATION_INVALID, "Invalid name or coordinates out of range");
        return -1;
    }

    if (_count >= MAX_LOCATIONS) {
        setError(ErrorCode::CONFIG_ERROR, "Maximum stored locations reached");
        return -1;
    }

    int newId = _nextId++;
    _locations[_count] = Location(newId, name, latitude, longitude);
    _count++;

    return newId;
}

bool LocationManager::editLocation(int id, const String& name, float latitude, float longitude) {
    if (!_validator.validateLocation(name, latitude, longitude)) {
        setError(ErrorCode::LOCATION_INVALID, "Invalid name or coordinates out of range");
        return false;
    }

    int idx = findIndexById(id);
    if (idx < 0) {
        setError(ErrorCode::LOCATION_INVALID, "Location id not found");
        return false;
    }

    _locations[idx].name = name;
    _locations[idx].latitude = latitude;
    _locations[idx].longitude = longitude;

    if (_activeId == id && _events) {
        _events->emit(Event::LOCATION_CHANGED, name);
    }

    return true;
}

bool LocationManager::deleteLocation(int id) {
    int idx = findIndexById(id);
    if (idx < 0) {
        setError(ErrorCode::LOCATION_INVALID, "Location id not found");
        return false;
    }

    // Shift remaining entries down to keep the array contiguous.
    for (uint8_t i = idx; i < _count - 1; i++) {
        _locations[i] = _locations[i + 1];
    }
    _count--;

    if (_activeId == id) {
        _activeId = -1; // active location was deleted; caller must select a new one
    }

    return true;
}

bool LocationManager::selectLocation(int id) {
    int idx = findIndexById(id);
    if (idx < 0) {
        setError(ErrorCode::LOCATION_INVALID, "Location id not found");
        return false;
    }

    _activeId = id;
    if (_events) _events->emit(Event::LOCATION_CHANGED, _locations[idx].name);
    return true;
}

const Location* LocationManager::getActiveLocation() const {
    if (_activeId < 0) return nullptr;
    int idx = findIndexById(_activeId);
    if (idx < 0) return nullptr;
    return &_locations[idx];
}

bool LocationManager::hasActiveLocation() const {
    return getActiveLocation() != nullptr;
}

uint8_t LocationManager::listLocations(Location outBuffer[], uint8_t maxCount) const {
    uint8_t n = (_count < maxCount) ? _count : maxCount;
    for (uint8_t i = 0; i < n; i++) {
        outBuffer[i] = _locations[i];
    }
    return n;
}

uint8_t LocationManager::getLocationCount() const {
    return _count;
}

bool LocationManager::setLocation(const String& name, float latitude, float longitude) {
    const Location* active = getActiveLocation();

    if (active == nullptr) {
        int newId = addLocation(name, latitude, longitude);
        if (newId < 0) return false;
        return selectLocation(newId);
    }

    return editLocation(active->id, name, latitude, longitude);
}

bool LocationManager::setLocationName(const String& name) {
    const Location* active = getActiveLocation();
    if (active == nullptr) {
        setError(ErrorCode::LOCATION_INVALID, "No active location to rename");
        return false;
    }
    return editLocation(active->id, name, active->latitude, active->longitude);
}

bool LocationManager::setLatitude(float latitude) {
    const Location* active = getActiveLocation();
    if (active == nullptr) {
        setError(ErrorCode::LOCATION_INVALID, "No active location set");
        return false;
    }
    return editLocation(active->id, active->name, latitude, active->longitude);
}

bool LocationManager::setLongitude(float longitude) {
    const Location* active = getActiveLocation();
    if (active == nullptr) {
        setError(ErrorCode::LOCATION_INVALID, "No active location set");
        return false;
    }
    return editLocation(active->id, active->name, active->latitude, longitude);
}

String LocationManager::getLocationName() const {
    const Location* active = getActiveLocation();
    return active ? active->name : "";
}

float LocationManager::getLatitude() const {
    const Location* active = getActiveLocation();
    return active ? active->latitude : 0.0f;
}

float LocationManager::getLongitude() const {
    const Location* active = getActiveLocation();
    return active ? active->longitude : 0.0f;
}

bool LocationManager::saveLocation() {
    // [PENDING] In-memory list is already "saved" for the current
    // session; true flash persistence needs StorageManager (Phase 3+).
    return _count > 0;
}

bool LocationManager::loadLocation() {
    // [PENDING] No flash-backed store to load from yet; the
    // in-memory list simply persists for the life of the sketch.
    return _count > 0;
}

void LocationManager::clearLocations() {
    _count = 0;
    _activeId = -1;
    _nextId = 1;
}

} // namespace AmelTech
