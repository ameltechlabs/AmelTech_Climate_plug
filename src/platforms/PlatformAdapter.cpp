/*
 * PlatformAdapter.cpp
 * AmelTech_Climate_plug
 * PHASE 2 - IMPLEMENTED
 */

#include "PlatformAdapter.h"

namespace AmelTech {

const char* platformTypeToString(PlatformType type) {
    switch (type) {
        case PlatformType::BLYNK:   return "Blynk";
        case PlatformType::MQTT:    return "MQTT";
        case PlatformType::REST:    return "REST";
        case PlatformType::WEBHOOK: return "Webhook";
        default:                    return "UNKNOWN";
    }
}

} // namespace AmelTech
