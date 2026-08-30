/*
 * CommandParser.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "CommandParser.h"

namespace AmelTech {

CommandParser::CommandParser() {
}

String CommandParser::collapseWhitespace(const String& input) const {
    String out = "";
    bool lastWasSpace = false;
    for (size_t i = 0; i < input.length(); i++) {
        char c = input[i];
        if (c == ' ' || c == '\t') {
            if (!lastWasSpace && out.length() > 0) {
                out += ' ';
            }
            lastWasSpace = true;
        } else {
            out += c;
            lastWasSpace = false;
        }
    }
    // Trim trailing space left by collapse logic.
    while (out.length() > 0 && out[out.length() - 1] == ' ') {
        out.remove(out.length() - 1);
    }
    return out;
}

ParsedCommand CommandParser::parse(const String& line) const {
    ParsedCommand result;

    String trimmed = line;
    trimmed.trim();
    result.raw = collapseWhitespace(trimmed);

    String lower = result.raw;
    lower.toLowerCase();
    result.normalized = lower;

    return result;
}

} // namespace AmelTech
