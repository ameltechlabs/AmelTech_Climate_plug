/*
 * InputSanitizer.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "InputSanitizer.h"

namespace AmelTech {

InputSanitizer::InputSanitizer() {
}

String InputSanitizer::sanitize(const String& input) const {
    String out = "";
    out.reserve(input.length());

    for (size_t i = 0; i < input.length(); i++) {
        char c = input[i];
        // Strip all control characters (0x00-0x1F, 0x7F) - these
        // config fields are single-line values with no legitimate
        // use for tabs, newlines, or other control bytes.
        if ((unsigned char)c < 0x20 || (unsigned char)c == 0x7F) {
            continue;
        }
        out += c;
    }

    out.trim();
    return out;
}

String InputSanitizer::truncate(const String& input, size_t maxLength) const {
    if (input.length() <= maxLength) return input;
    return input.substring(0, maxLength);
}

String InputSanitizer::clean(const String& input, size_t maxLength) const {
    return truncate(sanitize(input), maxLength);
}

bool InputSanitizer::isSafeForJSON(const String& input) const {
    for (size_t i = 0; i < input.length(); i++) {
        char c = input[i];
        // Raw unescaped quotes or backslashes would break a
        // hand-assembled JSON string literal.
        if (c == '"' || c == '\\') return false;
        if ((unsigned char)c < 0x20) return false;
    }
    return true;
}

bool InputSanitizer::isSafeForSerial(const String& input) const {
    for (size_t i = 0; i < input.length(); i++) {
        char c = input[i];
        if (c == '\n' || c == '\r') return false;
    }
    return true;
}

} // namespace AmelTech
