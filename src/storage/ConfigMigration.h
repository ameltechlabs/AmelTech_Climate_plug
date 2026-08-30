/*
 * ConfigMigration.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED (framework real; no migrations exist yet
 * since CURRENT_CONFIG_VERSION == 1 is the only version that has
 * ever shipped - there is nothing to migrate FROM yet. The
 * mechanism is real and ready for the first future version bump.)
 *
 * Header-only migration dispatch: given a stored version number,
 * returns whether a migration is needed and performs it via
 * StorageManager's raw Preferences access. Kept as free functions
 * (not a class) since migrations are inherently one-off/versioned
 * logic, not stateful objects.
 */

#ifndef AMELTECH_CONFIG_MIGRATION_H
#define AMELTECH_CONFIG_MIGRATION_H

#include <Arduino.h>
#include <Preferences.h>
#include "ConfigVersion.h"

namespace AmelTech {

inline bool configNeedsMigration(uint16_t storedVersion) {
    return storedVersion != 0 && storedVersion < CURRENT_CONFIG_VERSION;
}

// Runs whatever migration steps are needed to bring `prefs` from
// `fromVersion` up to CURRENT_CONFIG_VERSION. Returns true if the
// migration completed (or wasn't needed); false if a migration step
// failed partway (caller should treat this as CONFIG_ERROR and
// consider offering factoryReset() rather than trusting partial data).
//
// No migration steps exist yet - see file header. This function is a
// documented no-op placeholder that returns true (nothing to do)
// rather than false (which would incorrectly signal an error), since
// "no migration needed/available yet" is not a failure.
inline bool runConfigMigration(Preferences& prefs, uint16_t fromVersion) {
    (void)prefs;
    if (fromVersion == CURRENT_CONFIG_VERSION) return true;

    // Future version bumps add `if (fromVersion == N) { ...steps...; fromVersion = N+1; }`
    // chains here, falling through until fromVersion == CURRENT_CONFIG_VERSION.

    return true;
}

} // namespace AmelTech

#endif // AMELTECH_CONFIG_MIGRATION_H
