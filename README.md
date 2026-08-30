# AmelTech_Climate_plug

ESP32 climate-monitoring library with a built-in, no-API-key
Open-Meteo provider, optional local sensors, hybrid remote+local data,
caching, a Serial command interface, and optional MQTT/Blynk/REST/
Webhook platform integration.

**Status: functional core, incomplete packaging.** Every module
described below is real, working code — not stubs. What's missing is
the example sketches and full documentation set; see
[PENDING_MODULES.md](PENDING_MODULES.md) for the exact, current list
of gaps. This file will be expanded as those are filled in.

## Features

- Built-in Open-Meteo climate data — no API key required
- The built-in provider is protected: it cannot be overwritten by the
  custom URL system
- User-configurable custom URLs with TX (send) / RX (receive) / TX+RX
- Serial Monitor command interface
- Multi-location management with runtime switching
- Continuous "live status" display, decoupled from update frequency
- Local sensor support: DHT11, DHT22, BME280, BME680, SHT31
- Hybrid mode: merge remote provider data with local sensor readings
- Optional MQTT, Blynk, REST, and Webhook platform integration
- Persistent configuration (WiFi, locations, URLs) via ESP32 flash
- Caching with configurable lifetime and LIVE/CACHED/STALE states
- Bounded retry with backoff — never retries forever
- Non-blocking main loop throughout

## Installation

### Arduino IDE

1. Download this repository as a ZIP.
2. Sketch → Include Library → Add .ZIP Library... → select the
   downloaded file.
3. `#include <AmelTech_Climate_plug.h>` in your sketch.

### PlatformIO

Add to `platformio.ini`:
```ini
lib_deps =
    https://github.com/AmelTechLabs/AmelTech_Climate_plug.git
```

## Basic usage

```cpp
#include <AmelTech_Climate_plug.h>

AmelTech::AmelTechClimate climate;

void setup() {
    Serial.begin(115200);
    climate.begin("YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD");
    climate.setLocation("My Location", 10.5276, 76.2144);
}

void loop() {
    climate.loop();

    if (!climate.isUpdating() && climate.isDataValid()) {
        float temp = climate.getTemperature();
        if (!isnan(temp)) {
            Serial.println("Temperature: " + String(temp) + " C");
        }
    }
}
```

Type `help` into the Serial Monitor once connected for the full list
of available commands.

## Optional dependencies

None of these are required for the core Open-Meteo functionality.
Install only what you use:

| Feature | Library |
|---|---|
| DHT11/DHT22 sensor | Adafruit "DHT sensor library" + "Adafruit Unified Sensor" |
| BME280 sensor | "Adafruit BME280 Library" + "Adafruit Unified Sensor" + "Adafruit BusIO" |
| BME680 sensor | "Adafruit BME680 Library" + same Adafruit dependencies |
| SHT31 sensor | "Adafruit SHT31 Library" + same Adafruit dependencies |
| MQTT platform | "PubSubClient" by Nick O'Leary |
| Blynk platform | "Blynk" (Blynk IoT) |
| JSON parsing (core, required) | "ArduinoJson" (v6 or later) |

## External-service usage limitations

This library does not, and cannot, provide unlimited use of
Open-Meteo or any other external API. Actual usage remains subject to
the applicable provider's terms of service, rate limits, and fair-use
policy, regardless of how many devices run this library. This library
does not implement anything intended to bypass those limits.

## Architecture

Every subsystem lives in its own module under `src/`: `core/`,
`network/`, `providers/`, `request/`, `data/`, `location/`, `urls/`,
`sensors/`, `cache/`, `serial/`, `platforms/`, `storage/`,
`security/`, `diagnostics/`. The top-level `AmelTechClimate` class in
`src/AmelTech_Climate_plug.cpp` is a thin orchestration layer over
these — it holds one instance of each manager and delegates to it.

See [PENDING_MODULES.md](PENDING_MODULES.md) for exactly what's
implemented vs. still pending, and [CONTRIBUTING.md](CONTRIBUTING.md)
for the key invariants to preserve when modifying this code
(especially the Open-Meteo protection boundary).

## License

MIT — see [LICENSE](LICENSE). Note the additional clause there
regarding third-party API usage through this library.
