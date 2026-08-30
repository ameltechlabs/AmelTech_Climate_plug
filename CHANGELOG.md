# Changelog

All notable changes to AmelTech_Climate_plug are documented here.

## [0.2.0] - Wired release

### Added
- Full wiring of location, custom URL/TX/RX, sensor, cache, platform,
  storage, and security modules into the top-level `AmelTechClimate`
  public API. Previously these modules existed as standalone,
  independently-verified code; this release connects them.
- Serial commands `location add/select/delete/list/change`,
  `URL link add/select/delete/list/test/change`, `sensor scan/status`,
  `cache status/clear`, `network status`, `provider status`,
  `platform status`, and `factory reset` now dispatch to real,
  working managers instead of placeholder text.
- Hybrid mode (`setMode`, `enableHybrid`, `useLocalData`,
  `useRemoteData`) now genuinely merges remote provider data with
  local sensor readings via `ClimateData::mergeFrom`.
- `factoryReset()` now genuinely clears flash-persisted configuration
  via `ConfigurationManager`, not just in-memory state.

### Known gaps (see PENDING_MODULES.md)
- `sensor read`, `diagnostics`, and `statistics` Serial commands are
  still placeholders - no backing implementation exists yet.
- Location/URL flash persistence must be triggered explicitly via
  `saveLocationsToFlash()`/`saveURLsToFlash()`; it is not yet
  automatic on every add/edit.
- Full worked example sketches (all 16 named in the original spec)
  and complete docs/ reference files are not yet included in this
  package - see PENDING_MODULES.md for the current status of each.

## [0.1.0] - Phase 1 (initial)

### Added
- Core architecture: WiFi connectivity, the protected built-in
  Open-Meteo provider, climate data acquisition/validation/
  normalization, non-blocking scheduler, event system.
- Serial commands: `help`, `status`, `update`, `live status`,
  `live status stop`.
- Complete standalone implementations of every module in the spec's
  architecture (location, urls, sensors, cache, platforms, storage,
  security) - not yet wired to the public API at this point.
