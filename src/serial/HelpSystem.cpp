/*
 * HelpSystem.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "HelpSystem.h"

namespace AmelTech {

HelpSystem::HelpSystem() {
}

String HelpSystem::render(const CommandRegistry& registry) const {
    String out = "";
    out += "==================================================\n";
    out += "AmelTech_Climate_plug - Command Reference\n";
    out += "==================================================\n\n";

    out += "-- AVAILABLE NOW --\n";
    for (uint8_t i = 0; i < registry.count(); i++) {
        const CommandEntry& e = registry.at(i);
        if (e.status == CommandStatus::IMPLEMENTED) {
            out += "  " + e.name + "\n      " + e.description + "\n";
        }
    }

    out += "\n-- PLANNED (not yet implemented) --\n";
    for (uint8_t i = 0; i < registry.count(); i++) {
        const CommandEntry& e = registry.at(i);
        if (e.status == CommandStatus::PENDING) {
            out += "  " + e.name + "  [pending]\n      " + e.description + "\n";
        }
    }

    out += "\nType a command and press Enter.\n";
    return out;
}

} // namespace AmelTech
