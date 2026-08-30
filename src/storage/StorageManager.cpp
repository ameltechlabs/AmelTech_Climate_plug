/*
 * StorageManager.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "StorageManager.h"

namespace AmelTech {

StorageManager::StorageManager() : _open(false), _readOnly(false), _errorManager(nullptr) {
}

void StorageManager::attachErrorManager(ErrorManager* errorManager) {
    _errorManager = errorManager;
}

void StorageManager::setError(ErrorCode code, const char* msg) {
    if (_errorManager) _errorManager->setError(code, msg);
}

bool StorageManager::begin(const char* nvsNamespace, bool readOnly) {
    if (nvsNamespace == nullptr || strlen(nvsNamespace) == 0 || strlen(nvsNamespace) > 15) {
        setError(ErrorCode::STORAGE_ERROR, "NVS namespace must be 1-15 characters");
        return false;
    }

    _open = _prefs.begin(nvsNamespace, readOnly);
    _readOnly = readOnly;

    if (!_open) {
        setError(ErrorCode::STORAGE_ERROR, "Preferences::begin() failed - flash may be corrupted or full");
    }
    return _open;
}

void StorageManager::end() {
    if (_open) {
        _prefs.end();
        _open = false;
    }
}

bool StorageManager::save(const char* key, const String& value) {
    if (!_open || _readOnly) { setError(ErrorCode::STORAGE_ERROR, "Storage not open or is read-only"); return false; }
    return _prefs.putString(key, value) > 0 || value.length() == 0;
}

bool StorageManager::save(const char* key, float value) {
    if (!_open || _readOnly) { setError(ErrorCode::STORAGE_ERROR, "Storage not open or is read-only"); return false; }
    return _prefs.putFloat(key, value) == sizeof(float);
}

bool StorageManager::save(const char* key, int value) {
    if (!_open || _readOnly) { setError(ErrorCode::STORAGE_ERROR, "Storage not open or is read-only"); return false; }
    return _prefs.putInt(key, value) == sizeof(int);
}

bool StorageManager::save(const char* key, bool value) {
    if (!_open || _readOnly) { setError(ErrorCode::STORAGE_ERROR, "Storage not open or is read-only"); return false; }
    return _prefs.putBool(key, value) == sizeof(bool);
}

bool StorageManager::save(const char* key, uint32_t value) {
    if (!_open || _readOnly) { setError(ErrorCode::STORAGE_ERROR, "Storage not open or is read-only"); return false; }
    return _prefs.putUInt(key, value) == sizeof(uint32_t);
}

bool StorageManager::save(const char* key, unsigned long value) {
    if (!_open || _readOnly) { setError(ErrorCode::STORAGE_ERROR, "Storage not open or is read-only"); return false; }
    return _prefs.putULong(key, value) == sizeof(unsigned long);
}

String StorageManager::loadString(const char* key, const String& defaultValue) const {
    if (!_open) return defaultValue;
    return _prefs.getString(key, defaultValue);
}

float StorageManager::loadFloat(const char* key, float defaultValue) const {
    if (!_open) return defaultValue;
    return _prefs.getFloat(key, defaultValue);
}

int StorageManager::loadInt(const char* key, int defaultValue) const {
    if (!_open) return defaultValue;
    return _prefs.getInt(key, defaultValue);
}

bool StorageManager::loadBool(const char* key, bool defaultValue) const {
    if (!_open) return defaultValue;
    return _prefs.getBool(key, defaultValue);
}

uint32_t StorageManager::loadUInt(const char* key, uint32_t defaultValue) const {
    if (!_open) return defaultValue;
    return _prefs.getUInt(key, defaultValue);
}

unsigned long StorageManager::loadULong(const char* key, unsigned long defaultValue) const {
    if (!_open) return defaultValue;
    return _prefs.getULong(key, defaultValue);
}

bool StorageManager::exists(const char* key) const {
    if (!_open) return false;
    return _prefs.isKey(key);
}

bool StorageManager::remove(const char* key) {
    if (!_open || _readOnly) { setError(ErrorCode::STORAGE_ERROR, "Storage not open or is read-only"); return false; }
    return _prefs.remove(key);
}

void StorageManager::clear() {
    if (_open && !_readOnly) {
        _prefs.clear();
    }
}

bool StorageManager::isOpen() const {
    return _open;
}

} // namespace AmelTech
