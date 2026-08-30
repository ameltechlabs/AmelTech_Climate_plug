/*
 * ProviderManager.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "ProviderManager.h"

namespace AmelTech {

ProviderManager::ProviderManager() : _active(ActiveProvider::OPEN_METEO) {
}

void ProviderManager::useOpenMeteo() {
    _active = ActiveProvider::OPEN_METEO;
}

void ProviderManager::useCustomProvider() {
    _active = ActiveProvider::CUSTOM;
}

Provider* ProviderManager::getProvider() {
    if (_active == ActiveProvider::OPEN_METEO) {
        return &_openMeteo;
    }
    return &_custom;
}

String ProviderManager::getProviderName() const {
    return (_active == ActiveProvider::OPEN_METEO) ? _openMeteo.getName() : _custom.getName();
}

String ProviderManager::getProviderStatus() const {
    String status = "Active provider: " + getProviderName() + "\n";
    if (_active == ActiveProvider::OPEN_METEO) {
        status += _openMeteo.getOpenMeteoConfiguration();
    } else {
        status += "Endpoint: " + (_custom.isConfigured() ? _custom.getEndpoint() : String("[not configured]"));
    }
    return status;
}

bool ProviderManager::isOpenMeteo() const {
    return _active == ActiveProvider::OPEN_METEO;
}

String ProviderManager::getOpenMeteoConfiguration() const {
    return _openMeteo.getOpenMeteoConfiguration();
}

void ProviderManager::setOpenMeteoVariables(const bool enabledFlags[(int)ClimateParameter::PARAMETER_COUNT]) {
    _openMeteo.setOpenMeteoVariables(enabledFlags);
}

void ProviderManager::resetOpenMeteoVariables() {
    _openMeteo.resetOpenMeteoVariables();
}

String ProviderManager::protectedModificationMessage() {
    return "Protected provider: modification not allowed.";
}

} // namespace AmelTech
