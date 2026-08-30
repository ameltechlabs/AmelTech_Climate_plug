/*
 * CommandRegistry.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "CommandRegistry.h"

namespace AmelTech {

CommandRegistry::CommandRegistry() : _count(0) {
}

void CommandRegistry::registerCommand(const String& name, const String& description, CommandHandler handler) {
    if (_count >= MAX_COMMANDS) return; // silently ignore overflow past the fixed capacity; library-internal invariant, never hit in practice
    _entries[_count].name = name;
    _entries[_count].description = description;
    _entries[_count].status = CommandStatus::IMPLEMENTED;
    _entries[_count].handler = handler;
    _count++;
}

void CommandRegistry::registerPending(const String& name, const String& description) {
    if (_count >= MAX_COMMANDS) return;
    _entries[_count].name = name;
    _entries[_count].description = description;
    _entries[_count].status = CommandStatus::PENDING;
    _entries[_count].handler = nullptr;
    _count++;
}

const CommandEntry* CommandRegistry::findMatch(const String& normalized) const {
    const CommandEntry* best = nullptr;
    size_t bestLength = 0;

    for (uint8_t i = 0; i < _count; i++) {
        const String& name = _entries[i].name;
        bool isExact = (normalized == name);
        bool isPrefixMatch = normalized.startsWith(name + " ");

        if ((isExact || isPrefixMatch) && name.length() > bestLength) {
            best = &_entries[i];
            bestLength = name.length();
        }
    }

    return best;
}

uint8_t CommandRegistry::count() const {
    return _count;
}

const CommandEntry& CommandRegistry::at(uint8_t index) const {
    return _entries[index];
}

} // namespace AmelTech
