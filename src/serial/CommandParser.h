/*
 * CommandParser.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * Splits a raw Serial line into a command name + argument tokens.
 * Deliberately simple (whitespace-split, case-insensitive command
 * matching) rather than a full grammar, since spec commands are all
 * fixed short phrases like "live status stop" or "location change".
 * Multi-word commands (e.g. "URL link change") are matched by the
 * CommandRegistry trying progressively shorter prefixes - see
 * CommandRegistry.cpp.
 */

#ifndef AMELTECH_COMMAND_PARSER_H
#define AMELTECH_COMMAND_PARSER_H

#include <Arduino.h>

namespace AmelTech {

struct ParsedCommand {
    String raw;             // original trimmed line
    String normalized;      // lowercased, single-spaced
};

class CommandParser {
public:
    CommandParser();

    // Trims and collapses internal whitespace; lowercases into
    // `normalized` while preserving original casing in `raw` (values
    // like location names or URLs need original casing preserved by
    // the caller, which re-parses raw itself when needed).
    ParsedCommand parse(const String& line) const;

private:
    String collapseWhitespace(const String& input) const;
};

} // namespace AmelTech

#endif // AMELTECH_COMMAND_PARSER_H
