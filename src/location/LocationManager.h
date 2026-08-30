/*
 * LocationManager.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED (in-memory; persistence to flash is a
 * storage/ module responsibility - see saveLocation()/loadLocation()
 * notes below)
 *
 * Fixed-capacity (spec: "Limit stored locations") in-memory list of
 * saved Locations, with full CRUD, active-location selection, and
 * the exact behavior spec's "LOCATION CHANGE BEHAVIOR" section
 * requires (validate -> store -> activate -> invalidate old data ->
 * emit LOCATION_CHANGED).
 */

#ifndef AMELTECH_LOCATION_MANAGER_H
#define AMELTECH_LOCATION_MANAGER_H

#include <Arduino.h>
#include "Location.h"
#include "LocationValidator.h"
#include "../diagnostics/ErrorManager.h"
#include "../core/ClimateEvents.h"

namespace AmelTech {

class LocationManager {
public:
    static const uint8_t MAX_LOCATIONS = 10;

    LocationManager();

    void attachErrorManager(ErrorManager* errorManager);
    void attachEvents(ClimateEvents* events);

    // Returns the new location's id, or -1 on validation failure /
    // capacity exceeded.
    int addLocation(const String& name, float latitude, float longitude);

    bool editLocation(int id, const String& name, float latitude, float longitude);
    bool deleteLocation(int id);

    // Makes location `id` active. Per spec LOCATION CHANGE BEHAVIOR:
    // marks previous data as belonging to the old location (caller -
    // typically AmelTechClimate - is responsible for actually
    // invalidating/refetching ClimateData; this method emits
    // LOCATION_CHANGED so that can happen reactively).
    bool selectLocation(int id);

    const Location* getActiveLocation() const;
    bool hasActiveLocation() const;

    uint8_t listLocations(Location outBuffer[], uint8_t maxCount) const;
    uint8_t getLocationCount() const;

    // Convenience setters/getters operating on the ACTIVE location
    // (spec: setLocation/setLocationName/setLatitude/setLongitude/
    // getLocationName/getLatitude/getLongitude). If no location is
    // active yet, setLocation() creates and activates one.
    bool setLocation(const String& name, float latitude, float longitude);
    bool setLocationName(const String& name);
    bool setLatitude(float latitude);
    bool setLongitude(float longitude);

    String getLocationName() const;
    float getLatitude() const;
    float getLongitude() const;

    // [PENDING full flash persistence] These currently operate on
    // the in-memory list only; true persistence across reboots
    // requires storage/StorageManager (not yet implemented - see
    // PENDING_MODULES.md). Calling them now is safe and does what
    // it can (returns the in-memory snapshot / accepts a snapshot),
    // it just doesn't survive a power cycle yet.
    bool saveLocation();
    bool loadLocation();
    void clearLocations();

private:
    Location _locations[MAX_LOCATIONS];
    uint8_t _count;
    int _activeId;
    int _nextId;

    LocationValidator _validator;
    ErrorManager* _errorManager;
    ClimateEvents* _events;

    int findIndexById(int id) const;
    void setError(ErrorCode code, const char* msg);
};

} // namespace AmelTech

#endif // AMELTECH_LOCATION_MANAGER_H
