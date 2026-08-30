/*
 * HelpSystem.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * Renders the `help` command's output from whatever is currently in
 * the CommandRegistry, grouped by IMPLEMENTED vs PENDING so the user
 * always sees an accurate picture of what works right now.
 */

#ifndef AMELTECH_HELP_SYSTEM_H
#define AMELTECH_HELP_SYSTEM_H

#include <Arduino.h>
#include "CommandRegistry.h"

namespace AmelTech {

class HelpSystem {
public:
    HelpSystem();

    String render(const CommandRegistry& registry) const;
};

} // namespace AmelTech

#endif // AMELTECH_HELP_SYSTEM_H
