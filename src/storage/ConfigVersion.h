/*
 * ConfigVersion.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * Tracks the on-flash configuration schema version, so future
 * library versions can detect an older stored config and migrate it
 * rather than silently misreading it or wiping it.
 */

#ifndef AMELTECH_CONFIG_VERSION_H
#define AMELTECH_CONFIG_VERSION_H

#include <Arduino.h>

namespace AmelTech {

// Bump this whenever the on-flash layout in StorageManager/
// ConfigurationManager changes in a way that isn't backward-readable.
static const uint16_t CURRENT_CONFIG_VERSION = 1;

} // namespace AmelTech

#endif // AMELTECH_CONFIG_VERSION_H
