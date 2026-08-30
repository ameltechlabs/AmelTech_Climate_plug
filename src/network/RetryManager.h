/*
 * RetryManager.h
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 *
 * Tracks retry count/delay for a single in-flight request sequence.
 * Enforces a hard maximum retry count so the library can NEVER retry
 * forever (spec: "Never create an infinite retry loop" under RETRY
 * ENGINE, and Rule 8: network failures must not freeze the ESP32).
 *
 * This class does not itself delay/block - it only tracks state and
 * tells the caller (RequestManager) WHEN it's OK to retry via
 * shouldRetryNow(), which callers should check non-blockingly from
 * loop()/update(), the same way ClimateScheduler is polled.
 */

#ifndef AMELTECH_RETRY_MANAGER_H
#define AMELTECH_RETRY_MANAGER_H

#include <Arduino.h>

namespace AmelTech {

enum class BackoffMode {
    FIXED,
    LINEAR,
    EXPONENTIAL
};

class RetryManager {
public:
    RetryManager();

    void setRetryCount(uint8_t maxRetries);
    uint8_t getRetryCount() const;

    void setRetryDelay(unsigned long ms);
    unsigned long getRetryDelay() const;

    void setBackoff(BackoffMode mode);
    BackoffMode getBackoff() const;

    // Call when a request attempt fails. Returns true if a retry is
    // still permitted (i.e. retries used < max); false means give up.
    bool retryRequest();

    void resetRetryState();

    uint8_t getRetryCountUsed() const;

    // Non-blocking readiness check: has enough time passed since the
    // last failed attempt (per current backoff) to retry now?
    bool shouldRetryNow() const;

    // millis() timestamp of when the next retry becomes eligible.
    unsigned long nextRetryAt() const;

private:
    static const uint8_t HARD_MAX_RETRIES = 10; // absolute ceiling, spec-mandated safety net

    uint8_t _maxRetries;
    unsigned long _baseDelayMs;
    BackoffMode _backoffMode;

    uint8_t _retriesUsed;
    unsigned long _lastFailureAt;

    unsigned long computeDelay() const;
};

} // namespace AmelTech

#endif // AMELTECH_RETRY_MANAGER_H
