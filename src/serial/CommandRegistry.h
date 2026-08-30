/*
 * CommandRegistry.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED (registry mechanism + P1 command set real;
 * PENDING commands are registered with a stub that reports their
 * status honestly rather than being silently absent)
 *
 * Holds the full list of commands from the spec's "SERIAL COMMANDS"
 * section. Each entry has a CommandStatus so `help` can accurately
 * show what's implemented vs. planned, per the "no fake functions"
 * rule - an unimplemented command says so instead of pretending.
 */

#ifndef AMELTECH_COMMAND_REGISTRY_H
#define AMELTECH_COMMAND_REGISTRY_H

#include <Arduino.h>
#include <functional>

namespace AmelTech {

enum class CommandStatus {
    IMPLEMENTED,
    PENDING
};

// Handler receives the full normalized command line plus the raw
// (original-case) line for commands that need literal text (URLs,
// location names). Returns the text to print to Serial.
typedef std::function<String(const String& normalized, const String& raw)> CommandHandler;

struct CommandEntry {
    String name;              // canonical command text, e.g. "live status stop"
    String description;
    CommandStatus status;
    CommandHandler handler;   // only called if status == IMPLEMENTED
};

class CommandRegistry {
public:
    static const uint8_t MAX_COMMANDS = 40;

    CommandRegistry();

    // Registers a fully-working command.
    void registerCommand(const String& name, const String& description, CommandHandler handler);

    // Registers a command name for API-completeness/discoverability
    // (so `help` lists it) without a working handler yet.
    void registerPending(const String& name, const String& description);

    // Finds the LONGEST registered command name that is a prefix of
    // `normalized` (so "location change kerala..." matches the
    // "location change" entry, not a shorter unrelated one).
    // Returns nullptr if nothing matches.
    const CommandEntry* findMatch(const String& normalized) const;

    uint8_t count() const;
    const CommandEntry& at(uint8_t index) const;

private:
    CommandEntry _entries[MAX_COMMANDS];
    uint8_t _count;
};

} // namespace AmelTech

#endif // AMELTECH_COMMAND_REGISTRY_H
