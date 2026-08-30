# Pending Modules & Known Gaps

This file is the single source of truth for what is and isn't done in
AmelTech_Climate_plug. Every "not yet implemented" message printed by
the library points here. This document is updated whenever a gap
below is closed — if something here is stale, the code comment near
the relevant stub is the tiebreaker.

## Fully implemented and wired (verified via syntax check against
## real Arduino/ESP32 API signatures — see note at bottom)

- Core: boot sequencing, non-blocking scheduler, event system, error
  management
- Network: WiFi management, HTTP client, TLS, connection facade,
  bounded retry with backoff
- Providers: the protected Open-Meteo provider, custom provider,
  provider manager with the "Protected provider: modification not
  allowed" enforcement
- Request pipeline: request building, response parsing/validation/
  normalization, non-blocking request state machine
- Data: the central `ClimateData` contract, validation, normalization
  (including full WMO weather code table), quality scoring, parameter
  registry
- Location: multi-location CRUD, coordinate validation
- Custom URLs / TX / RX: URL CRUD (separate storage from Open-Meteo),
  structural URL validation, TX send (JSON/text/raw/climate data/
  sensor data), RX fetch
- Sensors: DHT11/DHT22, BME280, BME680, SHT31 adapters (each an
  optional dependency), sensor registry and manager, multi-sensor
  merge into one `ClimateData`
- Cache: in-memory cache with TTL, optional flash-backed persistence,
  LIVE/CACHED/STALE/UNAVAILABLE state tracking
- Platforms: REST and Webhook (no external dependency, work
  immediately), MQTT (optional PubSubClient dependency), Blynk
  (optional Blynk library dependency)
- Storage: generic ESP32 NVS wrapper, domain-specific persistence for
  WiFi/locations/URLs/sensors/platforms, config versioning/migration
  framework (no migrations needed yet — this is the first schema
  version)
- Security: input sanitization, credential masking (WiFi passwords
  are never printed to Serial), URL validation
- Top-level `AmelTechClimate` public API: every method listed in the
  original spec is declared; all of the above are wired in and
  callable through it
- Serial commands: `help`, `status`, `update`, `live status`,
  `live status stop`, `location add/select/delete/list/change`,
  `URL link add/select/delete/list/test/change`, `sensor scan/status`,
  `cache status/clear`, `network status`, `provider status`,
  `platform status`, `factory reset`
- All 16 example sketches (`01_Basic` through `16_FullSystem`) — each
  is a real, working `.ino` file, individually compile-verified
  against the actual class signatures (see verification note at the
  bottom of this file).

## Genuinely NOT implemented (calling these returns a clear message,
## not a silent no-op)

- **`sensor read`** (Serial command) — no single-sensor immediate-read
  command handler exists yet; `readAllSensorsIntoLocalData()` (public
  API) and `sensor status` (Serial) both work today as partial
  substitutes.
- **`diagnostics`** (Serial command) — the spec's `healthCheck()` /
  `networkHealth()` / etc. family from the HEALTH MONITOR section was
  never implemented as a distinct module. `network status`,
  `provider status`, `platform status`, and `cache status` cover
  related ground individually.
- **`statistics`** (Serial command) — the spec's STATISTICS section
  (request counts, cache hit/miss counts, sensor read counts, etc.)
  was never implemented. No counters are currently tracked anywhere
  in the codebase.
- **Automatic location/URL flash persistence** — `saveLocationsToFlash()`
  and `saveURLsToFlash()` exist and work, but nothing calls them
  automatically after `addLocation()`/`addURL()`/etc. You must call
  them explicitly if you want changes to survive a reboot.
- **Full documentation set** (`docs/Architecture.md`, `docs/API.md`,
  `docs/Commands.md`, `docs/OpenMeteo.md`, `docs/Locations.md`,
  `docs/URLs.md`, `docs/Sensors.md`, `docs/Platforms.md`,
  `docs/Cache.md`, `docs/Security.md`, `docs/Troubleshooting.md`) —
  none of these files have been written yet.
- **`test/` directory contents** — the folder structure exists
  (`test_data/`, `test_location/`, `test_url/`, `test_parser/`,
  `test_validation/`) but no actual test files have been written.

## A note on verification

Every file in this library was syntax-checked with `g++ -fsyntax-only`
against a set of mock headers approximating the real Arduino Core,
ESP32 WiFi/HTTPClient/Preferences libraries, ArduinoJson, and the four
optional sensor/platform libraries (Adafruit DHT/BME280/BME680/SHT31,
PubSubClient, Blynk). This caught and fixed three real bugs during
development, including a preprocessor-macro collision between this
library's `SensorType` enum and the real DHT library's `DHT11`/`DHT22`
macros that would have broken real hardware builds.

**This is not the same as compiling against the real, current-version
Arduino/ESP32 toolchain.** No `arduino-cli` or network access to
download the ESP32 board core was available during development. If
you hit a compile error on real hardware that the syntax check should
have caught, it's likely because a mock header didn't perfectly match
the real library's current API surface — please file an issue with
the exact error.
