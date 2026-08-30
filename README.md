# AmelTech_Climate_plug

An ESP32 library for climate and weather monitoring. It comes with
free, built-in weather data (no API key needed), and can also read
local sensors, talk to your own server, or publish to MQTT/Blynk —
all through simple function calls or by typing commands into the
Serial Monitor.

**Honesty note before you start:** this README documents exactly what
the code does right now — nothing more. A handful of features are
listed as "not yet available" further down, and those are genuinely
not built yet, not just undocumented. See
[PENDING_MODULES.md](PENDING_MODULES.md) for the complete, current
list.

---

## Table of contents

1. [Introduction](#1-introduction)
2. [Features](#2-features)
3. [Supported ESP32 usage](#3-supported-esp32-usage)
4. [Installation](#4-installation)
5. [Arduino IDE installation](#5-arduino-ide-installation)
6. [PlatformIO installation](#6-platformio-installation)
7. [Basic example](#7-basic-example)
8. [Open-Meteo architecture](#8-open-meteo-architecture)
9. [Location commands](#9-location-commands)
10. [URL commands](#10-url-commands)
11. [TX/RX](#11-txrx)
12. [Live status](#12-live-status)
13. [Sensors](#13-sensors)
14. [Hybrid mode](#14-hybrid-mode)
15. [Blynk integration](#15-blynk-integration)
16. [MQTT integration](#16-mqtt-integration)
17. [REST integration](#17-rest-integration)
18. [Webhook integration](#18-webhook-integration)
19. [Cache](#19-cache)
20. [Error handling](#20-error-handling)
21. [Diagnostics](#21-diagnostics)
22. [Security](#22-security)
23. [External-service usage limitations](#23-external-service-usage-limitations)
24. [Troubleshooting](#24-troubleshooting)
25. [API reference](#25-api-reference)
26. [License](#26-license)

---

## 1. Introduction

AmelTech_Climate_plug turns an ESP32 into a climate station. Out of
the box it fetches live weather data from
[Open-Meteo](https://open-meteo.com) for any location on Earth — no
sign-up, no API key. From there you can layer on local sensors
(temperature/humidity/pressure chips wired to your board), send data
to your own server or MQTT broker, and control all of it either from
your sketch's code or by typing plain-English-ish commands into the
Serial Monitor while the board is running.

The library is built as many small, focused pieces (one folder per
concern under `src/`) rather than one giant file, so it's easier to
read, test, and extend. You interact with almost all of it through
one object: `AmelTechClimate`.

## 2. Features

- Free built-in weather data via Open-Meteo — no API key
- The built-in weather source is protected — you can't accidentally
  overwrite it while configuring your own custom URLs
- Your own custom URLs, for sending data out (TX), pulling data in
  (RX), or both
- A Serial Monitor command interface — type `help` to see what's
  available
- Multiple saved locations, switchable while the board is running
- A continuous "live status" screen that refreshes on its own timer
- Local sensor support: DHT11, DHT22, BME280, BME680, SHT31
- Hybrid mode — blend live weather data with your own local sensor
  readings
- Optional MQTT, Blynk, REST, and Webhook integrations
- Settings (WiFi, saved locations, custom URLs) can be saved to flash
  and survive a reboot
- A local cache, so you're not stuck with nothing if a request fails
- The main loop never freezes waiting on the network — everything is
  designed to keep running

## 3. Supported ESP32 usage

This library targets the ESP32 Arduino framework and doesn't assume
any particular board — it should work on any ESP32 board supported by
that framework (ESP32, ESP32-S2, ESP32-S3, ESP32-C3, etc.), as long as
the board has WiFi, which the library requires.

You can install this library on as many boards as you like. What
*isn't* unlimited is Open-Meteo itself — see
[section 23](#23-external-service-usage-limitations).

## 4. Installation

Pick whichever matches your setup:

- **Arduino IDE** → [section 5](#5-arduino-ide-installation)
- **PlatformIO** → [section 6](#6-platformio-installation)

Either way, you'll also want **ArduinoJson** (version 6 or later) —
the library uses it to parse weather data, and it's required even if
you don't touch any of the optional sensor/platform features.

## 5. Arduino IDE installation

1. Download this repository as a ZIP file (if you're reading this
   from GitHub: the green "Code" button → "Download ZIP").
2. Open the Arduino IDE.
3. Go to **Sketch → Include Library → Add .ZIP Library...**
4. Select the ZIP file you downloaded.
5. Also install **ArduinoJson** via **Sketch → Include Library →
   Manage Libraries...** (search "ArduinoJson", install the latest
   6.x or 7.x release).
6. In your sketch, add:
   ```cpp
   #include <AmelTech_Climate_plug.h>
   ```

If you plan to use a local sensor or a platform like MQTT/Blynk,
install that specific library too — see [section 13](#13-sensors) and
sections [15](#15-blynk-integration)–[18](#18-webhook-integration) for
exactly which ones.

## 6. PlatformIO installation

Add this to your `platformio.ini`:

```ini
[env:your_esp32_board]
platform = espressif32
board = your_board_name
framework = arduino
lib_deps =
    https://github.com/AmelTechLabs/AmelTech_Climate_plug.git
    bblanchon/ArduinoJson @ ^7.0.0
```

Swap in whatever board name matches your hardware. Add any
sensor/platform libraries from sections 13, 15–18 to `lib_deps` the
same way, only if you actually need them.

## 7. Basic example

This is the smallest sketch that does something useful — connect to
WiFi, set a location, and print the temperature once data arrives.
A working copy of this exact code is included at
[`examples/01_Basic/01_Basic.ino`](examples/01_Basic/01_Basic.ino).

```cpp
#include <AmelTech_Climate_plug.h>

AmelTech::AmelTechClimate climate;

void setup() {
    Serial.begin(115200);

    climate.begin("YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD");
    climate.setLocation("My Location", 10.5276, 76.2144);
}

void loop() {
    climate.loop(); // must be called every iteration - see note below

    if (!climate.isUpdating() && climate.isDataValid()) {
        float temp = climate.getTemperature();
        if (!isnan(temp)) {
            Serial.println("Temperature: " + String(temp) + " C");
        }
    }
}
```

**Why `climate.loop()` matters:** this one call is what actually does
the work — checking WiFi, deciding if it's time to fetch new weather
data, processing Serial commands, and so on. Nothing in this library
blocks for a long time, so it's safe to call every single iteration of
your `loop()`, even if you have other things going on in the same
sketch.

**Why some getters can return "not a number":** `getTemperature()` and
similar functions can return `NAN` (not-a-number) if that particular
piece of data isn't available yet — for example, right after boot,
before the first weather update has completed. Always check with
`isnan()` (a standard C function) before using the value, as shown
above. This library deliberately never invents a fake `0` for missing
data — see [section 20](#20-error-handling) for why.

## 8. Open-Meteo architecture

The built-in weather provider (Open-Meteo) is intentionally
**protected** — you cannot overwrite or redirect it, on purpose.

Here's why this matters: the library also lets you add your *own*
custom URLs (for sending data to your own server, for example — see
[section 10](#10-url-commands)). Without protection, it would be easy
to accidentally overwrite the built-in weather source with one of your
custom URLs by reusing a name like `open-meteo`. This library keeps
the two systems completely separate under the hood, and additionally
refuses any custom URL literally named `open-meteo`, `openmeteo`,
`open_meteo`, or `meteo` (case-insensitive) with this exact message:

```
Protected provider: modification not allowed.
```

You can switch *which* provider is active — built-in Open-Meteo, or
one of your own custom URLs — with:

```cpp
climate.useOpenMeteo();       // switch to the built-in weather source
climate.useCustomProvider();  // switch to your currently-selected custom URL
```

Switching to a custom provider requires you to have already added and
selected a custom URL (see [section 10](#10-url-commands)) — if you
haven't, `useCustomProvider()` will set an error rather than silently
doing nothing.

To see what's currently active:

```cpp
String name = climate.getProviderName();      // "Open-Meteo" or "Custom"
String status = climate.getProviderStatus();   // a readable summary
bool isBuiltIn = climate.isOpenMeteo();
```

### What weather data is available

Open-Meteo can return a lot of different measurements, but not every
one is guaranteed for every location. This library tracks 20 possible
fields — temperature, humidity, dew point, feels-like temperature,
pressure, precipitation, rain, snowfall, weather code and condition,
cloud cover, wind speed/direction/gusts, solar radiation, UV index,
evapotranspiration, soil temperature, soil moisture, freezing level,
visibility, and CAPE (a storm-severity measurement).

If one of these isn't available for your location, the corresponding
getter returns `NAN` (or `-1` for the weather code) rather than a
made-up `0` — see [section 20](#20-error-handling).

## 9. Location commands

### From your sketch (code)

```cpp
// Set the ONE active location (used for weather updates):
climate.setLocation("Thrissur", 10.5276, 76.2144);

// Read it back:
String name = climate.getLocationName();
float lat = climate.getLatitude();
float lon = climate.getLongitude();

// Manage a LIST of saved locations (up to 10):
bool added = climate.addLocation("Backup Location", 12.9716, 77.5946);
climate.selectLocation(2);       // make saved location #2 active (by its id)
climate.editLocation(2, "New Name", 13.0, 78.0);
climate.deleteLocation(2);
int count = climate.getLocationCount();

// Save/load the saved-location list to/from flash (survives reboot):
climate.saveLocationsToFlash();
climate.loadLocationsFromFlash();
```

Coordinates are validated: latitude must be between -90 and +90,
longitude between -180 and +180. Anything outside that range is
rejected — check `climate.hasError()` afterward if you're not sure it
worked.

### From the Serial Monitor

Type any of these (press Enter after each):

| Command | What it does |
|---|---|
| `location change <name> / <lat> / <lon>` | Change the active location in one step |
| `location add <name> / <lat> / <lon>` | Add a new saved location |
| `location select <id>` | Make a saved location active |
| `location delete <id>` | Remove a saved location |
| `location list` | List every saved location |
| `location show` | Show the currently active location |

Example:
```
location change Kerala, Thrissur / 10.5276 / 76.2144
```

After a successful location change, the library automatically:
validates the coordinates, saves the location, makes it active, and
schedules a fresh weather update for the new location.

## 10. URL commands

Custom URLs are for connecting this library to *your own* services —
for example, sending weather data to your own backend, or pulling data
from an API you control. They're completely separate from the
protected Open-Meteo system (see [section 8](#8-open-meteo-architecture)).

### From your sketch (code)

```cpp
climate.addURL("my-server", "https://example.com/api/climate");
climate.selectURL(1);   // make custom URL #1 active
climate.editURL(1, "my-server", "https://example.com/api/v2/climate");
climate.deleteURL(1);
int count = climate.getURLCount();

climate.saveURLsToFlash();
climate.loadURLsFromFlash();
```

A URL is only accepted if it starts with `http://` or `https://` and
has a real host — `addURL()` returns `false` and sets an error if it
doesn't (see [section 20](#20-error-handling)). Up to 10 custom URLs
can be stored at once.

### From the Serial Monitor

| Command | What it does |
|---|---|
| `URL link change <name> / <url>` | Add or change a custom URL |
| `URL link add <name> / <url>` | Add a new custom URL |
| `URL link select <id>` | Make a custom URL active |
| `URL link delete <id>` | Remove a custom URL |
| `URL link list` | List every custom URL |
| `URL link test <id>` | Check that a URL is structurally valid |

Example:
```
URL link change website / https://example.com/api
```

If you try to use the reserved name `open-meteo` (or similar), you'll
see: `Protected provider: modification not allowed.` — this is
intentional, see [section 8](#8-open-meteo-architecture).

## 11. TX/RX

TX (transmit) sends data out to a URL; RX (receive) fetches data in
from a URL.

```cpp
// TX - sending data out:
climate.setTXURL("https://example.com/api/upload");
climate.enableTX();
climate.sendClimateDataTX();          // sends the current weather data as JSON
climate.sendJSON("{\"custom\":123}"); // or send your own raw JSON

// RX - fetching data in:
climate.setRXURL("https://example.com/api/download");
climate.enableRX();
climate.receiveRX();                  // fetches from the RX URL

if (climate.hasReceivedData()) {
    String data = climate.readReceivedData();
    Serial.println(data);
}
```

TX and RX are both disabled by default — call `enableTX()` /
`enableRX()` before using them. This is a deliberate safety default:
the library won't send or fetch data to/from a URL you haven't
explicitly turned on.

## 12. Live status

"Live status" is a continuously-refreshing display of the current
weather data on your Serial Monitor. It's purely a *display* feature —
turning it on does **not** make the library fetch weather data more
often. How often you see fresh data and how often the library actually
checks Open-Meteo are two separate, independent timers.

### From the Serial Monitor

```
live status
```

This prints the full current status, then keeps refreshing it
automatically (every 5 seconds by default). Every field that isn't
currently available shows `N/A`. If the data hasn't been refreshed
recently, you'll see a `[STALE]` marker.

```
live status stop
```

Stops the continuous refresh immediately. Nothing else is affected —
your WiFi connection, the weather-update scheduler, any connected
sensors, and any platform integrations (MQTT, etc.) all keep running
exactly as before.

### From your sketch (code)

Live status is driven through the Serial command interface rather
than exposed as a direct function on the main `AmelTechClimate`
object in this version. If you need to check or control it
programmatically from your own code, see `src/serial/LiveStatus.h` —
that's the class the library uses internally, and its functions
(`startLiveStatus()`, `stopLiveStatus()`, `isLiveStatusRunning()`,
`setLiveStatusInterval()`) are usable directly if you construct your
own instance.

## 13. Sensors

You can plug in a physical sensor and read local temperature,
humidity, and (depending on the sensor) pressure. Four sensor types
are supported. **None of these are required** — the built-in weather
data works completely fine with no sensor connected at all.

| Sensor | Measures | Library you need to install |
|---|---|---|
| DHT11 | Temperature, humidity | Adafruit "DHT sensor library" + "Adafruit Unified Sensor" |
| DHT22 | Temperature, humidity (better accuracy than DHT11) | same as above |
| BME280 | Temperature, humidity, pressure | "Adafruit BME280 Library" + Adafruit Unified Sensor + Adafruit BusIO |
| BME680 | Temperature, humidity, pressure | "Adafruit BME680 Library" + same Adafruit dependencies |
| SHT31 | Temperature, humidity (high accuracy) | "Adafruit SHT31 Library" + same Adafruit dependencies |

### Example: wiring up a DHT22

```cpp
#include <AmelTech_Climate_plug.h>
#include <sensors/DHTAdapter.h>

AmelTech::AmelTechClimate climate;
AmelTech::DHTAdapter myDHT(4, true); // GPIO pin 4, "true" = DHT22 (use "false" for DHT11)

void setup() {
    Serial.begin(115200);
    climate.begin("YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD");

    climate.registerSensor(&myDHT);
    climate.scanSensors(); // initializes every registered sensor
}

void loop() {
    climate.loop();
    // see section 14 for how to actually use sensor readings
}
```

### Example: wiring up a BME280

```cpp
#include <sensors/BME280Adapter.h>

AmelTech::BME280Adapter myBME(0x76); // I2C address - usually 0x76 or 0x77
// ... same registerSensor()/scanSensors() pattern as above
```

### From the Serial Monitor

| Command | What it does |
|---|---|
| `sensor scan` | Initialize every registered sensor |
| `sensor status` | Show the status of every registered sensor |

## 14. Hybrid mode

Hybrid mode lets you combine live Open-Meteo weather data with your
own local sensor readings — useful because a sensor sitting right next
to your ESP32 is often more accurate for *your exact spot* than a
regional weather forecast.

```cpp
climate.registerSensor(&myDHT);
climate.scanSensors();

climate.setMode(AmelTech::DataSource::HYBRID);
// or: climate.enableHybrid();

climate.readAllSensorsIntoLocalData(); // takes a fresh sensor reading

float temp = climate.getTemperature(); // now prefers your sensor's reading, falls back to Open-Meteo for anything your sensor doesn't measure
```

Three modes are available:

```cpp
climate.useRemoteData();  // REMOTE - Open-Meteo only (the default)
climate.useLocalData();   // LOCAL - your sensor(s) only
climate.enableHybrid();   // HYBRID - blend both
```

When blending, your local sensor's readings take priority for any
field it measures (like temperature), and Open-Meteo fills in
everything else (like wind speed, which no supported sensor here
measures).

**Note:** `readAllSensorsIntoLocalData()` takes a fresh reading each
time you call it — it doesn't happen automatically in the background.
Call it as often as makes sense for your sensor (e.g. once every few
seconds in your `loop()`).

## 15. Blynk integration

[Blynk](https://blynk.io) is an app-based IoT dashboard. This
integration is **entirely optional** — if you don't use Blynk, none of
this code runs or is required.

You'll need to install the "Blynk" library, and define three macros
*before* including this library's Blynk headers (Blynk itself requires
this):

```cpp
#define BLYNK_TEMPLATE_ID "your_template_id"
#define BLYNK_TEMPLATE_NAME "your_template_name"
#define BLYNK_AUTH_TOKEN "your_auth_token"

#include <AmelTech_Climate_plug.h>
#include <platforms/BlynkAdapter.h>

AmelTech::AmelTechClimate climate;
AmelTech::BlynkAdapter blynk;

void setup() {
    Serial.begin(115200);
    climate.begin("YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD");

    blynk.begin(BLYNK_AUTH_TOKEN, "YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD");

    // Map climate fields to Blynk "virtual pins":
    blynk.mapTemperature(0); // V0
    blynk.mapHumidity(1);    // V1

    climate.attachBlynkAdapter(&blynk);
    climate.enablePlatform(AmelTech::PlatformType::BLYNK);
}

void loop() {
    climate.loop();
    // Periodically:
    climate.publishToAllPlatforms(); // sends current data to every enabled, connected platform - including Blynk
}
```

Only fields you've explicitly mapped with a `mapX()` call get sent —
anything unmapped is simply skipped, never sent as a fake `0`.

## 16. MQTT integration

Also **entirely optional**. You'll need the "PubSubClient" library by
Nick O'Leary.

```cpp
#include <AmelTech_Climate_plug.h>
#include <platforms/MQTTAdapter.h>

AmelTech::AmelTechClimate climate;
AmelTech::MQTTAdapter mqtt;

void setup() {
    Serial.begin(115200);
    climate.begin("YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD");

    mqtt.setBroker("YOUR_MQTT_SERVER");
    mqtt.setPort(1883);
    // mqtt.setUsername("..."); mqtt.setPassword("..."); // if your broker needs auth
    mqtt.begin();
    mqtt.connect();

    climate.attachMQTTAdapter(&mqtt);
    climate.enablePlatform(AmelTech::PlatformType::MQTT);
}

void loop() {
    climate.loop(); // this also drives mqtt.update() automatically once attached
    climate.publishToAllPlatforms();
}
```

## 17. REST integration

REST works with **no extra library** — it's built entirely on the
same HTTP code the built-in weather provider uses.

```cpp
climate.setRESTEndpoint("https://example.com/api/climate");
climate.enableRESTPlatform();

climate.publishToAllPlatforms(); // sends current weather data as JSON to your REST endpoint
```

## 18. Webhook integration

Also works with **no extra library**. Webhooks are meant for
fire-and-forget event notifications rather than an ongoing data feed.

```cpp
climate.setWebhookURL("https://example.com/webhook");
climate.enableWebhookPlatform();

climate.triggerWebhook(); // fires a simple ping
```

## 19. Cache

The library keeps the most recent weather data cached in memory (and
optionally on flash), so a temporary WiFi hiccup doesn't leave you
with nothing.

```cpp
climate.enableCache();  // on by default
climate.setCacheLifetime(1800000); // 30 minutes, in milliseconds

bool fresh = climate.isCacheValid();
unsigned long ageMs = climate.getCacheAge();

climate.clearCache();
climate.forceRefresh(); // ignore the cache and force a real fetch next update
```

Data is always shown honestly: if it's past its lifetime, it's marked
stale rather than presented as if it were fresh — this library never
pretends old data is current.

## 20. Error handling

Almost every function that can fail sets an internal error rather than
crashing or silently doing nothing.

```cpp
if (climate.hasError()) {
    Serial.println("Error: " + climate.getErrorMessage());
    AmelTech::ErrorCode code = climate.getErrorCode();
    climate.clearError();
}
```

Some of the error codes you might see: `WIFI_TIMEOUT`, `URL_INVALID`,
`HTTP_TIMEOUT`, `DATA_UNAVAILABLE`, `LOCATION_INVALID`,
`SENSOR_NOT_FOUND`, `PROVIDER_ERROR`, `PLATFORM_ERROR`,
`STORAGE_ERROR`. The full list is in `src/diagnostics/ErrorManager.h`.

**Why getters can return `NAN` or `-1`:** if a particular weather
field simply isn't available (not every location has every
measurement), the getter for it returns `NAN` (for numbers) or `-1`
(for the weather code) and sets `DATA_UNAVAILABLE`. This library
deliberately never fills in a fake `0` for missing data — a `0` could
easily be mistaken for a real reading of zero.

## 21. Diagnostics

You can check several parts of the system's status individually:

```cpp
String providerStatus = climate.getProviderStatus();
String platformStatus = climate.getPlatformStatus();
bool wifiConnected = climate.isWiFiConnected();
int rssi = climate.getRSSI(); // WiFi signal strength
```

From the Serial Monitor: `network status`, `provider status`,
`platform status`, and `cache status` each show a focused summary.

**Honest gap:** a single combined `diagnostics` command (and a
`statistics` command tracking things like total request counts) are
listed in this library's original design but haven't been built yet —
typing either into the Serial Monitor will tell you plainly that it's
not yet implemented, rather than pretending to run. See
[PENDING_MODULES.md](PENDING_MODULES.md).

## 22. Security

- **WiFi passwords are never printed to the Serial Monitor.** Status
  and diagnostic output never includes raw credentials.
- Text you type in (location names, custom URL names) is cleaned up
  before being stored — stray control characters are stripped.
- Custom URLs are checked for a valid `http://`/`https://` scheme and
  a real host before being accepted.
- If you store other credentials (like an MQTT password) using
  `climate.setCredential(...)`, you can display a safe masked version
  with `climate.getMaskedCredential(...)` instead of the real value.

One thing worth knowing: on ESP32, the flash storage this library uses
(NVS) is **not encrypted by default**. If you're storing sensitive
values and need real protection at rest, look into ESP32's flash
encryption feature at the board/partition level — that's outside what
this library alone controls.

## 23. External-service usage limitations

This library does **not** provide unlimited access to Open-Meteo or
any other external service. You can install this library on as many
ESP32 devices as you'd like, but the actual number of requests you can
make remains governed by Open-Meteo's own terms of service, rate
limits, and fair-use policy — the same as if you were calling their
API directly yourself. This library doesn't do anything to get around
those limits.

If you're deploying at any real scale (more than a handful of
devices), please check Open-Meteo's current terms directly.

## 24. Troubleshooting

**"Nothing happens after `begin()`"** — Make sure you're calling
`climate.loop()` on every single iteration of your sketch's `loop()`.
Nothing in this library runs on its own without that call.

**"`getTemperature()` always returns `nan`"** — Check
`climate.isDataValid()` first; if it's `false`, no successful weather
update has completed yet. Also check `climate.hasError()` for a
specific reason (often `WIFI_TIMEOUT` if the board never connected).

**"I get 'Protected provider: modification not allowed'"** — You tried
to name a custom URL something like `open-meteo`. This is intentional
— see [section 8](#8-open-meteo-architecture). Pick a different name.

**"My custom URL won't save"** — `addURL()` requires the URL to start
with `http://` or `https://` and include a real host. Double-check for
typos, and that you haven't hit the 10-URL limit.

**"A Serial command says 'not yet implemented'"** — A small number of
commands genuinely aren't built yet (`sensor read`, `diagnostics`,
`statistics`). This is expected, not a bug — see
[PENDING_MODULES.md](PENDING_MODULES.md) for the exact list.

**"I don't see any example sketches besides 01_Basic"** — Correct,
only that one is written so far. The other 15 folders under
`examples/` currently contain a short note pointing you to the
relevant source file instead of working code. See
[PENDING_MODULES.md](PENDING_MODULES.md).

## 25. API reference

A full, dedicated `docs/API.md` with every function documented
individually hasn't been written yet (see
[PENDING_MODULES.md](PENDING_MODULES.md)). Until then, the most
accurate reference is the source itself:

- **`src/AmelTech_Climate_plug.h`** — every public function on the
  main `AmelTechClimate` object, grouped by feature area, each marked
  `[WIRED]` (works today) or `[PENDING]` (not yet built).
- Each subsystem's own header (e.g. `src/location/LocationManager.h`,
  `src/urls/URLManager.h`) documents that specific area in more
  depth, including function-by-function comments.

## 26. License

MIT — see [LICENSE](LICENSE), which also includes an important note
about Open-Meteo being a separate, independently-operated service not
covered by this project's license.

---

For the exact, current list of what is and isn't implemented across
the whole library, see [PENDING_MODULES.md](PENDING_MODULES.md). For
guidance on modifying the code yourself, see
[CONTRIBUTING.md](CONTRIBUTING.md).
