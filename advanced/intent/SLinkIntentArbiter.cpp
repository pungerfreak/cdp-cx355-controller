#include "intent/SLinkIntentArbiter.h"

const SLinkIntentArbiter::Policy& SLinkIntentArbiter::policyFor(
    SLinkIntentType type) {
  constexpr uint8_t kPriorityLow = 1;
  constexpr uint8_t kPriorityMedium = 2;
  constexpr uint8_t kPriorityHigh = 3;

  constexpr uint32_t kThrottleTransportMs = 250;
  constexpr uint32_t kThrottlePowerMs = 500;
  constexpr uint32_t kThrottleSelectMs = 750;
  constexpr uint32_t kThrottleStatusMs = 500;

  constexpr uint32_t kExpireTransportMs = 5000;
  constexpr uint32_t kExpirePowerMs = 8000;
  constexpr uint32_t kExpireChangeDiscMs = 8000;
  constexpr uint32_t kExpireChangeTrackMs = 5000;
  constexpr uint32_t kExpireGetCurrentDiscMs = 4000;

  static const Policy kPolicies[] = {
      {kPriorityMedium, kThrottleTransportMs, kExpireTransportMs},  // Play
      {kPriorityHigh, kThrottleTransportMs, kExpireTransportMs},    // Stop
      {kPriorityMedium, kThrottleTransportMs, kExpireTransportMs},  // Pause
      {kPriorityHigh, kThrottlePowerMs, kExpirePowerMs},            // PowerOn
      {kPriorityHigh, kThrottlePowerMs, kExpirePowerMs},            // PowerOff
      {kPriorityLow, kThrottleSelectMs, kExpireChangeDiscMs},       // ChangeDisc
      {kPriorityLow, kThrottleSelectMs, kExpireChangeTrackMs},      // ChangeTrack
      {kPriorityHigh, kThrottleStatusMs, kExpireGetCurrentDiscMs}   // GetCurrentDisc
  };
  static_assert(sizeof(kPolicies) / sizeof(kPolicies[0]) ==
                    SLinkIntentArbiter::kIntentTypeCount,
                "Intent policy table size mismatch");
  uint8_t index = static_cast<uint8_t>(type);
  if (index >= kIntentTypeCount) {
    static const Policy kDefault{1, 250, 0};
    return kDefault;
  }
  return kPolicies[index];
}

bool SLinkIntentArbiter::shouldExpire(uint32_t now,
                                      const Policy& policy,
                                      uint32_t enqueuedAt) {
  return policy.expireMs > 0 && (now - enqueuedAt) > policy.expireMs;
}

bool SLinkIntentArbiter::isThrottled(uint32_t now,
                                     const Policy& policy,
                                     uint32_t lastDispatched) {
  return policy.throttleMs > 0 && lastDispatched != 0 &&
         (now - lastDispatched) < policy.throttleMs;
}

namespace {
struct IntentCandidate {
  uint8_t offset = 0;
  uint8_t priority = 0;
  uint32_t enqueuedAt = 0;
};

bool isBetterCandidate(const IntentCandidate& candidate,
                       const IntentCandidate& best) {
  if (candidate.priority != best.priority) {
    return candidate.priority > best.priority;
  }
  return candidate.enqueuedAt < best.enqueuedAt;
}
}  // namespace

bool SLinkIntentArbiter::selectNext(SLinkIntentQueue& queue,
                                    SLinkCommandIntent& intent,
                                    uint8_t& offsetOut) {
  if (queue.isEmpty()) return false;

  const uint32_t now = millis();

  for (uint8_t offset = 0; offset < queue.size();) {
    SLinkCommandIntent candidate;
    uint32_t enqueuedAt = 0;
    if (!queue.peekAt(offset, candidate, enqueuedAt)) break;
    const Policy& policy = policyFor(candidate.type);
    if (shouldExpire(now, policy, enqueuedAt)) {
      SLinkCommandIntent dropped;
      uint32_t droppedAt = 0;
      queue.removeAt(offset, dropped, droppedAt);
      continue;
    }
    ++offset;
  }

  if (queue.isEmpty()) return false;

  IntentCandidate best;
  bool found = false;

  for (uint8_t offset = 0; offset < queue.size(); ++offset) {
    SLinkCommandIntent candidate;
    uint32_t enqueuedAt = 0;
    if (!queue.peekAt(offset, candidate, enqueuedAt)) break;
    const Policy& policy = policyFor(candidate.type);
    uint8_t index = static_cast<uint8_t>(candidate.type);
    uint32_t last = (index < kIntentTypeCount) ? _lastDispatchMs[index] : 0;
    if (isThrottled(now, policy, last)) {
      continue;
    }

    IntentCandidate scored{offset, policy.priority, enqueuedAt};
    if (!found || isBetterCandidate(scored, best)) {
      best = scored;
      found = true;
    }
  }

  if (!found) return false;

  uint32_t enqueuedAt = 0;
  if (!queue.peekAt(best.offset, intent, enqueuedAt)) return false;
  offsetOut = best.offset;
  return true;
}

void SLinkIntentArbiter::noteDispatched(SLinkIntentType type, uint32_t now) {
  uint8_t index = static_cast<uint8_t>(type);
  if (index < kIntentTypeCount) {
    _lastDispatchMs[index] = now;
  }
}
