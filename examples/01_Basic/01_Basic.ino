/*
 * 01_Basic.ino
 * AmelTech_Climate_plug - Basic Example
 *
 * Minimal working example: connects to WiFi, sets a location, and
 * prints temperature/humidity once data arrives. This matches the
 * minimal usage example in the library's README and spec.
 *
 * Replace YOUR_WIFI_SSID / YOUR_WIFI_PASSWORD with your own network
 * credentials before uploading. Never commit real credentials to a
 * public repository - see the library's .gitignore for a suggested
 * secrets.h pattern for larger projects.
 */

#include <AmelTech_Climate_plug.h>

AmelTech::AmelTechClimate climate;

const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

void setup() {
    Serial.begin(115200);
    delay(1000); // give the Serial Monitor time to attach

    Serial.println("AmelTech_Climate_plug - Basic Example");
    Serial.println("Connecting to WiFi...");

    bool ready = climate.begin(WIFI_SSID, WIFI_PASSWORD);

    if (!ready) {
        Serial.println("Warning: library did not reach READY state during begin().");
        Serial.println("Check WiFi credentials. The library will keep retrying in the background.");
    }

    // Set the active location. Replace with your own coordinates.
    // (Kerala, Thrissur, India - used only as a placeholder example.)
    climate.setLocation("Example Location", 10.5276, 76.2144);

    Serial.println("Setup complete. Waiting for first climate data update...");
    Serial.println("Type 'help' in the Serial Monitor at any time for available commands.");
}

void loop() {
    // Required every iteration: drives WiFi maintenance, the
    // non-blocking climate update scheduler, and Serial command
    // handling. Never blocks for long.
    climate.loop();

    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 15000) { // print a status line every 15s, not every loop() iteration
        lastPrint = millis();

        if (climate.isUpdating()) {
            Serial.println("Update in progress...");
        } else if (climate.isDataValid()) {
            float temp = climate.getTemperature();
            float humidity = climate.getRelativeHumidity();

            Serial.print("Temperature: ");
            if (isnan(temp)) {
                Serial.println("N/A");
            } else {
                Serial.print(temp);
                Serial.println(" C");
            }

            Serial.print("Humidity: ");
            if (isnan(humidity)) {
                Serial.println("N/A");
            } else {
                Serial.print(humidity);
                Serial.println(" %");
            }
        } else if (climate.hasError()) {
            Serial.println("Error: " + climate.getErrorMessage());
        } else {
            Serial.println("No valid data yet - waiting for first update.");
        }
    }
}
