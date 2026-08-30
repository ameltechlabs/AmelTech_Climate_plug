/*
 * URLManager.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * Fixed-capacity CRUD store for custom URLRecords, entirely separate
 * from OpenMeteoProvider's storage (see that file's header comment).
 *
 * *** RULE 1/2 ENFORCEMENT (user-facing side) ***
 * This class has NO reference to OpenMeteoProvider and cannot reach
 * or alter its base URL under any code path - there is no shared
 * storage, no pointer, no back-channel. The isProtectedName() check
 * exists only as a courtesy: if a user tries to add/edit a custom
 * URL literally named "open-meteo" or similar (to avoid confusing
 * two entries with similar names), they get the spec's required
 * message. This is a naming-collision guard, NOT the actual security
 * boundary - the actual boundary is architectural separation, which
 * holds regardless of what name the user picks.
 */

#ifndef AMELTECH_URL_MANAGER_H
#define AMELTECH_URL_MANAGER_H

#include <Arduino.h>
#include "URLRecord.h"
#include "URLValidator.h"
#include "../diagnostics/ErrorManager.h"
#include "../core/ClimateEvents.h"

namespace AmelTech {

class URLManager {
public:
    static const uint8_t MAX_URLS = 10;

    URLManager();

    void attachErrorManager(ErrorManager* errorManager);
    void attachEvents(ClimateEvents* events);

    // Returns new record's id, or -1 on failure. If `name` collides
    // with a protected-provider name, fails with the spec message
    // (see class header) rather than silently renaming or overwriting.
    int addURL(const String& name, const String& url,
               URLDirection direction = URLDirection::TX,
               URLMethod method = URLMethod::GET,
               URLFormat format = URLFormat::JSON);

    bool editURL(int id, const String& name, const String& url);
    bool deleteURL(int id);
    bool renameURL(int id, const String& newName);

    bool selectURL(int id);
    const URLRecord* getActiveURL() const;

    uint8_t listURLs(URLRecord outBuffer[], uint8_t maxCount) const;
    uint8_t getURLCount() const;

    String getURL(int id) const;
    String getURLName(int id) const;

    bool enableURL(int id);
    bool disableURL(int id);

    // Structural validation only (see URLValidator). Does NOT make a
    // network request - see TXManager/RXManager for that.
    bool validateURL(const String& url) const;

    // [PENDING network reachability check] Currently performs only
    // structural validation; a live HTTP HEAD/GET check requires
    // wiring an HTTPManager instance, planned alongside TXManager/
    // RXManager network integration.
    bool testURL(int id);

    // [PENDING flash persistence] see LocationManager.h for the same
    // caveat - operates on the in-memory list only for now.
    bool saveURL();
    bool loadURL();
    void clearURLs();

    // Returns true (and refuses the operation) if `name` collides
    // with a reserved/protected provider name.
    static bool isProtectedName(const String& name);

private:
    URLRecord _records[MAX_URLS];
    uint8_t _count;
    int _activeId;
    int _nextId;

    URLValidator _validator;
    ErrorManager* _errorManager;
    ClimateEvents* _events;

    int findIndexById(int id) const;
    void setError(ErrorCode code, const char* msg);
};

} // namespace AmelTech

#endif // AMELTECH_URL_MANAGER_H
