/*
 * ProviderManager.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * Owns one OpenMeteoProvider (always present, protected) and one
 * CustomProvider (user-configurable), and exposes which one is
 * currently "active" for climate requests.
 *
 * *** THIS IS THE RULE 1/RULE 2 ENFORCEMENT POINT ***
 * attemptProtectedModification() is the single function that
 * URLManager calls (see urls/URLManager.cpp) whenever a command
 * would otherwise touch the built-in provider. It always returns
 * false and always yields the exact spec-mandated message.
 */

#ifndef AMELTECH_PROVIDER_MANAGER_H
#define AMELTECH_PROVIDER_MANAGER_H

#include <Arduino.h>
#include "OpenMeteoProvider.h"
#include "CustomProvider.h"

namespace AmelTech {

enum class ActiveProvider {
    OPEN_METEO,
    CUSTOM
};

class ProviderManager {
public:
    ProviderManager();

    void useOpenMeteo();
    void useCustomProvider();

    Provider* getProvider(); // returns whichever is currently active
    String getProviderName() const;
    String getProviderStatus() const;
    bool isOpenMeteo() const;

    OpenMeteoProvider& openMeteo() { return _openMeteo; }
    CustomProvider& custom() { return _custom; }

    String getOpenMeteoConfiguration() const;
    void setOpenMeteoVariables(const bool enabledFlags[(int)ClimateParameter::PARAMETER_COUNT]);
    void resetOpenMeteoVariables();

    // Called by URLManager whenever a user command targets the
    // protected provider. Always refuses; returns the required
    // message text so callers can print it verbatim to Serial.
    static String protectedModificationMessage();

private:
    OpenMeteoProvider _openMeteo;
    CustomProvider _custom;
    ActiveProvider _active;
};

} // namespace AmelTech

#endif // AMELTECH_PROVIDER_MANAGER_H
