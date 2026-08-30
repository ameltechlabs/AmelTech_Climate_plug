/*
 * ClimateScheduler.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * Decides WHEN the next provider request should fire, using
 * millis()-based comparison (never delay()). Deliberately knows
 * nothing about Live Status - per spec RULE 3 ("Live Status is a
 * presentation layer, not an uncontrolled API polling mechanism")
 * and the "IMPORTANT" note under SCHEDULER, this class is the ONLY
 * thing that paces provider requests. LiveStatus (serial/LiveStatus.*)
 * has its own independent display-refresh timer and never calls
 * back into this scheduler to force an early update.
 */

#ifndef AMELTECH_CLIMATE_SCHEDULER_H
#define AMELTECH_CLIMATE_SCHEDULER_H

#include <Arduino.h>

namespace AmelTech {

class ClimateScheduler {
public:
    ClimateScheduler();

    void setUpdateInterval(unsigned long ms);
    unsigned long getUpdateInterval() const;

    void startScheduler();
    void stopScheduler();
    bool isSchedulerRunning() const;

    // Marks "an update just happened" - resets the interval countdown.
    // Called by ClimateController after a request completes (success
    // or final failure), not by RequestManager directly.
    void scheduleUpdate();

    // True if it's time for the next scheduled update. Does NOT
    // reset the timer itself - caller must call scheduleUpdate()
    // once it actually acts on this.
    bool shouldUpdate() const;

    unsigned long timeUntilNextUpdate() const;

private:
    unsigned long _intervalMs;
    unsigned long _lastUpdateAt;
    bool _running;
};

} // namespace AmelTech

#endif // AMELTECH_CLIMATE_SCHEDULER_H
