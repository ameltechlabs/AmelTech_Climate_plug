/*
 * RetryManager.cpp
 * AmelTech_Climate_plug
 * PHASE 1 - IMPLEMENTED
 */

#include "RetryManager.h"

namespace AmelTech {

RetryManager::RetryManager()
    : _maxRetries(3),
      _baseDelayMs(2000),
      _backoffMode(BackoffMode::EXPONENTIAL),
      _retriesUsed(0),
      _lastFailureAt(0) {
}

void RetryManager::setRetryCount(uint8_t maxRetries) {
    // Clamp to HARD_MAX_RETRIES no matter what the caller requests -
    // this is the enforcement point for "never infinite retries".
    _maxRetries = (maxRetries > HARD_MAX_RETRIES) ? HARD_MAX_RETRIES : maxRetries;
}

uint8_t RetryManager::getRetryCount() const {
    return _maxRetries;
}

void RetryManager::setRetryDelay(unsigned long ms) {
    _baseDelayMs = ms;
}

unsigned long RetryManager::getRetryDelay() const {
    return _baseDelayMs;
}

void RetryManager::setBackoff(BackoffMode mode) {
    _backoffMode = mode;
}

BackoffMode RetryManager::getBackoff() const {
    return _backoffMode;
}

unsigned long RetryManager::computeDelay() const {
    switch (_backoffMode) {
        case BackoffMode::FIXED:
            return _baseDelayMs;
        case BackoffMode::LINEAR:
            return _baseDelayMs * (_retriesUsed + 1);
        case BackoffMode::EXPONENTIAL: {
            // Cap the shift to avoid overflow on many retries; HARD_MAX_RETRIES
            // is small (10) so this never realistically triggers, but the
            // clamp keeps the arithmetic well-defined regardless.
            uint8_t shift = (_retriesUsed > 6) ? 6 : _retriesUsed;
            return _baseDelayMs * (1UL << shift);
        }
        default:
            return _baseDelayMs;
    }
}

bool RetryManager::retryRequest() {
    _lastFailureAt = millis();
    if (_retriesUsed >= _maxRetries) {
        return false; // exhausted - caller must surface a final failure, not loop
    }
    _retriesUsed++;
    return true;
}

void RetryManager::resetRetryState() {
    _retriesUsed = 0;
    _lastFailureAt = 0;
}

uint8_t RetryManager::getRetryCountUsed() const {
    return _retriesUsed;
}

bool RetryManager::shouldRetryNow() const {
    if (_retriesUsed == 0 || _retriesUsed > _maxRetries) return false;
    return millis() - _lastFailureAt >= computeDelay();
}

unsigned long RetryManager::nextRetryAt() const {
    return _lastFailureAt + computeDelay();
}

} // namespace AmelTech
