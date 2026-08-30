# Contributing to AmelTech_Climate_plug

## Architecture

Before contributing, read `docs/Architecture.md` (or, until that file
is written, the module-level header comments in `src/*/`) — every
module has a clear single responsibility and the top-level
`AmelTechClimate` class in `src/AmelTech_Climate_plug.cpp` is
intentionally a thin orchestration layer over the real logic, which
lives in `src/core/`, `src/location/`, `src/urls/`, etc.

## Key invariants to preserve

1. **Open-Meteo protection** (`src/providers/OpenMeteoProvider.h`):
   the built-in endpoint must never become editable through
   `URLManager` or any public setter. If you're touching provider or
   URL code, re-read that file's header comment first.
2. **Non-blocking `loop()`**: no `delay()` calls in any code path that
   runs from `AmelTechClimate::loop()`, except the one documented
   exception in `ClimateCore::bootStageWiFi()` (bounded, runs only
   during `begin()`).
3. **Never fabricate missing data as zero**: every `ClimateData` field
   has a matching `hasX` flag. If a value isn't available, leave
   `hasX` false — don't default the value to 0.0 and set `hasX` true.
4. **Bounded retries**: `RetryManager` enforces a hard ceiling
   (`HARD_MAX_RETRIES`). Don't bypass it.

## Testing your changes

This repository does not yet have a CI pipeline. Before submitting a
change, at minimum:
- Compile against the ESP32 Arduino core (Arduino IDE or
  `arduino-cli`) for at least one real board.
- If you can't access real hardware, a syntax-only check against
  mocked Arduino/ESP32 headers (`g++ -fsyntax-only`) catches most
  structural errors, though it won't catch ESP32-specific API misuse.

## Reporting issues

Please include: board model, Arduino core version, the exact Serial
output (with any WiFi password/API tokens redacted), and whether the
issue is reproducible with `useOpenMeteo()` (built-in provider) or
only with a custom provider/URL.
