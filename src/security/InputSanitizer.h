/*
 * InputSanitizer.h
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 *
 * Sanitizes free-text input arriving from Serial (location names,
 * custom URL names, MQTT topic strings, etc.) before it's stored or
 * used to build requests. Distinct from URLValidator (which checks
 * URL *structure*) - this strips/rejects characters that have no
 * legitimate place in stored config strings regardless of field.
 */

#ifndef AMELTECH_INPUT_SANITIZER_H
#define AMELTECH_INPUT_SANITIZER_H

#include <Arduino.h>

namespace AmelTech {

class InputSanitizer {
public:
    InputSanitizer();

    // Removes control characters (0x00-0x1F except none allowed,
    // since these are single-line config fields) and trims
    // leading/trailing whitespace. Safe to call on any free-text
    // field before storing it.
    String sanitize(const String& input) const;

    // Truncates to maxLength if longer (spec: bounded storage for
    // names/URLs). Applied AFTER sanitize() in the typical pipeline.
    String truncate(const String& input, size_t maxLength) const;

    // Convenience: sanitize() + truncate() in one call, the standard
    // pipeline for any incoming free-text field.
    String clean(const String& input, size_t maxLength = 128) const;

    // True if the string contains only characters safe for a JSON
    // string value without escaping concerns (used before treating
    // user input as a literal in hand-built JSON, e.g. a sensor name
    // in TXManager::sendSensorData - though ArduinoJson's own
    // serializer already escapes correctly, this is an extra guard
    // for any hand-assembled string paths).
    bool isSafeForJSON(const String& input) const;

    // True if the string contains no characters that could be used
    // for command injection into a Serial-like protocol (newlines,
    // carriage returns) - relevant when a value will later be
    // echoed back or forwarded to another serial-driven system.
    bool isSafeForSerial(const String& input) const;
};

} // namespace AmelTech

#endif // AMELTECH_INPUT_SANITIZER_H
